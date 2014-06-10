// Singleton��ͳһʵ��
// ȱ�㣺1. û�п��Ƕ�������٣������������˳�ʱ��Ȼ���ͷ�������Դ (2012-12-3�ѽ��)
//       2. Tֻ����Ĭ�Ϲ��캯������֧�ִ������Ĺ��캯��
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
        // �ڳ������ʱ��ϵͳ�����������������
        static TrashClean s_trashClean;
    }
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
    ~Singleton() {}

private:
    static pthread_once_t s_once;
    static T* s_pInstance;

// �����ڳ���������ͷž�̬�������ڴ�
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
