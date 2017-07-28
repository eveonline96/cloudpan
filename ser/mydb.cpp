#include "mydb.h"


MyDB::MyDB()  //
{
    conn = mysql_init(NULL);
    if(conn == NULL)
    {
        cout<<"Errorr"<<mysql_error(conn)<<endl;
        exit(1);
    }
}
MyDB::~MyDB()  //
{
    if(conn != NULL)
    {
        mysql_close(conn);
    }
}

bool MyDB::initDB(string host,string user,string password,string db_name)
{
    conn = mysql_real_connect(conn,
       host.c_str(),       /*服务器的地址*/
        user.c_str(),      /*用户名*/
        password.c_str(),  /*密码*/
        db_name.c_str(),   /*连接的数据库名字*/
        3306,              /*端口*/
        NULL,
        0);

    // conn=mysql_real_connect(conn,
    //             "localhost",/*服务器的地址*/
    //             "root",/*用户名*/
    //             "654321",/*密码*/
    //             "l0904",/*连接的数据库名字*/
    //             3306,/*端口*/
    //             NULL,
    //             0);
    if(conn == NULL)
    {
        cout<<"Mysql Connecting Error"<<mysql_error(conn)<<endl;
        return false;
    }
    else
    {
        cout<<"mysql connect ok"<<endl;
        return true;
    }
    
}

bool MyDB::execSQL(string sql)   //查询
{
    int i,j;
    if(mysql_query(conn,sql.c_str()))
    {
        cout<<"Query Error"<<mysql_error(conn)<<endl;
        exit(1);
    }
    else
    {
        result = mysql_use_result(conn);//获取结果集
        for(i=0;i<mysql_field_count(conn);++i)
        {
            row = mysql_fetch_row(result);  //获取下一行
            if(row <= 0) break;
            for(j=0;j<mysql_num_fields(result);++j)  //返回结果集中字段的数
            {
                //cout<<row[j]<<" ";
                res.push_back(row[j]);
                //cout<<"push_back"<<endl;
            }
            cout<<endl;
        }
        //释放结果集的内存
        mysql_free_result(result);
    }
    if (res.size()!=0)
    {
         return true;
    }
    else
    {
        return false;
    }

//    return true;
}

void MyDB::showResult()
{
    cout<<"showResult():"<<res.size()<<endl;
    list<string>::iterator it;
    for(it=res.begin();it!= res.end();++it)
    {
        cout<<*it<<endl;
    }
}
