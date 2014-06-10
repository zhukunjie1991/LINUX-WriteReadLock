#pragma once

/****************************************************************************
** 1. 不接受局部变量或者全局变量等固定分配内存方式
** 2. 不接受数组
** 3. new/malloc出来的内存，智能指针会隐式释放，不能显示释放
** 4. new/malloc出来的内存，一旦分配给一个智能指针的时候，就不能再赋值给第二个智能指针，如果需要进行类似操作，请用智能指针操作
**                                                              by Wang Zhaoyang
****************************************************************************/

#include <iostream>
#include "Mutex.h"
#include <string>
using namespace std;

template <typename T>
class SmartPtr
{
private:
	int *m_count;                     // m_ptr引用的次数
	T *m_ptr;
	int *m_ptrIsDelete;             //TRUE:删除 FALSE:未删除
	CMUTEX *m_pMutex;

	CMUTEX m_mutex;

	void InitPtr()
	{
		m_count = NULL;
		m_ptr = NULL;
		m_pMutex = NULL;
		m_ptrIsDelete = NULL;
	}

	void DestroyPtr()
	{
		delete m_count;
		delete m_ptr;
		delete m_pMutex;
		delete m_ptrIsDelete;
	}

	void Init(T* ths)
	{
		m_count = new int;
		*m_count = 1;
		m_pMutex = new CMUTEX;
		m_ptr = ths;
		m_ptrIsDelete = new int;
		*m_ptrIsDelete = FALSE;
	}

	/************************************************************************/
	/* 指针赋值                                                                          */
	/************************************************************************/
	void Add(SmartPtr& ths)
	{
		if (ths.m_pMutex == NULL)
			return;

		// ths本身有引用计数在，不需要判断指针是否为空，如果为空，表示逻辑出问题
		AutoLock locker(ths.m_pMutex);

		// 如果赋值的指针已经调用过delete，则被赋值的指针清空
		if (*(ths.m_ptrIsDelete) == TRUE)
		{
			InitPtr();
			return;
		}

		++*ths.m_count;
		m_count = ths.m_count;
		m_ptr = ths.m_ptr;
		m_pMutex = ths.m_pMutex;
		m_ptrIsDelete = ths.m_ptrIsDelete;
	}

	/************************************************************************/
	/* 删除指针，不阻塞                                                              */
	/************************************************************************/
	void DeleteForNotBlock()
	{
		if (m_pMutex == NULL)
			return;

		m_pMutex->Lock();
		--*m_count;

		// 当引用计算为0的时候，表示m_ptr只在该智能指针中已经完成
		// 调用该指针的时候，在指针需要已经锁住了
		if (*m_count == 0)
		{
			m_pMutex->UnLock();
			DestroyPtr();
			InitPtr();

			return;
		}
		m_pMutex->UnLock();

		InitPtr();
	}

public:
	SmartPtr()
	{
		AutoLock locker(&m_mutex);
		InitPtr();
	}

	SmartPtr(SmartPtr<T> &ths)
	{
		AutoLock locker(&m_mutex);
		AutoLock locker1(&ths.m_mutex);
		InitPtr();
		Add(ths);
	}

	SmartPtr(const SmartPtr<T> &ths)
	{
		AutoLock locker(&m_mutex);
		AutoLock locker1((CMUTEX *)&ths.m_mutex);
		InitPtr();
		Add(const_cast<SmartPtr&>(ths));
	}

	SmartPtr(T *t)
	{
		AutoLock locker(&m_mutex);
		Init(t);
	}

	~SmartPtr()
	{
		AutoLock locker(&m_mutex);
		DeleteForNotBlock();
	}

	void Delete()
	{
		AutoLock locker(&m_mutex);
		if (m_pMutex == NULL)
			return;

		m_pMutex->Lock();

		*m_ptrIsDelete = TRUE;

		m_pMutex->UnLock();
	}

	BOOL IsDelete()
	{
		AutoLock locker(&m_mutex);
		if (m_pMutex == NULL)
			return TRUE;
		AutoLock locker1(m_pMutex);

		if ((m_ptrIsDelete == NULL) || (*m_ptrIsDelete == TRUE))
			return TRUE;
		else
			return FALSE;
	}

	/************************************************************************/
	/* 比较运算符号重载                                                              */
	/************************************************************************/
	/*bool operator==(int v)
	{
		AutoLock locker(m_pMutex);
		return m_ptr == v;
	}

	bool operator==(SmartPtr &ths)
	{
		AutoLock locker1(m_pMutex);
		AutoLock locker2(ths.m_pMutex);

		return m_ptr == ths.m_ptr;
	}*/

	/************************************************************************/
	/* 赋值运算符号重载                                                              */
	/************************************************************************/
	SmartPtr& operator=(SmartPtr& ths)
	{
		if (this == &ths)
			return *this;

		// 需要放在上一个判断语句之后，否则可能会造成死锁
		AutoLock locker(&m_mutex);
		AutoLock locker1(&ths.m_mutex);

		DeleteForNotBlock();
		Add(ths);

		return *this;
	}

	// 尽量不要使用该函数，以免造成逻辑错误
	/*SmartPtr& operator=(T *ths)
	{
		DeleteForNotBlock();

		Init(ths);

		return *this;
	}*/

	/************************************************************************/
	/* 指针运算符号重载                                                               */
	/************************************************************************/
	T *operator->()
	{
		AutoLock locker1(&m_mutex);
		if (m_pMutex == NULL)
			return NULL;
		AutoLock locker(m_pMutex);
		return m_ptr;
	}

	T &operator*()
	{
		AutoLock locker1(&m_mutex);
		if (m_pMutex == NULL)
			return *m_ptr;
		AutoLock locker(m_pMutex);
		return *m_ptr;
	}

	/************************************************************************/
	/* 输入输出重载                                                                     */
	/************************************************************************/
	friend ostream& operator << (ostream &ot, SmartPtr &ths)
	{
		ot << *ths.m_ptr;
		return ot;
	}

	friend istream& operator >> (istream &it, SmartPtr &ths)
	{
		it >> *ths->m_ptr;
		return it;
	}
};
