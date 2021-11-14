#ifndef CHATSERVICE_H
#define CHATSERVICE_H


#include <functional>
#include <mutex>
#include <memory> // 智能指针,
#include <muduo/net/TcpConnection.h>
#include <string>
#include <unordered_map>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;


#include "groupmodel.hpp"
#include "friendmodel.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "json.hpp"
#include "redis.hpp"
using json = nlohmann::json;

// 表示处理消息的事件回调方法类型,
using MsgHandler = std::function<void(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)>;

// 聊天服务器业务类,单例模式
class ChatService
{
public:
    // 获取单例对象的接口函数,
    static ChatService *instance();

public:
    // 处理登录业务,
    void login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);

    // 处理注册业务,
    void reg(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);

    // 一对一聊天业务,
    void oneChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);

    // 添加好友业务
    void addFriend(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);

    // 创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);   
    
    // 服务器异常,业务重置方法,
    void reset();


    // 获取消息对应的处理器,
    MsgHandler getHandler(int msgid);

    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int userid, string msg);


    // 处理客户端异常退出,
    void clientCloseException(const muduo::net::TcpConnectionPtr &conn);

private:
    ChatService();
    ~ChatService();

private:
    // 获取消息Id和其对应的业务 处理方法,
    unordered_map<int, MsgHandler> _msgHandlerMap;

    // 存储在线用户的通信连接, 注意线程安全,
    unordered_map<int, muduo::net::TcpConnectionPtr> _userConnMap;

    // 定义互斥锁,保证 _userConnMap 的线程安全,
    std::mutex _connMutex;

    // 数据操作类,
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    // redis 操作对象
    Redis _redis;

};

#endif