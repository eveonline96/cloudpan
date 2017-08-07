负载均衡服务器的作用：
1.接收Cli端客户请求消息Request Msg，并作相应处理，回传Respone Msg;
2.负载均衡服务器负责客户的登录、注册功能;
3.负载均衡服务器管理后端的功能处理服务器（FileTransServer、ChartServer）,选择合适服务器处理业务

gcc a.c `mysql_config --libs`  编译时加入mysql

在linux上安装openssl   md5： dnf install  openssl 
dnf install libmemcached-devel   。devel头文件header

应用时-lcrypto 

云盘文件  加密MD5:实现秒传功能


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

服务端：相对version1.0版本的改进是：使用[G6](http://www.oschina.net/p/G6)开源负载均衡器，并加入相应的进程池、连接池

1.G6：该项目是在linux下采用C/C++编程，基于TCP协议，服务器端采用多进程长连接的socket的通信方式，并采用开源的负载均衡器G6作为客户端请求与服务端响应的中间件分发器，[G6](http://www.oschina.net/p/G6)是该开发者在[G5](http://www.oschina.net/p/g5)的基础上的升级版，相比G5多了心跳报告和命令管理的功能，所以对于服务器的状态就不需要关心了，包括服务器的组装，只需要修改G6的配置文件。

2、连接池：考虑到服务器端如果每接收到一个请求fork一个进程处理方式的效率低，所以服务器端采用进程池方式，结合[libevent](http://libevent.org/)网络库，可以处理并发的TCP连接请求，同时对数据库的访问会带来同样的问题，所以实现了一个基于[mysqlclient](https://pypi.python.org/pypi/mysqlclient)接口的连接池。

3.memcached[memcached](http://php.net/manual/zh/book.memcached.php)的采用是为了减轻对mysql数据库的访问，比如在用户注册的时候，需要插入mysql中并在memcached中基于key-value的键值对用户名和密码进行存储，这样对于用户之后的登录只需要在memcached中查询，而不需要访问数据库，因memcached是基于内存的缓存系统，所以在这样查询效率就会很高，但是不好的一点就是如果系统宕机或者重启，数据就没有了；对于这种情况我的处理方式是，定期将memcached中的数据写入硬盘文件，每次重启导入该文件，这里的定期采用的是libevent中统一事件源中的定时器事件。


