#pragma once

/****************************************************************************
** 1. �����ֲܾ���������ȫ�ֱ����ȹ̶������ڴ淽ʽ
** 2. ����������
** 3. new/malloc�������ڴ棬����ָ�����ʽ�ͷţ�������ʾ�ͷ�
** 4. new/malloc�������ڴ棬һ�������һ������ָ���ʱ�򣬾Ͳ����ٸ�ֵ���ڶ�������ָ�룬�����Ҫ�������Ʋ�������������ָ�����
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
	int *m_count;                     // m_ptr���õĴ���
	T *m_ptr;
	int *m_ptrIsDelete;             //TRUE:ɾ�� FALSE:δɾ��
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
	/* ָ�븳ֵ                                                                          */
	/************************************************************************/
	void Add(SmartPtr& ths)
	{
		if (ths.m_pMutex == NULL)
			return;

		// ths���������ü����ڣ�����Ҫ�ж�ָ���Ƿ�Ϊ�գ����Ϊ�գ���ʾ�߼�������
		AutoLock locker(ths.m_pMutex);

		// �����ֵ��ָ���Ѿ����ù�delete���򱻸�ֵ��ָ�����
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
	/* ɾ��ָ�룬������                                                              */
	/************************************************************************/
	void DeleteForNotBlock()
	{
		if (m_pMutex == NULL)
			return;

		m_pMutex->Lock();
		--*m_count;

		// �����ü���Ϊ0��ʱ�򣬱�ʾm_ptrֻ�ڸ�����ָ�����Ѿ����
		// ���ø�ָ���ʱ����ָ����Ҫ�Ѿ���ס��
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
	/* �Ƚ������������                                                              */
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
	/* ��ֵ�����������                                                              */
	/************************************************************************/
	SmartPtr& operator=(SmartPtr& ths)
	{
		if (this == &ths)
			return *this;

		// ��Ҫ������һ���ж����֮�󣬷�����ܻ��������
		AutoLock locker(&m_mutex);
		AutoLock locker1(&ths.m_mutex);

		DeleteForNotBlock();
		Add(ths);

		return *this;
	}

	// ������Ҫʹ�øú�������������߼�����
	/*SmartPtr& operator=(T *ths)
	{
		DeleteForNotBlock();

		Init(ths);

		return *this;
	}*/

	/************************************************************************/
	/* ָ�������������                                                               */
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
	/* �����������                                                                     */
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
