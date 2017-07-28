#ifndef _MYDB_H
#define _MYDB_H

#include "../public.h"
//#include "/usr/include/mysql/mysql.h"

class MyDB
{
public:
    MyDB();
    ~MyDB();
    bool initDB(string host,string user,string password,string db_name);
    bool execSQL(string sql);
    void showResult();
private:
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;
    list<string> res;
};
#endif

