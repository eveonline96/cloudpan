#include "G6.h"

void *TimeThread()
{
	struct ServerEnv	*penv = g_penv ;
	
	/* ������־����ļ� */
	SetLogFile( penv->cmd_para.log_pathfilename );
	SetLogLevel( penv->cmd_para.log_level );
	SETPID
	SETTID
	InfoLog( __FILE__ , __LINE__ , "--- G6.WorkerProcess.TimeThread ---" );
	
	/* ��ʱˢ��ȫ����� */
	while( g_exit_flag == 0 )
	{
		UPDATEDATETIMECACHE
		
		sleep(1);
	}
	
	return NULL;
}

void *_TimeThread( void *pv )
{
	TimeThread();
	
	UPDATEDATETIMECACHEFIRST
	InfoLog( __FILE__ , __LINE__ , "pthread_exit" );
	pthread_exit(NULL);
}

