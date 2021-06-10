#include <iostream>

using namespace std;

class Lazy_V3_Singleton
{
private:
    Lazy_V3_Singleton()
    {
        cout<<"构造函数"<<endl;
    }
    Lazy_V3_Singleton(Lazy_V3_Singleton&) = delete;
    Lazy_V3_Singleton operator= (Lazy_V3_Singleton&) = delete;

public:
    ~Lazy_V3_Singleton()
    {
        cout<<"析构函数"<<endl;
    }

    static Lazy_V3_Singleton& get_instance()
    {
        static Lazy_V3_Singleton instance;
        return instance;
    }
};

int main()
{
    Lazy_V3_Singleton& instance_1 = Lazy_V3_Singleton::get_instance();
    Lazy_V3_Singleton& instance_2 = Lazy_V3_Singleton::get_instance();
    return 0;
}