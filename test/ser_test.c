#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char const *argv[])
{
	int listen_fd=socket(AF_INET,SOCK_STREAM,0);
	if (listen_fd==-1)
	{
		perror("init soccket fail\n");
		exit(-1);
	}
	//初始化地址结构=地质族+地址值+端口值
	struct sockaddr_in listen_addr;
	bzero(&listen_addr,sizeof(listen_addr));
	listen_addr.sin_family=AF_INET;
	listen_addr.sin_port=htons(1234);
	listen_addr.sin_addr.s_addr=inet_addr("192.168.247.128");
	//绑定
	if (bind(listen_fd,(struct sockaddr*)&listen_addr,sizeof(listen_addr))==-1)
	{
		perror("bind fail\n");
		exit(-1);
	}
	if (listen(listen_fd,10)==-1)
	{
		perror("listen fail\n");
		exit(-1);
	}
	// 接收客户请求，并返回一个新的套接字分配给客户
	while(1)
	{
		struct sockaddr_in client_addr;
		socklen_t client_len=sizeof(client_addr);
		int client_fd=accept(listen_fd,(struct sockaddr*)&client_addr,&client_len);
		printf("accept end\n");
		if (client_fd>0)
		{
			printf("ip=%s\tport=%d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		}
	}
	return 0;
}
