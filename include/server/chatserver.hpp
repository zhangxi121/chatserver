#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

// 聊天服务器的主类,
class ChatServer
{
public:
    // 初始化聊天服务器对象,
    ChatServer(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &listenAddr,
               const string &nameArg,
               TcpServer::Option option = TcpServer::kNoReusePort);
    ~ChatServer();

    // 启动服务,
    void start();

private:
    // 上报连接相关信息的回调函数,
    void onConnection(const muduo::net::TcpConnectionPtr &conn);

    // 上报读写事件相关信息的回调函数,
    void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time);

private:
    muduo::net::TcpServer _server; // 组合的muduo库, 实现服务器功能的类对象,
    muduo::net::EventLoop *_loop;  // 指向时间循环对象的指针,
};

#endif