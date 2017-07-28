#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



int main(int argc, char const *argv[])
{
	int client_fd=socket(AF_INET,SOCK_STREAM,0);
	if (client_fd==-1)
	{
		perror("socket fail\n");
		exit(-1);
	}
	//定义服务端地址结构
	struct sockaddr_in ser_addr;
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_port=htons(1234);
	ser_addr.sin_addr.s_addr=inet_addr("192.168.247.128");
	if (connect(client_fd,(struct sockaddr*)&ser_addr,sizeof(ser_addr))==-1)
	{
		perror("connect fail");
		exit(-1);
	}
	return 0;
}
