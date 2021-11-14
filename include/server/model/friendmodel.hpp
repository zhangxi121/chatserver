#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H


#include "user.hpp"
#include <string>
#include <vector>

// 维护好友信息的操作接口方法,

class FriendModel
{
public:
    FriendModel();
    ~FriendModel();
public:
    // 添加好友关系,
    void insert(int userid,int  friendid);


    // 返回用户的好友列表,  frinedid  user.name   user.state,  做 friend  user 表的联合查询, 然后返回,
    std::vector<User> query(int userid);


private:

};

#endif //FRIENDMODEL_H