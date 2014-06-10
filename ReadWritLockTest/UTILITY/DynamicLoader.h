#pragma once

#ifdef WIN32
#include <atlstr.h>
#else
typedef void* (*method_t)();
#endif
#include <string>

class DynamicLoader {
public:
	//deprecated function 
	//use DynamicLoader() and LoadComp(const std::string& strDllName) to instead of it 
    DynamicLoader(const std::string& dllName);
    ~DynamicLoader(void);

    // ��ȡ��̬���еĶ���ʵ����**�ö����ɵ������Լ��ͷ��ڴ�**
	//deprecated function 
	//use GetCompFunc(const std::string& strFuncName) and call the return func pointer to instead of it
    void* GetObj();

	//hauk 2013-09-12 start NTSTR00001301
	//default constructor without any parameter
	DynamicLoader(void);

	//load dynamic component
	int LoadComp(const std::string& strDllName);

	//Get component function pointer
	void* GetCompFunc(const std::string& strFuncName);

	//Destroy Dynamic component
	int DestroyComp(void);
	//end NTSTR00001301

private:
#ifdef WIN32
    HMODULE hDllLib;
#else
    void *pHandle;
    method_t funCreate;
    char *errorInfo;
#endif
};
