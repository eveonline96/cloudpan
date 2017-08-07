#ifndef _PUBLIC_
#define _PUBLIC_
#include <stdio.h>
#include <string.h>
#include <string>
#include <list>
#include <iostream>
#include <errno.h>
#include <cstring>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
using namespace std;

#define  MANP 6666      //管理端口
#define  TRAP 6667		//传输端口
#define  CHAP 6668		//聊天端口
#define  MEMP 11211		//memcached 端口

#define SERIP   "192.168.253.129"//"192.168.253.129"
#define SERADD  "localhost"  //"47.95.9.185"  //服务端地址
#define DBUSER  "root"      //数据库用户名
#define DBPSWD  "654321"    //密码
#define DBNAME  "cloudpan"  //数据库名字

#define ERR_EXIT(m) \
do \
{ \
   perror(m); \
   exit(EXIT_FAILURE); \
}while(0)

//用户操作类型
enum  OP_TYPE{ERROR=-1,ENTER=1,REGISTER=2,UPLOAD=3,DOWNLOAD=4,CHART=5};


//用户信息
typedef struct msg
{
	// OP_TYPE type;
	string user_id;
	string password; 
}Msg;
//文件头信息
typedef struct fileinfo
{
	string file_name;    //文件名
	string file_MD5;	 //文件MD5值
	string file_path;    //文件路径
	int file_size  ;	 //文件大小
	int block_size;		//分块大小
	bool tran_statu;    //传输状态
}fileinfo;

typedef struct blockinfo
{
	string file_name;  //文件名
	int id;				//分块id
	int offset;			//该块在文件中的偏移量
	bool tran_statu;   //传输状态
}blockinfo;


#endif

