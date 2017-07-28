#include "G6.h"

char			__G6_VERSION[] = "1.0.4" ;
char			*__G6_VERSION_1_0_4 = __G6_VERSION ;

struct ServerEnv	*g_penv = NULL ;

static void version()
{
	printf( "G6 v%s build %s %s\n" , __G6_VERSION , __DATE__ , __TIME__ );
	printf( "TCP Bridge && Load-Balance Dispenser\n" );
	printf( "Copyright by calvin 2016\n" );
	return;
}

static void usage()
{
	printf( "USAGE : G6 -f (config_pathfilename) [ -t (forward_thread_size) ] [ -s (forward_session_size) ] [ --log-level (DEBUG|INFO|WARN|ERROR|FATAL) ] [ --log-filename (logfilename) ] [ --close-log ] [ --no-daemon ] [ --set-cpu-affinity ]\n" );
	return;
}

int main( int argc , char *argv[] )
{
	struct ServerEnv	env , *penv = & env ;
	int			n ;
	
	int			nret = 0 ;
	
	/* ���ñ�׼����޻��� */
	setbuf( stdout , NULL );
	
	/* ������������� */
	srand( (unsigned)time(NULL) );
	
	/* ��ʼ������������ */
	memset( penv , 0x00 , sizeof(struct ServerEnv) );
	g_penv = penv ;
	
	/* ��ʼ�������в��� */
	memset( & (penv->cmd_para) , 0x00 , sizeof(struct CommandParameter) );
	penv->cmd_para.config_pathfilename = NULL ;
	if( sysconf(_SC_NPROCESSORS_ONLN) - 2 < 1 )
		penv->cmd_para.forward_thread_size = 1 ;
	else
		penv->cmd_para.forward_thread_size = sysconf(_SC_NPROCESSORS_ONLN) - 2 ;
	penv->cmd_para.forward_session_size = DEFAULT_FORWARD_SESSIONS_MAXCOUNT ;
	penv->cmd_para.log_level = LOGLEVEL_INFO ;
	snprintf( penv->cmd_para.log_pathfilename , sizeof(penv->cmd_para.log_pathfilename)-1 , "%s/log/G6.log" , getenv("HOME") );
	penv->cmd_para.no_daemon_flag = 0 ;
	
	/* ������־����ļ� */
	SetLogFile( penv->cmd_para.log_pathfilename );
	SetLogLevel( penv->cmd_para.log_level );
	
	/* ���������в��� */
	if( argc == 1 )
	{
		version();
		usage();
		exit(7);
	}
	
	for( n = 1 ; n < argc ; n++ )
	{
		if( strcmp( argv[n] , "-v" ) == 0 && 1 + 1 == argc )
		{
			version();
			exit(0);
		}
		else if( strcmp( argv[n] , "-f" ) == 0 && n + 1 < argc )
		{
			n++;
			penv->cmd_para.config_pathfilename = argv[n] ;
		}
		else if( strcmp( argv[n] , "-t" ) == 0 && n + 1 < argc )
		{
			n++;
			penv->cmd_para.forward_thread_size = atoi(argv[n]) ;
		}
		else if( strcmp( argv[n] , "-s" ) == 0 && n + 1 < argc )
		{
			n++;
			penv->cmd_para.forward_session_size = atoi(argv[n]) ;
		}
		else if( strcmp( argv[n] , "--log-level" ) == 0 && n + 1 < argc )
		{
			n++;
			if( strcmp( argv[n] , "DEBUG" ) == 0 )
				penv->cmd_para.log_level = LOGLEVEL_DEBUG ;
			else if( strcmp( argv[n] , "INFO" ) == 0 )
				penv->cmd_para.log_level = LOGLEVEL_INFO ;
			else if( strcmp( argv[n] , "WARN" ) == 0 )
				penv->cmd_para.log_level = LOGLEVEL_WARN ;
			else if( strcmp( argv[n] , "ERROR" ) == 0 )
				penv->cmd_para.log_level = LOGLEVEL_ERROR ;
			else if( strcmp( argv[n] , "FATAL" ) == 0 )
				penv->cmd_para.log_level = LOGLEVEL_FATAL ;
			else
			{
				fprintf( stderr , "invalid log level[%s]\r\n" , argv[n] );
				usage();
				exit(7);
			}
			
			SetLogLevel( penv->cmd_para.log_level );
		}
		else if( strcmp( argv[n] , "--log-filename" ) == 0 && n + 1 < argc )
		{
			n++;
			snprintf( penv->cmd_para.log_pathfilename , sizeof(penv->cmd_para.log_pathfilename)-1 , argv[n] );
			SetLogFile( penv->cmd_para.log_pathfilename );
		}
		else if( strcmp( argv[n] , "--close-log" ) == 0 )
		{
			penv->cmd_para.close_log_flag = 1 ;
		}
		else if( strcmp( argv[n] , "--no-daemon" ) == 0 )
		{
			penv->cmd_para.no_daemon_flag = 1 ;
		}
		else if( strcmp( argv[n] , "--set-cpu-affinity" ) == 0 )
		{
			penv->cmd_para.set_cpu_affinity_flag = 1 ;
		}
		else
		{
			fprintf( stderr , "invalid opt[%s]\r\n" , argv[n] );
			usage();
			exit(7);
		}
	}
	penv->argv = argv ;
	
	if( penv->cmd_para.config_pathfilename == NULL )
	{
		usage();
		exit(7);
	}
	
	SETPID
	SETTID
	UPDATEDATETIMECACHEFIRST
	
	InfoLog( __FILE__ , __LINE__ , "--- G6 BEGIN --- v%s build %s %s" , __G6_VERSION , __DATE__ , __TIME__ );
	
	/* ��ʼ������ */
	nret = InitEnvirment( penv ) ;
	if( nret )
	{
		printf( "InitEnvirment failed[%d]\n" , nret );
		return -nret;
	}
	
	/* ���ù������� */
	/*
	...
	*/
	
	/* װ������ */
	nret = LoadConfig( penv ) ;
	if( nret )
	{
		printf( "LoadConfig failed[%d]\n" , nret );
		return -nret;
	}
	
	/* �������������˿� */
	nret = AddListeners( penv ) ;
	if( nret )
	{
		printf( "AddListeners failed[%d]\n" , nret );
		return -nret;
	}
	
	/* �����ظ����� */
	if( penv->cmd_para.no_daemon_flag )
	{
		nret = MonitorProcess( penv ) ;
	}
	else
	{
		nret = BindDaemonServer( NULL , & _MonitorProcess , penv , NULL ) ;
	}
	
	/* ж������ */
	UnloadConfig( penv );
	
	/* ������ */
	CleanEnvirment( penv );
	
	InfoLog( __FILE__ , __LINE__ , "--- G6 FINISH --- v%s build %s %s" , __G6_VERSION , __DATE__ , __TIME__ );
	
	return -nret;
}

