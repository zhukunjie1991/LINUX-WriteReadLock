// Singleton的统一实现
// 缺点：1. 没有考虑对象的销毁，不过当程序退出时自然会释放所有资源 (2012-12-3已解决)
//       2. T只能是默认构造函数，不支持带参数的构造函数
#include <pthread.h>

template<typename T>
class Singleton : public T {
public:
    static T& GetInstance()
    {
        if (s_pInstance == NULL) {
            pthread_once(&s_once, &Singleton::Init);
        }
        return *s_pInstance;
    }

private:
    static void Init()
    {
        s_pInstance = new T();
    }

    Singleton()
    {
        // 在程序结束时，系统会调用它的析构函数
        static TrashClean s_trashClean;
    }
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
    ~Singleton() {}

private:
    static pthread_once_t s_once;
    static T* s_pInstance;

// 用于在程序结束后，释放静态变量的内存
private:
    class TrashClean {
    public:
        TrashClean() {}
        ~TrashClean()
        {
            if (s_pInstance != NULL) {
                delete s_pInstance;
                s_pInstance = NULL;
            }
        }
    };
};

template<typename T>
pthread_once_t Singleton<T>::s_once = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::s_pInstance = NULL;
