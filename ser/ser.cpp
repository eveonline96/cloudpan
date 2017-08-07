/*使用“nestat -a”命令，查看所有服务端口【监听、连接】列表信息。*/
#include "ser.h"
#include "mydb.h"
conser::conser(int s_port)
{
	if ((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		ERR_EXIT("socket init fail");
	}
	memset(&ser_addr,0,sizeof(ser_addr));
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_port=htons(s_port);
	ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
}

conser::~conser(){}

void conser::Bind()
{
	int on=1;
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
	{
		ERR_EXIT("setsockopt fail");
	}
	if (bind(listenfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr))==-1)
	{
		ERR_EXIT("Bind fail");
	}
}

void conser::Listen()
{
	if (listen(listenfd,10)==-1)
	{
		ERR_EXIT("listen fail");
	}
}

void conser::receive()
{
	cli_len=sizeof(cli_addr);
	cli_fd=accept(listenfd,(struct sockaddr*)&cli_addr,&cli_len);
	//cout<<"accept ok"<<endl;
	if (cli_fd>0)
	{
		//cout<<cli_fd<<endl;
		bzero((void *)msg.c_str(),msg.size());
		recv(cli_fd,(void *)msg.c_str(),1024,0);
		cout<<"server recv ok"<<endl; 
		// send(cli_fd,msg.c_str(),msg.size()+1,0);
		// cout<<"server send ok"<<endl;  

	}

}


bool conser::mydbuserconfirm(char * name)
{

	MyDB db;
	db.initDB(SERADD,DBUSER,DBPSWD,DBNAME);
    string Name=name;
   // cout<<"Name="<<Name<<endl;
    string usersql ="select * from userinfo where name='"+Name+"'";
    bool userBool=db.execSQL(usersql); 
    if (userBool)
     {
     	return true;
     } 
    else
    {
    	return false;
    }  
}

bool conser::mydbpsconfirm(char * pswd)
{
	MyDB db;
	db.initDB(SERADD,DBUSER,DBPSWD,DBNAME);
    string PSWD=pswd;
   // cout<<"PSWD="<<PSWD<<endl;
    string usersql ="select * from userinfo where password='"+PSWD+"'";
    bool userBool=db.execSQL(usersql); 
    if (userBool)
     {
     	return true;
     	//cout<<"usenname exist"<<endl;
     } 
    else
    {
    	return false;
    	//cout<<"username nonexist"<<endl;
    }  
}

void conser::confirm()
{
	MyDB db;
    db.initDB(SERADD,DBUSER,DBPSWD,DBNAME);
    cout<<"enter database"<<endl;

	char buf[40];
	strcpy(buf,msg.c_str());
	//cout<<"buf="<<buf<<endl;
	char *ptr=strtok(buf," ");
	bool userbool=mydbuserconfirm(ptr);
	if (!userbool)
	{
		cout<<"username is not exist"<<endl;
	}
	else
	{
		ptr=strtok(NULL," ");
		bool pswdbool=mydbpsconfirm(ptr);
		if (pswdbool)
		{
			cout<<"login ok"<<endl;
			db.showResult();
		}
		else
		{
			cout<<"password  wrong"<<endl;
		}
	}
}
 

bool conser::filemd5confirm(char * filemd5)
{
	MyDB db;
	db.initDB(SERADD,DBUSER,DBPSWD,DBNAME);
    string FILEMD5=filemd5;
    string usersql ="select * from file where filemd5='"+FILEMD5+"'";
    bool userBool=db.execSQL(usersql); 
    if (userBool)
     {
     	return true;
     } 
    else
    {
    	return false;
    }  
}

void conser::filemd5()
{
	recv(cli_fd,(void *)fileinfo.c_str(),1024,0);

	MyDB db;
    db.initDB(SERADD,DBUSER,DBPSWD,DBNAME);
    cout<<"enter database"<<endl;

	char buf[40];
	strcpy(buf,fileinfo.c_str());
	//cout<<"buf="<<buf<<endl;
	char *ptr=strtok(buf," ");
	cout<<"ptr="<<ptr<<endl;
	bool userbool=filemd5confirm(ptr);
	if (!userbool)
	{
		cout<<"file is not exist"<<endl;
	}
	else
	{
		cout<<"file is exist"<<endl;
	}
}

void conser::recvfile()
{
	char *filename="hacker.png";
    FILE* fp=fopen(filename,"wb");
    if (fp==NULL)
    {
        perror("openfile fail:");
    }
    cout<<"begin  recvfile "<<endl;
    char buffer[1024]={0};
    int nCount;
    while((nCount=recv(cli_fd,buffer,1024,0))>0)
    {
        fwrite(buffer,nCount,1,fp);
    }
    fclose(fp);
    cout<<"recvfile ok"<<endl;
}


void conser::Run()
{
	Bind();
	Listen();
	receive();
	//confirm();
	filemd5();
	//recvfile();
}

int main(int argc, char const *argv[])
{
	
	conser ser(MANP);
	ser.Run();

	return 0;
}



















