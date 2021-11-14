#include "offlinemessagemodel.hpp"
#include "db/db.h"
#include <iostream>

OfflineMsgModel::OfflineMsgModel() {}
OfflineMsgModel::~OfflineMsgModel() {}

// 存储用户的离线消息,
void OfflineMsgModel::insert(int userid, std::string msg)
{
    // 1. 组装SQL语句,
    char sqlStr[1024] = {};
    sprintf(sqlStr, "insert into offlinemessage(userid, message)  values(%d, '%s')", userid, msg.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sqlStr);
    }
    return;
}

// 删除用户的离线消息,
void OfflineMsgModel::remove(int userid)
{
    // 1. 组装SQL语句,
    char sqlStr[1024] = {};
    sprintf(sqlStr, "delete from offlinemessage where userid = %d", userid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sqlStr);
    }
    return;
}

// 查询用户的离线消息,
std::vector<std::string> OfflineMsgModel::query(int userid)
{
    char sqlStr[1024] = {};
    sprintf(sqlStr, "select message from offlinemessage where userid = %d", userid);

    std::vector<std::string> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sqlStr);
        if (res != nullptr)
        {
            // 把 userid 用户的所有离线消息放入到 vec 中返回,
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.emplace_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}
