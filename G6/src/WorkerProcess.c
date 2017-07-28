#include "G6.h"

int WorkerProcess( struct ServerEnv *penv )
{
	unsigned int		forward_thread_index ;
	unsigned int		*p_forward_thread_index = NULL ;
	
	int			nret = 0 ;
	
	/* ������־����ļ� */
	UPDATEDATETIMECACHEFIRST
	InfoLog( __FILE__ , __LINE__ , "--- G6.WorkerProcess ---" );
	
	signal( SIGTERM , SIG_IGN );
	signal( SIGUSR1 , SIG_IGN );
	signal( SIGUSR2 , SIG_IGN );
	
	/* ����ʱ������߳� */
	nret = pthread_create( & (penv->time_thread_tid) , NULL , & _TimeThread , NULL ) ;
	if( nret )
	{
		ErrorLog( __FILE__ , __LINE__ , "pthread_create time thread failed , errno[%d]" , errno );
		return -1;
	}
	else
	{
		InfoLog( __FILE__ , __LINE__ , "parent_thread : [%lu] pthread_create TimeThread[%lu]" , pthread_self() , penv->time_thread_tid );
	}
	
	/* ����ת�����߳� */
	for( forward_thread_index = 0 ; forward_thread_index < penv->cmd_para.forward_thread_size ; forward_thread_index++ )
	{
		p_forward_thread_index = (unsigned int *)malloc( sizeof(unsigned int) ) ;
		if( p_forward_thread_index == NULL )
		{
			ErrorLog( __FILE__ , __LINE__ , "malloc failed , errno[%d]" , errno );
			exit(9);
		}
		(*p_forward_thread_index) = forward_thread_index ;
		
		/* ���������շ��߳� */
		nret = pthread_create( penv->forward_thread_tid_array+forward_thread_index , NULL , & _ForwardThread , (void*)p_forward_thread_index ) ;
		if( nret )
		{
			ErrorLog( __FILE__ , __LINE__ , "pthread_create forward thread failed , errno[%d]" , errno );
			return -1;
		}
		else
		{
			InfoLog( __FILE__ , __LINE__ , "parent_thread : [%lu] pthread_create ForwardThread[%lu]" , pthread_self() , penv->forward_thread_tid_array[forward_thread_index] );
		}
	}
	
	/* ���߳�Ϊ���ӽ����߳� */
	_AcceptThread( (void*)penv );
	
	/* �ȴ�����ʱ������߳� */
	InfoLog( __FILE__ , __LINE__ , "parent_thread : [%lu] pthread_join TimeThread[%lu] ..." , pthread_self() , penv->time_thread_tid );
	pthread_join( penv->time_thread_tid , NULL );
	InfoLog( __FILE__ , __LINE__ , "parent_thread : [%lu] pthread_join TimeThread[%lu] ok" , pthread_self() , penv->time_thread_tid );
	
	/* �����˳������ַ����ȴ����������շ��߳� */
	for( forward_thread_index = 0 ; forward_thread_index < penv->cmd_para.forward_thread_size ; forward_thread_index++ )
	{
		InfoLog( __FILE__ , __LINE__ , "parent_thread : [%lu] pthread_join ForwardThread[%lu] ..." , pthread_self() , penv->forward_thread_tid_array[forward_thread_index] );
		pthread_join( penv->forward_thread_tid_array[forward_thread_index] , NULL );
		UPDATEDATETIMECACHEFIRST
		InfoLog( __FILE__ , __LINE__ , "parent_thread : [%lu] pthread_join ForwardThread[%lu] ok" , pthread_self() , penv->forward_thread_tid_array[forward_thread_index] );
	}
	
	InfoLog( __FILE__ , __LINE__ , "exit" );
	
	return 0;
}

