#include "G6.h"

void IgnoreReverseSessionEvents( struct ForwardSession *p_forward_session , struct epoll_event *p_events , int event_index , int event_count )
{
	struct ForwardSession	*p_reverse_forward_session = p_forward_session->p_reverse_forward_session ;
	struct epoll_event	*p_event = NULL ;
	
	if( event_count == 0 )
		return;
	
	for( ++event_index , p_event = p_events + event_index ; event_index < event_count ; event_index++ , p_event++ )
	{
		if( p_event->data.ptr == p_forward_session )
			continue;
		
		if( p_event->data.ptr == p_reverse_forward_session )
			p_event->events = 0 ;
	}
	
	return;
}

int OnForwardInput( struct ServerEnv *penv , struct ForwardSession *p_forward_session , int forward_epoll_fd , struct epoll_event *p_events , int event_index , int event_count , unsigned char after_accept_flag )
{
	struct ForwardSession	*p_reverse_forward_session = p_forward_session->p_reverse_forward_session ;
	int			len ;
	struct epoll_event	event ;
	
	/* ����ͨѶ���� */
	p_forward_session->io_buffer_offsetpos = 0 ;
	p_forward_session->io_buffer_len = recv( p_forward_session->sock , p_forward_session->io_buffer , DEFAULT_FORWARD_TRANSFER_BUFSIZE , 0 ) ;
	if( p_forward_session->io_buffer_len == 0 )
	{
		/* �Զ˶Ͽ����� */
		DebugLog( __FILE__ , __LINE__ , "recv #%d# closed" , p_forward_session->sock );
		return 1;
	}
	else if( p_forward_session->io_buffer_len == -1 )
	{
		if( _ERRNO == _EWOULDBLOCK )
		{
			if( after_accept_flag == 1 )
			{
				memset( & event , 0x00 , sizeof(struct epoll_event) );
				event.data.ptr = p_reverse_forward_session ;
				event.events = EPOLLIN | EPOLLERR ;
				epoll_ctl( forward_epoll_fd , EPOLL_CTL_ADD , p_reverse_forward_session->sock , & event );
				
				memset( & event , 0x00 , sizeof(struct epoll_event) );
				event.data.ptr = p_forward_session ;
				event.events = EPOLLIN | EPOLLERR ;
				epoll_ctl( forward_epoll_fd , EPOLL_CTL_ADD , p_forward_session->sock , & event );
			}
			
			return 0;
		}
		
		/* ͨѶ���ճ��� */
		ErrorLog( __FILE__ , __LINE__ , "recv #%d# failed , errno[%d]" , p_forward_session->sock , errno );
		return -1;
	}
	
	/* �Ǽ������ʱ��� */
	if( p_forward_session->p_forward_rule->load_balance_algorithm == LOAD_BALANCE_ALGORITHM_RT )
	{
		if( p_forward_session->type == FORWARD_SESSION_TYPE_CLIENT )
			p_forward_session->p_forward_rule->server_addr_array[p_forward_session->server_index].rtt = GETSECONDSTAMP ;
	}
	
	/* ����ͨѶ���� */
	len = send( p_reverse_forward_session->sock , p_forward_session->io_buffer , p_forward_session->io_buffer_len , 0 ) ;
	if( len == -1 )
	{
		if( _ERRNO == _EWOULDBLOCK )
		{
			/* ͨѶ���ͻ������� */
			IgnoreReverseSessionEvents( p_forward_session , p_events , event_index , event_count );
			
			if( after_accept_flag == 1 )
			{
				memset( & event , 0x00 , sizeof(struct epoll_event) );
				event.data.ptr = p_forward_session ;
				event.events = EPOLLERR ;
				epoll_ctl( forward_epoll_fd , EPOLL_CTL_ADD , p_forward_session->sock , & event );
				
				memset( & event , 0x00 , sizeof(struct epoll_event) );
				event.data.ptr = p_reverse_forward_session ;
				event.events = EPOLLOUT | EPOLLERR ;
				epoll_ctl( forward_epoll_fd , EPOLL_CTL_ADD , p_reverse_forward_session->sock , & event );
			}
			else
			{
				memset( & event , 0x00 , sizeof(struct epoll_event) );
				event.data.ptr = p_forward_session ;
				event.events = EPOLLERR ;
				epoll_ctl( forward_epoll_fd , EPOLL_CTL_MOD , p_forward_session->sock , & event );
				
				memset( & event , 0x00 , sizeof(struct epoll_event) );
				event.data.ptr = p_reverse_forward_session ;
				event.events = EPOLLOUT | EPOLLERR ;
				epoll_ctl( forward_epoll_fd , EPOLL_CTL_MOD , p_reverse_forward_session->sock , & event );
			}
		}
		else
		{
			/* ͨѶ���ͳ��� */
			ErrorLog( __FILE__ , __LINE__ , "send #%d# failed , errno[%d]" , p_forward_session->sock , errno );
			return -1;
		}
	}
	else if( len == p_forward_session->io_buffer_len )
	{
		/* һ��ȫ���� */
		InfoLog( __FILE__ , __LINE__ , "transfer [%s:%u]#%d# [%d]bytes to [%s:%u]#%d#" , p_forward_session->netaddr.ip , p_forward_session->netaddr.port.port_int , p_forward_session->sock , len , p_reverse_forward_session->netaddr.ip , p_reverse_forward_session->netaddr.port.port_int , p_reverse_forward_session->sock );
		DebugHexLog( __FILE__ , __LINE__ , p_forward_session->io_buffer , p_forward_session->io_buffer_len , NULL );
		p_forward_session->io_buffer_len = 0 ;
		
		if( after_accept_flag == 1 )
		{
			memset( & event , 0x00 , sizeof(struct epoll_event) );
			event.data.ptr = p_reverse_forward_session ;
			event.events = EPOLLIN | EPOLLERR ;
			epoll_ctl( forward_epoll_fd , EPOLL_CTL_ADD , p_reverse_forward_session->sock , & event );
			
			memset( & event , 0x00 , sizeof(struct epoll_event) );
			event.data.ptr = p_forward_session ;
			event.events = EPOLLIN | EPOLLERR ;
			epoll_ctl( forward_epoll_fd , EPOLL_CTL_ADD , p_forward_session->sock , & event );
		}
	}
	else
	{
		/* ֻ�����˲��� */
		InfoLog( __FILE__ , __LINE__ , "transfer #%d# [%d/%d]bytes to #%d#" , p_forward_session->netaddr.ip , p_forward_session->netaddr.port.port_int , p_forward_session->sock , len , p_forward_session->io_buffer_len , p_reverse_forward_session->netaddr.ip , p_reverse_forward_session->netaddr.port.port_int , p_reverse_forward_session->sock );
		DebugHexLog( __FILE__ , __LINE__ , p_forward_session->io_buffer , len , NULL );
		
		IgnoreReverseSessionEvents( p_forward_session , p_events , event_index , event_count );
		
		p_forward_session->io_buffer_len -= len ;
		p_forward_session->io_buffer_offsetpos = len ;
		
		if( after_accept_flag == 1 )
		{
			memset( & event , 0x00 , sizeof(struct epoll_event) );
			event.data.ptr = p_forward_session ;
			event.events = EPOLLERR ;
			epoll_ctl( forward_epoll_fd , EPOLL_CTL_ADD , p_forward_session->sock , & event );
			
			memset( & event , 0x00 , sizeof(struct epoll_event) );
			event.data.ptr = p_reverse_forward_session ;
			event.events = EPOLLOUT | EPOLLERR ;
			epoll_ctl( forward_epoll_fd , EPOLL_CTL_ADD , p_reverse_forward_session->sock , & event );
		}
		else
		{
			memset( & event , 0x00 , sizeof(struct epoll_event) );
			event.data.ptr = p_forward_session ;
			event.events = EPOLLERR ;
			epoll_ctl( forward_epoll_fd , EPOLL_CTL_MOD , p_forward_session->sock , & event );
			
			memset( & event , 0x00 , sizeof(struct epoll_event) );
			event.data.ptr = p_reverse_forward_session ;
			event.events = EPOLLOUT | EPOLLERR ;
			epoll_ctl( forward_epoll_fd , EPOLL_CTL_MOD , p_reverse_forward_session->sock , & event );
		}
	}
	
	/* �Ǽ����дʱ��� */
	if( p_forward_session->p_forward_rule->load_balance_algorithm == LOAD_BALANCE_ALGORITHM_RT )
	{
		if( p_forward_session->type == FORWARD_SESSION_TYPE_CLIENT )
			p_forward_session->p_forward_rule->server_addr_array[p_forward_session->server_index].wtt = GETSECONDSTAMP ;
	}
	
	return 0;
}

static int OnForwardOutput( struct ServerEnv *penv , struct ForwardSession *p_forward_session , int forward_epoll_fd , struct epoll_event *p_events , int event_index , int event_count )
{
	struct ForwardSession	*p_reverse_forward_session = p_forward_session->p_reverse_forward_session ;
	int			len ;
	struct epoll_event	event ;
	
	/* ��������ͨѶ���� */
	len = send( p_forward_session->sock , p_reverse_forward_session->io_buffer + p_reverse_forward_session->io_buffer_offsetpos , p_reverse_forward_session->io_buffer_len , 0 ) ;
	if( len == -1 )
	{
		if( _ERRNO == _EWOULDBLOCK )
		{
			/* ͨѶ���ͻ������� */
		}
		else
		{
			/* ͨѶ���ͳ��� */
			ErrorLog( __FILE__ , __LINE__ , "send #%d# failed , errno[%d]" , p_forward_session->sock , errno );
			return -1;
		}
	}
	else if( len == p_reverse_forward_session->io_buffer_len )
	{
		/* һ��ȫ���� */
		InfoLog( __FILE__ , __LINE__ , "transfer [%s:%u]#%d# [%d]bytes to [%s:%u]#%d#" , p_reverse_forward_session->netaddr.ip , p_reverse_forward_session->netaddr.port.port_int , p_reverse_forward_session->sock , len , p_forward_session->netaddr.ip , p_forward_session->netaddr.port.port_int , p_forward_session->sock );
		DebugHexLog( __FILE__ , __LINE__ , p_reverse_forward_session->io_buffer + p_reverse_forward_session->io_buffer_offsetpos , p_reverse_forward_session->io_buffer_len , NULL );
		
		p_reverse_forward_session->io_buffer_len = 0 ;
		
		memset( & event , 0x00 , sizeof(struct epoll_event) );
		event.data.ptr = p_forward_session ;
		event.events = EPOLLIN | EPOLLERR ;
		epoll_ctl( forward_epoll_fd , EPOLL_CTL_MOD , p_forward_session->sock , & event );
		
		memset( & event , 0x00 , sizeof(struct epoll_event) );
		event.data.ptr = p_reverse_forward_session ;
		event.events = EPOLLIN | EPOLLERR ;
		epoll_ctl( forward_epoll_fd , EPOLL_CTL_MOD , p_reverse_forward_session->sock , & event );
	}
	else
	{
		/* ֻ�����˲��� */
		InfoLog( __FILE__ , __LINE__ , "transfer [%s:%u]#%d# [%d/%d]bytes to [%s:%u]#%d#" , p_reverse_forward_session->netaddr.ip , p_reverse_forward_session->netaddr.port.port_int , p_reverse_forward_session->sock , len , p_reverse_forward_session->io_buffer_len , p_forward_session->netaddr.ip , p_forward_session->netaddr.port.port_int , p_forward_session->sock );
		DebugHexLog( __FILE__ , __LINE__ , p_reverse_forward_session->io_buffer + p_reverse_forward_session->io_buffer_offsetpos , len , NULL );
		
		p_reverse_forward_session->io_buffer_len -= len ;
		p_reverse_forward_session->io_buffer_offsetpos = len ;
	}
	
	/* �Ǽ����дʱ��� */
	if( p_forward_session->p_forward_rule->load_balance_algorithm == LOAD_BALANCE_ALGORITHM_RT )
	{
		if( p_forward_session->type == FORWARD_SESSION_TYPE_CLIENT )
			p_forward_session->p_forward_rule->server_addr_array[p_forward_session->server_index].wtt = GETSECONDSTAMP ;
	}
	
	return 0;
}

void *ForwardThread( unsigned long forward_thread_index )
{
	struct ServerEnv	*penv = g_penv ;
	int			forward_epoll_fd ;
	
	struct epoll_event	events[ WAIT_EVENTS_COUNT ] ;
	int			event_count = 0 ;
	int			event_index = 0 ;
	struct epoll_event	*p_event = NULL ;
	
	struct ForwardSession	*p_forward_session = NULL ;
	struct ForwardSession	*p_reverse_forward_session = NULL ;
	
	int			nret = 0 ;
	
	forward_epoll_fd = penv->forward_epoll_fd_array[forward_thread_index] ;
	
	/* ������־����ļ� */
	SetLogFile( penv->cmd_para.log_pathfilename );
	SetLogLevel( penv->cmd_para.log_level );
	SETPID
	SETTID
	InfoLog( __FILE__ , __LINE__ , "--- G6.WorkerProcess.ForwardThread.%d ---" , forward_thread_index+1 );
	
	if( penv->cmd_para.set_cpu_affinity_flag == 1 )
	{
		BindCpuAffinity( forward_thread_index+1 );
		InfoLog( __FILE__ , __LINE__ , "sched_setaffinity" );
	}
	
	/* ������ѭ�� */
	while( g_exit_flag == 0 || penv->forward_session_count > 0 )
	{
		if( penv->cmd_para.close_log_flag == 1 )
			CloseLogFile();
		event_count = epoll_wait( forward_epoll_fd , events , WAIT_EVENTS_COUNT , 1000 ) ;
		if( g_exit_flag == 1 )
		{
			UPDATEDATETIMECACHEFIRST
		}
		DebugLog( __FILE__ , __LINE__ , "epoll_wait sock[%d][...] forward_session_count[%u] return [%d]events" , penv->forward_command_pipe[forward_thread_index].fds[0] , penv->forward_session_count , event_count  );
		
		while(1)
		{
			p_forward_session = GetExpireTimeoutNode( penv ) ;
			if( p_forward_session == NULL )
				break;
			p_reverse_forward_session = p_forward_session->p_reverse_forward_session ;
			
			ErrorLog( __FILE__ , __LINE__ , "forward session TIMEOUT" );
			DISCONNECT_PAIR
		}
		
		for( event_index = 0 , p_event = events ; event_index < event_count ; event_index++ , p_event++ )
		{
			if( p_event->data.ptr == NULL )
			{
				char		command ;
				
				DebugLog( __FILE__ , __LINE__ , "pipe session event" );
				
				DebugLog( __FILE__ , __LINE__ , "read forward_command_pipe ..." );
				nret = read( penv->forward_command_pipe[forward_thread_index].fds[0] , & command , 1 ) ;
				DebugLog( __FILE__ , __LINE__ , "read forward_command_pipe done[%d][%c]" , nret , command );
				if( nret == -1 )
				{
					ErrorLog( __FILE__ , __LINE__ , "read request pipe failed , errno[%d]" , errno );
					exit(0);
				}
				else if( nret == 0 )
				{
					ErrorLog( __FILE__ , __LINE__ , "read request pipe close" );
					exit(0);
				}
				else
				{
					if( command == 'L' )
					{
						CloseLogFile();
					}
					else if( command == 'Q' )
					{
					}
				}
			}
			else
			{
				DebugLog( __FILE__ , __LINE__ , "forward session event" );
				
				p_forward_session = p_event->data.ptr ;
				p_reverse_forward_session = p_forward_session->p_reverse_forward_session ;
				
				if( p_event->events & EPOLLIN ) /* �����¼� */
				{
					nret = OnForwardInput( penv , p_forward_session , forward_epoll_fd , events , event_index , event_count , 1 ) ;
					if( nret > 0 )
					{
						DISCONNECT_PAIR
					}
					else if( nret < 0 )
					{
						ErrorLog( __FILE__ , __LINE__ , "OnForwardInput failed[%d]" , nret );
						DISCONNECT_PAIR
					}
					else
					{
						if( p_forward_session->p_forward_rule->forward_addr_array[p_forward_session->forward_index].timeout > 0 )
						{
							UpdateTimeoutNode2( penv , p_forward_session , p_forward_session->p_reverse_forward_session , GETSECONDSTAMP + p_forward_session->p_forward_rule->forward_addr_array[p_forward_session->forward_index].timeout );
						}
					}
				}
				else if( p_event->events & EPOLLOUT ) /* ����¼� */
				{
					nret = OnForwardOutput( penv , p_forward_session , forward_epoll_fd , events , event_index , event_count ) ;
					if( nret )
					{
						ErrorLog( __FILE__ , __LINE__ , "OnForwardOutput failed[%d]" , nret );
						DISCONNECT_PAIR
					}
					else
					{
						if( p_forward_session->p_forward_rule->forward_addr_array[p_forward_session->forward_index].timeout > 0 )
						{
							UpdateTimeoutNode2( penv , p_forward_session , p_forward_session->p_reverse_forward_session , GETSECONDSTAMP + p_forward_session->p_forward_rule->forward_addr_array[p_forward_session->forward_index].timeout );
						}
					}
				}
				else if( p_event->events ) /* �����¼� */
				{
					ErrorLog( __FILE__ , __LINE__ , "forward session #%d#EPOLLERR" , p_forward_session->sock );
					DISCONNECT_PAIR
					continue;
				}
			}
		}
	}
	
	return NULL;
}

void *_ForwardThread( void *pv )
{
	unsigned int	*p_forward_thread_index = (unsigned int *)pv ;
	unsigned int	forward_thread_index = (*p_forward_thread_index) ;
	
	free( p_forward_thread_index );
	ForwardThread( forward_thread_index );
	
	UPDATEDATETIMECACHEFIRST
	InfoLog( __FILE__ , __LINE__ , "pthread_exit" );
	pthread_exit(NULL);
}

