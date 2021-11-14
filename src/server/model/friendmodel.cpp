#include "friendmodel.hpp"
#include "db/db.h"
#include <iostream>

using namespace std;

FriendModel::FriendModel()
{
}

FriendModel::~FriendModel()
{
}

// 添加好友关系,
void FriendModel::insert(int userid, int friendid)
{

    // 1. 组装SQL语句,
    char sqlStr[1024] = {};
    sprintf(sqlStr, "insert into friend(userid, friendid)  values(%d, %d)", userid, friendid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sqlStr);
    }
    return;
}

// 返回用户的好友列表,  frinedid  user.name   user.state,  做 friend  user 表的联合查询, 然后返回,
std::vector<User> FriendModel::query(int userid)
{
    // friend  user   userid==>friendid (很多)
    // mysql> select usera.id,usera.name,usera.state  from user usera inner join friend friendb on friendb.friendid = usera.id  where friendb.userid = 13;
    // +----+----------+---------+
    // | id | name     | state   |
    // +----+----------+---------+
    // | 15 | li si    | offline |
    // | 21 | gao yang | offline |
    // +----+----------+---------+
    // 2 rows in set (0.02 sec)


    char sqlStr[1024] = {};
    sprintf(sqlStr, "select usera.id,usera.name,usera.state  from user usera inner join friend friendb on friendb.friendid = usera.id  where friendb.userid =  %d", userid);

    std::vector<User> vec;
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
                User user; 
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.emplace_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}
