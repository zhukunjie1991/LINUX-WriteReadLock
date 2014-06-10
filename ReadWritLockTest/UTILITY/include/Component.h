/*******************************************************************************
* ��Ȩ����(C) 2012 China Foreign Exchange Trade System  All rights reserved.
* �ļ����� : Compnent.h
* ��ǰ�汾 : 1.0.0.1
* ��    �� : wujinghe (jinnwu@yahoo.com)
* ������� : 2012��9��17��
* ����ժҪ : 
* �޸ļ�¼ : 
* ��    ��  ��    ��  �޸���   �޸�ժҪ

********************************************************************************/

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <string>

class Component
{
public:
	// �����ʹ�õķ�ʽ
	enum Used_Mode
	{
		SINGLE = 1, // һ�����ֻ�ܱ�һ������ʹ��
		MULTIPLE = 2, // һ������ɱ��������ʹ��
	};

public:
	Component(void)
	{
		m_refCount = 0;
		m_usedMode = SINGLE;
	}
	virtual ~Component(void)
	{}

	virtual int Init() = 0;
	virtual int Finit() = 0;
	virtual int GetType(std::string& type, std::string& subType) const = 0;
	//virtual int GetCompVersion(VersionInfo& ver) const = 0;

	int GetRefCount() const
	{
		return m_refCount;
	}

	void IncreaseRefCount()
	{
		m_refCount++;
	}

	void DecreaseRefCount()
	{
		if (m_refCount <= 0) {
			// ���ü�����������Ǹ���
			// LOG_ERR(CFETS::GetErrorMsg(xxx));
			return;
		}
			m_refCount--;
	}

	Used_Mode GetUsedMode() const
	{
		return m_usedMode;
	}

	void SetUsedMode(Used_Mode usedMode)
	{
		m_usedMode = usedMode;
	}

private:
	int m_refCount;
	Used_Mode m_usedMode;
};

#endif //__COMPONENT_H__
