#include "json.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
using json = nlohmann::json;

//
//
// Ctrl+F5
//
//

void func1()
{
    nlohmann::json js;
    js["msg_type"] = 2;
    js["from"] = "zhangsan";
    js["to"] = "lisi";
    js["msg"] = "Hello World!";
    cout << js << endl;

    string sendBuf = js.dump();
    cout << sendBuf.c_str() << endl;
}

void func2()
{
    json js;
    // 添加数组
    js["id"] = {1, 2, 3, 4, 5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu hehe"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu hehe", "hello china"}}; // 覆盖了上面的  js["msg"],
    cout << js << endl;
}

void func3()
{
    json js;
    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;
    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;
    cout << js << endl;

    string sendBuf = js.dump();
    cout << sendBuf.c_str() << endl;
}

std::string rfunc1()
{
    nlohmann::json js;
    js["msg_type"] = 2;
    js["from"] = "zhangsan";
    js["to"] = "lisi";
    js["msg"] = "Hello World!";
    cout << js << endl;

    string sendBuf = js.dump();
    return sendBuf;
}

std::string rfunc2()
{
    json js;
    // 添加数组
    js["id"] = {1, 2, 3, 4, 5};
    js["name"] = "zhang san";
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu hehe"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu hehe", "hello china"}};
    cout << js << endl;
    return js.dump();
}

std::string rfunc3()
{
    json js;
    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;
    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;
    cout << js << endl;

    string sendBuf = js.dump();
    return sendBuf;
}

int main()
{
    // func1();
    // func2();
    // func3();

    // std::string recvBuf = rfunc1();
    // nlohmann::json jsbuf = json::parse(recvBuf); // json 字符串反序列化成数据对象,key-value,
    // cout << jsbuf["msg_type"] << endl;
    // cout << jsbuf["from"] << endl;
    // cout << jsbuf["to"] << endl;
    // cout << jsbuf["msg"] << endl;

    // std::string recvBuf = rfunc2();
    // nlohmann::json jsbuf = json::parse(recvBuf); // json 字符串反序列化成数据对象,key-value,
    // cout << jsbuf["id"] << endl;
    // auto arr = jsbuf["id"];
    // cout <<arr[0] <<endl;
    // cout <<arr[2] <<endl;
    // auto msgbuf = jsbuf["msg"];
    // cout << msgbuf["zhang san"] << endl;
    // cout << msgbuf["liu hehe"] << endl;

    std::string recvBuf = rfunc3();
    nlohmann::json jsbuf = json::parse(recvBuf); // json 字符串反序列化成数据对象,key-value,
    vector<int> vec = jsbuf["list"];
    for (int &v : vec)
    {
        cout << v << " ";
    }
    cout << endl;
    map<int, string> mymap = jsbuf["path"];
    for (auto &pair : mymap)
    {
        cout << pair.first << ":" << pair.second << endl;
    }
    cout << endl;

    return 0;
}

/////////////////////////////////////////////////// 


static int add(int a, int b)
{
    return a + b;
}

static int sub(int a, int b)
{
    return a - b;
}

std::string rfunc_01()
{
    json js;
    typedef int(*fun)(int,int); 
    
    map<std::string, fun> m;
    m.insert({"add", add});
    m.insert({"sub", sub});
    js["path"] = m;
    cout << js << endl;

    string sendBuf = js.dump();
    return sendBuf;
}


int main02()
{
    typedef int(*fun)(int,int); 

    std::string recvBuf = rfunc_01();
    // nlohmann::json jsbuf = json::parse(recvBuf); // json 字符串反序列化成数据对象,key-value,
    // map<std::string, fun> mymap = jsbuf["path"];

    // for (auto &pair : mymap)
    // {
    //     // cout << pair.first << ":" << pair.second << endl;
    //     cout << pair.second (5,3) << endl;
    // }
    // cout << endl;
}