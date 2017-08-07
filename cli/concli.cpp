#include "concli.h"
#include "md5.h"

concli::concli(int ser_port,const char *s_add)
{
	 client_fd=socket(AF_INET,SOCK_STREAM,0);
	if (client_fd==-1)
	{
		perror("socket fail\n");
		exit(-1);
	}
	//服务端地址结构
	memset(&ser_addr,0,sizeof(ser_addr));
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_port=htons(ser_port);
	ser_addr.sin_addr.s_addr=inet_addr(s_add);
}
concli::~concli(){}
void concli::conn()     //与客户端连接
{
	if (connect(client_fd,(struct sockaddr*)&ser_addr,sizeof(ser_addr))==-1)
	{
		perror("conn fail");
		exit(-1);
	}

}

// void concli::select()   //选择服务项
// {
// 	int select;
// 	cin>>select;
// 	switch(select)
// 	{
// 		case ENTER://登录
// 	        msg = "1 ";
// 	        login();
// 	        break;

// 	    case REGISTER://注册
// 	        msg = "2 ";
// 	        login();
// 	        break;

// 	    case CHART://聊天
// 	        msg = "5 ";
// 	        break;

// 	    case UPLOAD://上传文件
// 	        msg = "3 ";
// 	        break;
	    
// 	    case DOWNLOAD://下载文件
// 	        msg = "4 ";
// 	        break;

// 	    default:
// 	        msg = "-1 ";
// 	        break;
// 	}
// }
void concli::login()    //登录
{
	string user,passwd;  
    cout<<"username:";
    cin>>user;
    msg = user;
    cout<<"password:";
    cin>>passwd;
    //msg = msg + " "+passwd;
    msg = msg+" "+MD5(passwd).toString();
  
}
void concli::sendmsg()   //发送消息
{
	//cout<<msg.c_str()<<endl;
    send(client_fd,msg.c_str(),msg.size()+1,0); 
    cout<<"client send ok"<<endl;       
    // recv(client_fd,(void *)msg.c_str(),1024,0);
    // cout<<"client recv ok"<<endl;  
    // cout<<"msg.c_str()="<<msg.c_str()<<endl;
 //    if(strncmp("1",msg.c_str(),1) == 0 )
	// {
 //       cout<<"login successful"<<endl;
	// }
    // else if(strncmp("The user existed",msg.c_str(),17)==0)
    // {
    //     cout<<msg.c_str()<<endl;
    //     select();
    // }else
    // {
    //     system("clear");  //清屏
    //     cout<<"username or password error"<<endl;
    //     select();
    // }
}
void concli::filemd5()   //发送filemd5
{
	string fname,passwd;
	cout<<"please input filename:"<<endl;
    cin>>fname;
    fileinfo=fname;
	fileinfo=MD5(fname).toString()+" "+fileinfo;

	send(client_fd,fileinfo.c_str(),fileinfo.size()+1,0); 
    cout<<"client send ok"<<endl;       
}

void concli::sendfile()      //发送文件
{
	char *filename="hacker.png";
    FILE* fp=fopen(filename,"rb");
    if (fp==NULL)
    {
        perror("openfile fail:");
    }
    cout<<"begin sendfile"<<endl;
    char buffer[1024]={0};
    int nCount;
    while((nCount=fread(buffer,1,1024,fp))>0)
    {
        send(client_fd,buffer,nCount,0);
    }
    shutdown(client_fd,SHUT_RD);
    //recv(client_fd,buffer,1024,0);
    fclose(fp);
    cout<<"sendfile ok"<<endl;
}


void concli::Run()
{
	conn();
	login();
	sendmsg();
	filemd5();
	//sendfile();
}


int main(int argc, char const *argv[])
{
	concli cli(MANP,SERIP);
	cli.Run();

	//cli.sendfile();
	return 0;
}


