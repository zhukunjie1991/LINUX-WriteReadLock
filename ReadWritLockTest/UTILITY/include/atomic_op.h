/*
*****************************************************************************
*
*   Copyright (C) 2006-2012, Sungard China
*
*****************************************************************************
  FileName:atomic_op.h
  Description: 
 ***********[History]******************************************************
  Created on: 2012/3/26 10:57:02  Zhangyunchao
*/

#ifndef __ATOMIC_OP_H__
#define __ATOMIC_OP_H__
#pragma  once
// Windows version
#ifdef _WIN32
#    include <Windows.h>
#    include <WinNT.h>
#endif // _WIN32

// Linux version
#ifdef _LINUX
#    include <cstdlib>
#endif // _LINUX

//==============================================================================
/*
 * An atomic increment and decrement operation.
 */
class CAtomic
{
public:
    //  Constructor.
    inline CAtomic(unsigned Value = 0);

    //  Standard destructor.
    ~CAtomic() {}


    //  Returns the current value.
    inline unsigned get();

    //  Changes the value.
    //  - Returns the previous value!
    inline unsigned set(unsigned Value);

    //  Incrementes the value.
    //  - Returns the previous value!
    inline unsigned add(unsigned Value = 1);

    //  Decrementes the value.
    //  - Returns the previous value!
    inline unsigned sub(unsigned Value = 1);


    //  Returns the incremented value.
    inline unsigned operator++();

    //  Returns the decremented value.
    inline unsigned operator--();

    //  Conversion to the integer (returns the current value).
    // inline operator unsigned();

    // Is equal a value
    inline bool operator == (unsigned Value);
protected:
#ifdef _WIN32
    volatile long m_Value;
#else
    volatile unsigned m_Value;
#endif // _LINUX

private:
    //  Private copy constructor.
    CAtomic(const CAtomic&);

    //  Private assignment operator.
    void operator=(const CAtomic&);
};

//==============================================================================
/*
 * Implementation...
 */
#include "atomic_op.hpp"
#endif //__ATOMIC_OP_H__