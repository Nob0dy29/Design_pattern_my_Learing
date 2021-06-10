#include <iostream>

using namespace std;

//版本1.0 有以下缺点
//1.线程不安全
//2.内存泄露

class Lazy_V1_Singleton
{
private:
    Lazy_V1_Singleton()
    {
        cout<<"构造函数"<<endl;
    }

    Lazy_V1_Singleton(Lazy_V1_Singleton&) = delete;
    Lazy_V1_Singleton operator = (const Lazy_V1_Singleton&) = delete;
    static Lazy_V1_Singleton* m_instance_ptr;

public:
    ~Lazy_V1_Singleton()
    {
        cout<<"析构函数"<<endl;
    }

    static Lazy_V1_Singleton* get_instance()
    {
        if(m_instance_ptr == nullptr)
        {
            m_instance_ptr = new Lazy_V1_Singleton;
        }
        return m_instance_ptr;
    }

    void use() const
    {
        cout<<"in use"<<endl;
    }
};

Lazy_V1_Singleton* Lazy_V1_Singleton::m_instance_ptr = nullptr;

int main()
{
    Lazy_V1_Singleton *instance = Lazy_V1_Singleton::get_instance();
    Lazy_V1_Singleton *instance_2 = Lazy_V1_Singleton::get_instance();
    return 0;
}