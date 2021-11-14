#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

// User 表的数据操作类,
class UserModel
{
public:
    UserModel();
    ~UserModel();

public:
    // User表的增加方法,
    bool insert(User &user);

    // 根据用户号码查询用户信息,
    User query(int id);

    // 更新用户的状态信息,
    bool updateState(const User &user);

    // 重置用户的状态信息,
    void resetState(void);

    // 重写add接口方法，实现增加用户操作
    // bool add(UserDO &user)
    // {
    //     // 组织sql语句
    //     char sql[1024] = {0};
    //     sprintf(sql, "insert into user(name,password,state) values('%s', '%s', '%s')",
    //             user.getName().c_str(),
    //             user.getPwd().c_str(),
    //             user.getState().c_str());
    //     MySQL mysql;
    //     if (mysql.connect())
    //     {
    //         if (mysql.update(sql))
    //         {
    //             LOG_INFO << "add User success => sql:" << sql;
    //             return true;
    //         }
    //     }
    //     LOG_INFO << "add User error => sql:" << sql;
    //     return false;
    // }
};

#endif