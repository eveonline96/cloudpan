#ifndef CONSER_H
#define CONSER_H
#include "../public.h"
#include "mydb.h"
#include "sqlConnPool.h"
class conSer
{
public
    conSer(int port,const char *addr);
    ~conSer();
    void Bind();
    void Listen();
    int accept_timeout(unsigned int wait_seconds);//select
    void epoll_work();                            //epoll
    void Run();

private:
    void selectFTServer();
    void do_service(int conn);
    void get_cmd();

private:
    int listenfd;      //监听套接字
    int port;			//端口
    char *addr;			//地址
    list<string> sql;
    string msg;
    struct sockaddr_in seraddr,peeraddr;
};

#endif
