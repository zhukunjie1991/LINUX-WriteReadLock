/*
*****************************************************************************
*
*   Copyright (C) 2006-2012, Sungard China
*
*****************************************************************************
  FileName:atomic_op.hpp
  Description: 
 ***********[History]******************************************************
  Created on: 2012/3/26 10:58:18  Zhangyunchao
*/

#if defined(__xlC__)
	#include <builtins.h>
#endif

#if defined(_MSC_VER)
#include <windows.h>
#include <intrin.h>
#pragma intrinsic(_ReadWriteBarrier)
#endif

inline void memoryBarrier()
{
#if defined (__GNUC__)
    __sync_synchronize();
#elif defined(_MSC_VER)
    _ReadWriteBarrier();
#elif defined(__xlC__)
    __fence();
    __eieio();
    __fence();
#else
#  error "no memory barrier implemented for this platform"
#endif
}
#ifdef _WIN32

inline CAtomic::CAtomic(unsigned Value) : m_Value(Value)
{
}


inline unsigned CAtomic::set(unsigned Value)
{
    return InterlockedExchange((long *)&m_Value, long(Value));
}


inline unsigned CAtomic::get()
{
//    MemoryBarrier();
//    return static_cast<unsigned const volatile &>(m_Value);
    return InterlockedExchangeAdd((long *)&m_Value, 0L);
}


inline unsigned CAtomic::add(unsigned Value)
{
    return InterlockedExchangeAdd((long *)&m_Value, long(Value));
}

inline unsigned CAtomic::sub(unsigned Value)
{
    return InterlockedExchangeAdd((long *)&m_Value, -long(Value));
}


inline unsigned CAtomic::operator ++()
{
    return InterlockedIncrement((long *)&m_Value);
}


inline unsigned CAtomic::operator --()
{
    return InterlockedDecrement((long *)&m_Value);
}

/*
inline CAtomic::operator unsigned()
{
    return get();
}
*/

inline bool CAtomic::operator == (unsigned Value)
{
	memoryBarrier();
    return m_Value == Value;
}



//==============================================================================
/*
 * Linux implementation of the class CAtomic.
 */

#elif defined (__GNUC__) || (defined(__IBMCPP__) && defined (_AIX) && (__IBMCPP__ >= 1210))

inline CAtomic::CAtomic(unsigned Value) : m_Value(Value)
{
}


inline unsigned CAtomic::set(unsigned Value)
{
    return __sync_lock_test_and_set(&m_Value, Value);
}


inline unsigned CAtomic::get()
{
    __sync_synchronize();
    return m_Value;
}


inline unsigned CAtomic::add(unsigned Value)
{
    return __sync_fetch_and_add(&m_Value, Value);
}


inline unsigned CAtomic::sub(unsigned Value)
{
    return __sync_fetch_and_sub(&m_Value, Value);
}


inline unsigned CAtomic::operator ++()
{
    return __sync_add_and_fetch(&m_Value, 1);
}


inline unsigned CAtomic::operator --()
{
    return __sync_sub_and_fetch(&m_Value, 1);
}

/*
inline CAtomic::operator unsigned()
{
    __sync_synchronize();
    return m_Value;
}
*/

inline bool CAtomic::operator == (unsigned Value)
{
	memoryBarrier();
    return this->m_Value == Value;
}

#elif (defined(__IBMCPP__) && defined (_AIX) && (__IBMCPP__ < 1210))

#include <builtins.h>

inline unsigned int lwarxu(volatile unsigned int * mem)
{
   return __lwarx((reinterpret_cast<volatile int *>(mem)));
}

inline bool stwcxu(volatile unsigned int * mem, unsigned int val)
{
   return (__stwcx(reinterpret_cast<volatile int *>(mem), (int)val) != 0);
}

inline CAtomic::CAtomic(unsigned Value) : m_Value(Value)
{
}


inline unsigned CAtomic::set(unsigned Value)
{
  	memoryBarrier();
    unsigned int oldValue = m_Value;
    m_Value = Value;
    return oldValue;
}

inline unsigned CAtomic::get()
{
  	memoryBarrier();
    return m_Value;		 
}

inline unsigned CAtomic::add(unsigned value)
{
    unsigned int oldValue;
    do
    {
        oldValue = lwarxu(&m_Value);
    }while(!stwcxu(&m_Value, oldValue + value));
    return oldValue;
}


inline unsigned CAtomic::sub(unsigned Value)
{
    return this->add(-Value);
}

inline unsigned CAtomic::operator ++()
{
    return this->add(1);
}

inline unsigned CAtomic::operator --()
{
    return this->add(-1);
}

inline bool CAtomic::operator == (unsigned Value)
{
  	memoryBarrier();
    return this->m_Value == Value;
}

#endif //

