
#pragma once


#ifdef WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

#include <pthread.h>

/******************
*kikin mdf 2012-9-17
*����tryLock����



*******************/

class CMUTEX  
{
public:
	void UnLock();
	void Lock();

	/****
	retrun 0,��ʾ�ɹ�������ʧ�ܡ�ʧ��ԭ��Ŀǰ�μ�pthread_mutex_trylock�����ķ���ֵ
	*δ����Ҫ����ʵ�ʵ�ʵ���߼����иĶ���
	***/
	int TryLock();
	CMUTEX();
	virtual ~CMUTEX();


protected:
   pthread_mutex_t m_lock;
};


class AutoLock  
{
public:
	AutoLock(CMUTEX *pmutex);
	virtual ~AutoLock();
private:
   CMUTEX *p_mutex;
};

class RWLock
{
public:
    /// Creates the Reader/Writer lock.
    RWLock();
    /// Destroys the Reader/Writer lock.

    ~RWLock();

    /// Acquires a read lock. If another thread currently holds a write lock,
    /// waits until the write lock is released.
    void ReadLock();

    /// Acquires a write lock. If one or more other threads currently hold 
    /// locks, waits until all locks are released. The results are undefined
    /// if the same thread already holds a read or write lock
    void WriteLock();

    /// Releases the read lock.
    void UnlockRead();

    /// Releases the write lock.
    void UnlockWrite();

// mdf by WuJinghe 2012-12-14
// for using pthread library substitute original implementation
// private:
//     int m_readers;
//     CMUTEX m_writeLock;
//     CMUTEX m_readLock;
// private:
//     RWLock(const RWLock&);
//     RWLock& operator = (const RWLock&);
private:
    pthread_rwlock_t m_rwlock;
// end mdf 2012-12-14
};
