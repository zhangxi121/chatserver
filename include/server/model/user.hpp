#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

// 匹配 User 表的 ORM 类,映射表的字段,
class User
{
public:
    User(int id = -1, std::string name = "", std::string pwd = "", std::string state = "offline")
        : m_id(id), m_name(name), m_password(pwd), m_state(state)
    {
    }

    ~User()
    {
    }

public:
    void setId(int id) { this->m_id = id; }
    void setName(std::string name) { this->m_name = name; }
    void setPwd(std::string pwd) { this->m_password = pwd; }
    void setState(std::string state) { this->m_state = state; }

    int getId(void) const { return m_id; }
    std::string getName(void) const { return m_name; }
    std::string getPwd(void) const { return m_password; }
    std::string getState(void) const { return m_state; }

protected:
    int m_id;
    std::string m_name;
    std::string m_password;
    std::string m_state;
};

#endif
