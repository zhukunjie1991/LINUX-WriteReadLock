/*******************************************************************************
* 版权所有(C) 2012 China Foreign Exchange Trade System  All rights reserved.
* 文件名称 : Compnent.h
* 当前版本 : 1.0.0.1
* 作    者 : wujinghe (jinnwu@yahoo.com)
* 设计日期 : 2012年9月17日
* 内容摘要 : 
* 修改记录 : 
* 日    期  版    本  修改人   修改摘要

********************************************************************************/

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <string>

class Component
{
public:
	// 组件被使用的方式
	enum Used_Mode
	{
		SINGLE = 1, // 一个组件只能被一个对象使用
		MULTIPLE = 2, // 一个组件可被多个对象使用
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
			// 引用计数已是零或是负数
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
