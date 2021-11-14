
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/function.hpp>

using namespace std;
using namespace boost;

// 
// g++ main.cpp  -o main -std=c++11
// 

class Hello
{
public:
    void say(string name)
    {
        cout << name << " say: hello world!" << endl;
    }
};

int main()
{
    Hello h;
    auto func = boost::bind(&Hello::say, &h, "zhangsan");
    func();
    return 0;
    return 0;
}
