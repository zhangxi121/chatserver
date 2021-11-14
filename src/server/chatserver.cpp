#include <functional>
#include <iostream>
#include <string>
#include <muduo/base/Logging.h>
#include "json.hpp"
#include "chatserver.hpp"
#include "chatservice.hpp"

using namespace std;
using namespace placeholders;
using json =  nlohmann::json;

ChatServer::ChatServer(muduo::net::EventLoop *loop,
                       const muduo::net::InetAddress &listenAddr,
                       const string &nameArg,
                       TcpServer::Option option) : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册连接回调,
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, placeholders::_1));

    // 注册消息回调,
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

    // 设置线程数量, 1个IO线程 3个worker线程,
    _server.setThreadNum(4);
}

ChatServer::~ChatServer()
{
}

// 启动服务,
void ChatServer::start()
{
    _server.start();
}

// 上报连接相关信息的回调函数,
void ChatServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // 客户端断开连接,
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
    // else
    // {
    //     LOG_INFO << "ChatServer - " << conn->peerAddress().toIpPort() << " -> "
    //              << conn->localAddress().toIpPort() << " state is UP";
    // }
}

// 上报读写事件相关信息的回调函数,
void ChatServer::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp time)
{
    string buf = buffer->retrieveAllAsString();

    // 数据的反序列化,
    nlohmann::json js = json::parse(buf);

    // 目的: 完全解耦网络模块的代码和业务模块的代码,
    // 通过 js["msgid"] 绑定回调操作, 获取handler => conn  js  time, 
    MsgHandler msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());

    // 回调消息绑定好的事件处理器,来执行相应的业务处理,
    msgHandler(conn, js, time);

    
}
