# 单例模式

### 什么是单例

单例 Singleton 是设计模式的一种，其特点是只提供**唯一**一个类的实例,具有全局变量的特点，在任何位置都可以通过接口获取到那个唯一实例;
具体运用场景如：

1. 设备管理器，系统中可能有多个设备，但是只有一个设备管理器，用于管理设备驱动;
2. 数据池，用来缓存数据的数据结构，需要在一处写，多处读取或者多处写，多处读取;

### 基础要点

- 全局只有一个实例：static 特性，同时禁止用户自己声明并定义实例（把构造函数设为 private）
- 线程安全
- 禁止赋值和拷贝
- 用户通过接口获取实例：使用 static 类成员函数

### 懒汉式

#### 有缺陷的懒汉式

懒汉式(Lazy-Initialization)的方法是直到使用时才实例化对象，也就说直到调用get_instance() 方法的时候才 new 一个单例的对象， 如果不被调用就不会占用内存。

```cpp
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
```

运行结果：

```cpp
构造函数
```

**问题**：

1. **线程安全的问题**,当多线程获取单例时有可能引发竞态条件：第一个线程在if中判断 `m_instance_ptr`是空的，于是开始实例化单例;同时第2个线程也尝试获取单例，这个时候判断`m_instance_ptr`还是空的，于是也开始实例化单例;这样就会实例化出两个对象,这就是线程安全问题的由来; **解决办法**:加锁
2. **内存泄漏**. 注意到类中只负责new出对象，却没有负责delete对象，因此只有构造函数被调用，析构函数却没有被调用;因此会导致内存泄漏。**解决办法**： 使用共享指针;

#### 线程安全、内存安全的懒汉式单例 （智能指针，锁）

```cpp
#include <iostream>
#include <memory> // shared_ptr
#include <mutex>  // mutex

using namespace std;

//版本2.0
//1.使用互斥锁解决线程安全问题
//2.使用智能指针解决内存泄露问题

class Lazy_V2_Singleton
{
private:
    Lazy_V2_Singleton()
    {
        cout<<"构造函数"<<endl;
    }

    Lazy_V2_Singleton(Lazy_V2_Singleton&) = delete;
    Lazy_V2_Singleton& operator= (const Lazy_V2_Singleton&) = delete;

    static shared_ptr<Lazy_V2_Singleton> m_instance_ptr;
    static mutex m_mutex;

public:
    ~Lazy_V2_Singleton()
    {
        cout<<"析构函数"<<endl;
    }

    static shared_ptr<Lazy_V2_Singleton> get_instance()
    {
        if(m_instance_ptr == nullptr)
        {
            //上锁 RAII
            lock_guard<mutex> lk(m_mutex);
            if(m_instance_ptr == nullptr)
            {
                m_instance_ptr = shared_ptr<Lazy_V2_Singleton>(new Lazy_V2_Singleton);
            }
        }
        return m_instance_ptr;
    }
};

shared_ptr<Lazy_V2_Singleton> Lazy_V2_Singleton::m_instance_ptr = nullptr;
mutex Lazy_V2_Singleton::m_mutex;
int main()
{
    shared_ptr<Lazy_V2_Singleton> instance = Lazy_V2_Singleton::get_instance();
    shared_ptr<Lazy_V2_Singleton> instance_2 = Lazy_V2_Singleton::get_instance();
    return 0;
}
```

**运行结果**：可以看到确实发生了析构

```cpp
构造函数
析构函数
```

**优点：**

- 基于 shared_ptr, 用了C++比较倡导的 RAII思想，用对象管理资源,当 shared_ptr 析构的时候，new 出来的对象也会被 delete掉。以此避免内存泄漏。
- 加了锁，使用互斥量来达到线程安全。这里使用了两个 if判断语句的技术称为**双检锁**；好处是，只有判断指针为空的时候才加锁，避免每次调用 get_instance的方法都加锁，锁的开销毕竟还是有点大的。

**缺点：**

- 使用智能指针会要求用户也得使用智能指针，非必要不应该提出这种约束; 使用锁也有开销; 同时代码量也增多了，实现上我们希望越简单越好。
- 在某些平台（与编译器和指令集架构有关），双检锁会失效！具体可以看[这篇文章](http://www.drdobbs.com/cpp/c-and-the-perils-of-double-checked-locki/184405726)，解释了为什么会发生这样的事情。

#### 最推荐的懒汉式单例(magic static )——局部静态变量

```cpp
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
```

**运行结果**：

```cpp
构造函数
析构函数
```

这种方法又叫做 Meyers' SingletonMeyer's的单例， 是著名的写出《Effective C++》系列书籍的作者 Meyers 提出的。所用到的特性是在C++11标准中的Magic Static特性：

*If control enters the declaration concurrently while the variable is being initialized, the concurrent execution shall wait for completion of the initialization.*
*如果当变量在初始化的时候，并发同时进入声明语句，并发线程将会阻塞等待初始化结束。*

这样保证了并发线程在获取静态局部变量的时候一定是初始化过的，所以具有线程安全性。

C++静态变量的生存期 是从声明到程序结束，这也是一种懒汉式。

**这是最推荐的一种单例实现方式：**

1. 通过局部静态变量的特性保证了线程安全 (C++11, GCC > 4.3, VS2015支持该特性);
2. 不需要使用共享指针，代码简洁；
3. 注意在使用的时候需要声明单例的引用 `Single&` 才能获取对象。

另外网上有人的实现返回指针而不是返回引用

```cpp
static Singleton* get_instance()
{
    static Singleton instance;
    return &instance;
}
```

这样做并不好，理由主要是无法避免用户使用`delete instance`导致对象被提前销毁。还是建议大家使用返回引用的方式。

### 饿汉式

```cpp
#include <iostream>

using namespace std;

class Hungry
{
private:
    Hungry()
    {
        cout<<"构造函数"<<endl;
    }
    Hungry(Hungry&) = delete;
    Hungry operator= (Hungry&) = delete;
    static Hungry* m_instance_ptr;

public:
    ~Hungry()
    {
        cout<<"析构函数"<<endl;
    }

    static Hungry* get_instance()
    {
        return m_instance_ptr;
    }
};
Hungry* Hungry::m_instance_ptr = new Hungry();

int main()
{
    Hungry* instance = Hungry::get_instance();
    Hungry* instance_1 = Hungry::get_instance();
    return 0;
}
```

**饿汉模式** 也就是在类加载的时候就饿了，非要创建实例，不考虑创建对象的内存开销

**缺点:**

**1.资源的占用:**
饿汉模式 在类创建的就得new好它的静态成员对象 故占用空间

**2.类中静态成员 初始化顺序不确定所导致的异常.**
也就是说类中的两个静态成员
 getInstance和Instance不同编译单元中的初始化顺序是未定义的,
如果在Instance初始化完成之前调用
getInstance() 方法会返回一个未定义的实例

