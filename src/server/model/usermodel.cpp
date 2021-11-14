#include "usermodel.hpp"
#include "db/db.h"
#include <iostream>
#include <string.h>

using namespace std;

UserModel::UserModel()
{
}

UserModel::~UserModel()
{
}

// User表的增加方法,
bool UserModel::insert(User &user)
{
    // 1. 组装SQL语句,
    char sqlStr[1024] = {};
    sprintf(sqlStr, "insert into user(name, password, state)  values('%s', '%s', '%s') ",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sqlStr))
        {
            // 获取插入成功的用户数据生成的主键id,
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    char sqlStr[1024] = {};
    sprintf(sqlStr, "select * from user where id = %d", id);

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sqlStr);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
}

bool UserModel::updateState(const User &user)
{
    // 1. 组装SQL语句,
    char sqlStr[1024] = {};
    sprintf(sqlStr, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sqlStr))
        {
            return true;
        }
    }
    return false;
}

// 重置用户的状态信息,
void UserModel::resetState(void)
{
    // 1. 组装SQL语句,
    char sqlStr[1024] =  "update user set state = 'offline' where state = 'online' ";
 
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sqlStr);
    }
}


