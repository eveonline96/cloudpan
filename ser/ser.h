#ifndef _SER_H
#define _SER_H
#include "../public.h"
#include <unistd.h>
class conser
{
public:
	conser(int s_port);
	~conser();
	void Bind();
	void Listen();
	void receive();
	void Run();
	void confirm();
	bool mydbuserconfirm(char * name);
	bool mydbpsconfirm(char * pswd);
	void recvfile();
	void filemd5();
	bool filemd5confirm(char * filemd5);
private:
	int  listenfd;   //监听套接字
	int cli_fd;    //客户端套接字
	int  s_port;    //端口
   //	const char * s_addr;      //客户端地址
   	string msg;             //传送消息
   	string fileinfo;             //传送文件消息
   	struct sockaddr_in ser_addr;   //定义服务端地址结构体
   	struct sockaddr_in cli_addr;   //定义客户端地址结构体
   	socklen_t cli_len;  

   //	char * name;
};

#endif
