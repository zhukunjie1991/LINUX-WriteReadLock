#include "logfile.h"
#include "error_def.h"
#include "DynamicLoader.h"

#include <stdio.h>
#ifdef WIN32
#include <atlstr.h>
#else
#include <dlfcn.h>
#endif

DynamicLoader::DynamicLoader(const std::string& dllName)
{
#ifdef WIN32
    hDllLib = LoadLibrary((LPCSTR)dllName.c_str());
#else
    //以指定模式打开制定的动态连接库文件。
    pHandle = dlopen(dllName.c_str(), RTLD_LAZY);
#endif
}

DynamicLoader::~DynamicLoader(void)
{
//hauk 2013-09-12 start NTSTR00001301
#if 0
#ifdef WIN32
	FreeLibrary(hDllLib);
#else
	dlclose(pHandle);
#endif
#else
	DestroyComp();
#endif
//end NTSTR00001301
}

void* DynamicLoader::GetObj()
{
#ifdef WIN32
    // 1. 判断dll库是否加载成功
    if (hDllLib == NULL) {
        printf("LoadLibrary error.\n");
        return NULL;
    }
    // 2. 获取dll库中的Create函数的函数指针
    FARPROC funCreate = GetProcAddress(hDllLib, "Create");
    if (funCreate == NULL) {
        printf("GetProcAddress error.\n");
        return NULL;
    }
#else
    // 1. 判断dll库是否加载成功
    if (pHandle == NULL) {
        printf("dlopen error: %s.\n", dlerror());
        return NULL;
    }
    typedef void* (*method_t)();
    // 2. 获取dll库中的Create函数的函数指针
    method_t funCreate =(method_t)dlsym(pHandle, "Create");
    if (funCreate == NULL) {
        printf("dlsym error: %s.\n", dlerror());
        return NULL;
    }
#endif
    // 3. 调用dll库中的Create函数，返回dll中类对象实例
    void *pObject = (void *)(*funCreate)();
    return pObject;
}

//hauk 2013-09-12 start NTSTR00001301

//default constructor without any parameter
//set the library pointer to null
DynamicLoader::DynamicLoader(void)
{
#ifdef WIN32
	hDllLib = NULL;
#else
	pHandle = NULL;
#endif
}

//load dynamic component
int DynamicLoader::LoadComp(const std::string& strDllName)
{
#ifdef WIN32
	hDllLib = LoadLibrary((LPCSTR)strDllName.c_str());
	if(NULL == hDllLib)
	{
        LOG_ERROR(ERROR_CODE(APP_DYNAMIC_LOAD_FAILED), 
			"DynamicLoader::LoadComp() load dynamic lib fail dllname=%s", strDllName.c_str());
		return ERROR_CODE(APP_DYNAMIC_LOAD_FAILED);
	}
#else
	pHandle = dlopen(strDllName.c_str(), RTLD_LAZY);
	if(NULL == pHandle)
	{
		LOG_ERROR(ERROR_CODE(APP_DYNAMIC_LOAD_FAILED), 
			"DynamicLoader::LoadComp() load dynamic lib fail dllname=%s", strDllName.c_str());
		return ERROR_CODE(APP_DYNAMIC_LOAD_FAILED);
	}
#endif

	return UTILITY::SUCCESS;
}

//Get component function pointer
void* DynamicLoader::GetCompFunc(const std::string& strFuncName)
{
#if WIN32
	//[1]check if dynamic library pointer is null first
	if(NULL == hDllLib)
	{
		LOG_ERROR(ERROR_CODE(APP_NULL_POINTER_OBJ), 
			"DynamicLoader::GetCompFunc() hDllLib is null, func name = %s", strFuncName.c_str());
		return NULL;
	}

	//[2]get function pointer required by function name parameter
	FARPROC pCompFunc = GetProcAddress(hDllLib, strFuncName.c_str());
	if (NULL == pCompFunc) 
	{
		LOG_ERROR(ERROR_CODE(APP_CALL_FUNC_FAILED), 
			"DynamicLoader::GetCompFunc() get func fail, func name = %s", strFuncName.c_str());
		return NULL;
	}

	return pCompFunc;

#else
	//[1]check if dynamic library pointer is null first
	if(NULL == pHandle)
	{
		LOG_ERROR(ERROR_CODE(APP_NULL_POINTER_OBJ), 
			"DynamicLoader::GetCompFunc() pHandle is null, func name = %s", strFuncName.c_str());
		return NULL;
	}

	void *pCompFunc = dlsym(pHandle, strFuncName.c_str());
	if (NULL == pCompFunc)
	{
		LOG_ERROR(ERROR_CODE(APP_CALL_FUNC_FAILED), 
			"DynamicLoader::GetCompFunc() get func fail, func name = %s", strFuncName.c_str());
		return NULL;
	}

	return pCompFunc;
#endif
}

//Destroy Dynamic component
int DynamicLoader::DestroyComp(void)
{
	//confirm the dynamic library is not null before free
#ifdef WIN32
	if(hDllLib)
	{
		FreeLibrary(hDllLib);
		hDllLib = NULL;
	}
#else
	if(pHandle)
	{
		dlclose(pHandle);
		pHandle = NULL;
	}
#endif

	return UTILITY::SUCCESS;
}
//end NTSTR00001301