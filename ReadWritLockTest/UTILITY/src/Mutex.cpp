// mutex.cpp: implementation of the CMUTEX class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>

#include "Mutex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMUTEX::CMUTEX()
{
   pthread_mutex_init(&m_lock,NULL);
}

CMUTEX::~CMUTEX()
{
   pthread_mutex_destroy(&m_lock);
}

void CMUTEX::Lock()
{
		pthread_mutex_lock(&m_lock);
}

int CMUTEX::TryLock()
{
		return pthread_mutex_trylock(&m_lock);
}

void CMUTEX::UnLock()
{
		pthread_mutex_unlock(&m_lock);
}

//////////////////////////////////////////////////////////////////////
// CPMUTEX Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AutoLock::AutoLock(CMUTEX *pmutex)
{
   p_mutex = pmutex;
   p_mutex->Lock();
}

AutoLock::~AutoLock()
{
   p_mutex->UnLock();
}


RWLock::RWLock()
{
    // add by WuJinghe 2012-12-14
    // for using pthread library substitute original implementation
    // m_readers = 0;
    if (pthread_rwlock_init(&m_rwlock, NULL))
        printf("Cann't create reader/writer lock.\n");
    // end add 2012-12-14
}

RWLock::~RWLock()
{
    // add by WuJinghe 2012-12-14
    // for using pthread library substitute original implementation
    // m_readers = 0;
    if (pthread_rwlock_destroy(&m_rwlock))
        printf("Cann't destroy reader/writer lock.\n");
    // end add 2012-12-14

}

void RWLock::ReadLock()
{
    // mdf by WuJinghe 2012-12-14
    // for using pthread library substitute original implementation
    // m_readers = 0;
    // m_readLock.Lock();
    // ++m_readers;
    // if (m_readers == 1)
    // {
    //     m_writeLock.Lock();
    // }
    // m_readLock.UnLock();
    if (pthread_rwlock_rdlock(&m_rwlock))
        printf("Cann't lock reader lock.\n");
    // end mdf 2012-12-14
}

void RWLock::WriteLock()
{
    // mdf by WuJinghe 2012-12-14
    // for using pthread library substitute original implementation
    // m_writeLock.Lock();
    if (pthread_rwlock_wrlock(&m_rwlock))
        printf("Cann't lock writer lock.\n");
    // end mdf 2012-12-14
}

void RWLock::UnlockRead()
{
    // mdf by WuJinghe 2012-12-14
    // for using pthread library substitute original implementation
    // m_readLock.Lock();
    // --m_readers;
    // if (m_readers == 0)
    // {
    //     m_writeLock.UnLock();
    // }
    // m_readLock.UnLock();
    if (pthread_rwlock_unlock(&m_rwlock))
        printf("Cann't unlock reader lock.\n");
    // end mdf 2012-12-14
}

void RWLock::UnlockWrite()
{
    // mdf by WuJinghe 2012-12-14
    // for using pthread library substitute original implementation
    // m_writeLock.UnLock();
    if (pthread_rwlock_unlock(&m_rwlock))
        printf("Cann't unlock writer lock.\n");
    // end mdf 2012-12-14
}

