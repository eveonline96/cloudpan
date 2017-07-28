负载均衡服务器的作用：
1.接收Cli端客户请求消息Request Msg，并作相应处理，回传Respone Msg;
2.负载均衡服务器负责客户的登录、注册功能;
3.负载均衡服务器管理后端的功能处理服务器（FileTransServer、ChartServer）,选择合适服务器处理业务

gcc a.c `mysql_config --libs`  编译时加入mysql

在linux上安装openssl   md5： dnf install  openssl 
dnf install libmemcached-devel   。devel头文件header

应用时-lcrypto 

云盘文件MD5


客户端： cli  MD5

参数：
	int client_fd;         //客户端套接字   
	int ser_port;			//服务端端口
	const char *s_add;		//服务端地址
	string msg;             //传送消息
	struct sockaddr_in ser_addr;   //定义服务端地址结构体
方法：
	void connect();   //创建链接	
		|
		v
	void select();	  //选择服务内容
		|
		v
	void login();	  //登录操作(加密)
		|
		v
	void sendmsg()    //发送消息

服务端：




