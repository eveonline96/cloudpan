/*
声明客户端的类
*/
#ifndef _CONCLI_H
#define _CONCLI_H
#include "../public.h"
class concli
{
public:
	concli(int ser_port,const char *s_add);
	~concli();
	void conn();   //创建链接
	void select();	  //选择服务内容
	void login();	  //登录操作
	void sendmsg() ;   //发送消息
	void sendfile();
	void filemd5() ;
	void Run();
private:
	int client_fd;         //客户端套接字   
	int ser_port;			//服务端端口
	const char *s_add;		//服务端地址
	string msg;             //传送消息
	string fileinfo;
	struct sockaddr_in ser_addr;   //定义服务端地址结构体

};

#endif 
