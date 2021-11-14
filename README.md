# chatserver
可以工作在nginx tcp 负载均衡环境中的集群聊天服务器和客户端源码,基于muduo库实现，redis mysql 

编译方式
cd build 
rm -rf  *
cmake ../
make 

运行，需要nginx tcp负载均衡

