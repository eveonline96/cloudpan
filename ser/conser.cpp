#include "MCache.h"
#include "conSer.h"
conSer::conSer(int port,const char *addr)
{
    if((listenfd=socket(PF_INET,SOCK_STREAM,0)) ==-1)
        ERR_EXIT("socket");
    memset(&seraddr,0,sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(port);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
}

conSer::~conSer(){}

void conSer::Run()
{
    Bind();
    Listen();
    epoll_work();
}

void conSer::Bind()
{
    int on = 1;
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0 )
        ERR_EXIT("setsockopt");
    if(bind(listenfd,(struct sockaddr*)&seraddr,sizeof(seraddr)) < 0 )
        ERR_EXIT("bind");
}

void conSer::Listen()
{
    if(listen(listenfd,SOMAXCONN) < 0)
        ERR_EXIT("listen");
}

//采用epoll方法:
void conSer::epoll_work()
{
    struct epoll_event ev,events[EPOLL_SIZE];
    int epfd = epoll_create(EPOLL_SIZE);
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

    while(1)
    {
        int events_count = epoll_wait(epfd,events,EPOLL_SIZE,-1);
        for(int i=0;;i<events_count;++i)
        {
            if(events[i].data.fd == listenfd)
            {
                int conn;
                socklen_t peerlen = sizeof(peeraddr);
                pid_t pid;
                
                system("clear");
                while((conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen))>0)
                {
                    cout<<endl<<"=============== Service ================"<<endl;
                    cout<<" EPOLL:Received New Connection Request "<<endl;
                    cout<<"  confd="<<conn<<endl;
                    cout<<"  ip="<<inet_ntoa(peeraddr.sin_addr)<<endl;
                    cout<<"  port="<<ntohs(peeraddr.sin_port)<<endl;
                    cout<<"========================================"<<endl;
                    pid = fork();
                    if(pid == -1)
                        ERR_EXIT("fork");
       
                    if(pid == 0)
                    {
                        close(listenfd);
                        cout<<"Start do_service"<<endl;
                        do_service(conn);
                        cout<<"End of do_service"<<endl;
                        cout<<"========================================"<<endl;
                        exit(EXIT_SUCCESS);
                    }else
                        close(conn);    
                }
            }
        }
    }
}



void conSer::do_service(int conn)
{
    while(1)
    {
        int ret = read(conn,(void *)msg.c_str(),1024);
        get_cmd();
        write(conn,(void*)msg.c_str(),msg.size());
        if(ret != -1)
            break;
    }
}


void conSer::selectFTServer()
{
    string FTS_IP = SERADDR;
    int FTS_PORT = TPT;
    stringstream ss;
    string port;
    ss<<FTS_PORT;
    ss>>port;
    msg += FTS_IP+" "+port;
}

void conSer::get_cmd()
{
    char *ptr = (char*)msg.c_str();
    char *cmd = strtok(ptr," ");
    string sqlExec;
    int select;
    while(ptr=strtok(NULL," "))
    {
        sql.push_back(ptr);
    }

    MyDB *db = new MyDB();
    if(!db->initDB(SERADDR,DBUSER,DBPSSW,DBNAME))
    {
        msg = "ser error(mysql connecting)";
        select = -1;
    }else
    {
        select = atoi(cmd);
    }

    string userid=*(sql.begin());
    sql.pop_front();
    string passwd=*(sql.begin());
    sql.pop_front();

    MCache mem(SERADDR,MCA);

    switch(select)
    {
    //登录：加入memcached后，不需要从数据库中查询
    case ENTER:
        if(strncmp(mem.getValue(userid.c_str()),passwd.c_str(),32) == 0)
        {
            msg = "1";
        }else
        {
            msg = "userid or passwd error!";
        }
        break;

    //注册：插入mysql数据库并且插入到memcached缓存中
    case REGISTER:
        //insert into user(user_id,password) select '用户名','密码' from dual where not exists(select *from user where user.user_id='用户名');
        sqlExec="select exists (select *from user where user_id='"+userid+"')";
        db->execSQL(sqlExec);
        msg=*(db->getResult()).begin();
        if(strncmp("1",msg.c_str(),1) == 0)
        {
            msg = "The user existed!";
        }else{

            sqlExec="insert into user(user_id,password) select '"+userid+"','"+passwd+"' from dual where not exists(select *from user where user.user_id='"+userid+"')";
            db->execSQL(sqlExec);
            msg = "register success!";
        }
        //插入memcached
        //MCache mem(SERADDR,MCA);
        mem.insertValue(userid.c_str(),passwd.c_str(),180);
        break;

    case UPLODE:
        msg = "3 ";
        selectFTServer();
        break;

    case DOWNLOAD:
        msg = "4 ";
        selectFTServer();
        break;

    case CHART:
        break;
    }
}
int main()
{
    conSer ser(MANP,SERADDR);
    ser.Run();
    return 0;
}
