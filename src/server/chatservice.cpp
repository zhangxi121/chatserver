#include "chatservice.hpp"
#include "public.hpp"
#include <iostream>
#include <muduo/base/Logging.h>
#include <string>
#include <vector>

using namespace std;
using namespace muduo;
using namespace muduo::net;

ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    // 注册消息已经对应的回调操作,
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, placeholders::_1, placeholders::_2, placeholders::_3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, placeholders::_1, placeholders::_2, placeholders::_3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, placeholders::_1, placeholders::_2, placeholders::_3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, placeholders::_1, placeholders::_2, placeholders::_3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, placeholders::_1, placeholders::_2, placeholders::_3)});

    // 群组业务管理相关事件处理回调注册
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

    // 连接 redis 服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, placeholders::_1, placeholders::_2));
    }
}

ChatService::~ChatService()
{
}

// 服务器异常,业务重置方法,
void ChatService::reset()
{
    // 把 online 状态的用户设置成 offline 状态,
    _userModel.resetState();
}

// 获取消息对应的处理器,
MsgHandler ChatService::getHandler(int msgid)
{
    // 记录错误日志, msgid没有对应的事件处理回调,
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        return [=](const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
        {
            LOG_ERROR << "msgid: " << msgid << " can not find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

// ORM 框架  Object-Relational-Mapping,
// 业务层操作的都是对象, 业务层看不到任何 SQL语句的操作,
// DAO 数据层,才是具体的数据库的操作,
//

// 处理登录业务 id  name  pwd
void ChatService::login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int id = js["id"].get<int>(); // 默认是字符串类型,需要转成整形用 get<int>() 方法,
    string pwd = js["password"];
    User user = _userModel.query(id);
    if (user.getId() == id && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            // 该用户已经登录,不允许重复登录,
            nlohmann::json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using, input another";
            conn->send(response.dump());
        }
        else
        {
            // 登录成功,记录用户连接信息,
            {
                lock_guard<std::mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // id用户登录成功后，向redis订阅channel(id)
            _redis.subscribe(id); 

            // 登录成功, 更新用户状态信息 state  offline=>online,
            user.setState("online");
            _userModel.updateState(user);

            nlohmann::json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 查询该用户是否有离线消息,有的话装载到json,
            vector<std::string> vec = _offlineMsgModel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec; // nlohmann::json  可以直接序列化 vector,
                // 读取该同户得离线消息后,把该用户的所有离线消息删除掉,
                _offlineMsgModel.remove(id);
            }
            // 查询该用户的好友信息,并返回json,
            vector<User> vec_user = _friendModel.query(id);
            if (!vec_user.empty())
            {
                vector<std::string> vec2;
                for (User &user : vec_user)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    // vec2.emplace_back(js.dump());
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }

            // 查询用户的群组信息,
            vector<Group> groupserVec = _groupModel.queryGroups(id);
            if (!groupserVec.empty())
            {
                // group:[{groupid:[xxx,xxx,xxx,xxx]}]
                vector<std::string> groupV;
                for (Group &group : groupserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userVec;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userVec.emplace_back(js.dump());
                    }
                    grpjson["users"] = userVec;
                    groupV.emplace_back(grpjson.dump());
                }
                response["groups"] = groupV;
            }
            std::cout << response.dump() << std::endl;
            conn->send(response.dump());
        }
    }
    else
    {
        // 该用户不存在, 或者用户存在但是密码错误,登录失败,
        nlohmann::json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "id or password is invalid";
        conn->send(response.dump());
    }

    LOG_INFO << "do login service!";
}

// 处理注册业务,  name  password  state=”offline“
void ChatService::reg(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    string name = js["name"].get<string>();
    string pwd = js["password"].get<string>();

    User user;
    user.setName(name);
    user.setPwd(pwd);

    bool state = _userModel.insert(user);
    if (state)
    {
        // 注册成功,
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        // 注册失败,
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "errno_1";
        conn->send(response.dump());
    }
}

// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid);

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}
// 处理客户端异常退出,
void ChatService::clientCloseException(const muduo::net::TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<std::mutex> lock(_connMutex);

        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 智能指针的比较,
                // 从 map 表删除用户的连接信息,
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(user.getId()); 

    // 更新用户的状态信息,
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

// 一对一聊天业务,
void ChatService::oneChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int toid = js["toid"].get<int>();

    {
        lock_guard<std::mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid); // 找到对端的 connection, 然后通过对端的 conn 发送消息,
        if (it != _userConnMap.end())
        {
            // toid 在线, 转发消息给他, 服务器主动推送消息给 toid 用户,
            it->second->send(js.dump());
            return;
        }
    }

    // 查询toid是否在线 
    User user = _userModel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, js.dump());
        return;
    }

    // toid 不在线, 存储离线消息,
    _offlineMsgModel.insert(toid, js.dump());

    return;
}

// msgid id friendid
void ChatService::addFriend(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 存储好友信息,
    _friendModel.insert(userid, friendid);
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connMutex);
    for (int id : useridVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 转发群消息
            it->second->send(js.dump());
        }
        else
        {
            // 查询toid是否在线
            User user = _userModel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
                // 存储离线群消息
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        // 接收到 conn[id] 订阅的消息, 然后将接收到到的消息转发给客户端,
        it->second->send(msg);
        return;
    }

    // 接收到消息的时候用户下线了,
    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}

//