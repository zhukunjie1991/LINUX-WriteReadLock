#include <assert.h>

#include <algorithm>

//#include "error_handler.h"
#include "VersionInfo.h"
#include "ConfigFile.h"
#include "Component.h"
#include "CompMgr.h"
#include "Singleton.h"
#include "error_def.h"
#include "logfile.h"

CompMgr::CompMgr(void)
{
}

CompMgr::~CompMgr(void)
{
}

//CompMgr* CompMgr::m_compMgr = NULL;
CompMgr* CompMgr::GetInstance()
{
    return &Singleton<CompMgr>::GetInstance();
    // if (m_compMgr == NULL) {
    //     m_compMgr = new CompMgr;
    // }
    // return m_compMgr;
}

int CompMgr::AddCompType(const std::string& type)
{
    if (type == "") {
        RAISE_ERROR(APP_COMP_TYPE_NULL);
        return ERROR_CODE(APP_COMP_TYPE_NULL);
    }
    // 如果type已存在，则不再添加
    ConfigItemArray::iterator iter = std::find(m_sectionArray.begin(),
                                               m_sectionArray.end(),
                                               type);
    if (iter == m_sectionArray.end())
        m_sectionArray.push_back(type);
    else {
        LOG_INFO("The type[%s] is already exist.", type.c_str());
    }

	return UTILITY::SUCCESS;
}

// 读取配置文件中对应项的值，并将值保存到容器中返回
int CompMgr::ReadConfigInfo(const std::string& type,
                            ConfigFile& configFile,
                            ConfigItemArray& configItemArray)
{
    // 1. 从配置文件中，读取组件（共享库）名（包括路径和文件名）
    std::string dllName = configFile.ReadString(type.c_str(),
                                                LibPath,
                                                "");
    if (dllName == "") {
        RAISE_ERROR(APP_UNREAD_CONFIG_ITEM_VALUE);
        LOG_INFO("ERROR - Read item [%s] failed and "
                 "the value is %s in the Type(section): %s.",
                 LibPath, dllName.c_str(), type.c_str());
        return ERROR_CODE(APP_UNREAD_CONFIG_ITEM_VALUE);
    }
    // 将读取到的项存入容器
    configItemArray.push_back(dllName);

    // 2. 从配置文件中，读取组件的子类型名
    std::string subType = configFile.ReadString(type.c_str(),
                                                SubType,
                                                "");
    if (subType == "") {
        RAISE_ERROR(APP_UNREAD_CONFIG_ITEM_VALUE);
        LOG_INFO("ERROR - Read item [%s] failed and "
                 "the value is %s in the Type(section): %s.",
                 SubType, subType.c_str(), type.c_str());
        return ERROR_CODE(APP_UNREAD_CONFIG_ITEM_VALUE);
    }
    // 将读取到的项存入容器
    configItemArray.push_back(subType);

	return UTILITY::SUCCESS;
}

// 根据指定的路径加载组件库(.dll or .so)
typedef Component* (*PCreateFunc)(void);
typedef int (*PGetCompVersionFunc)(VersionInfo*);

int CompMgr::LoadComp(const std::string& dllName, Component** component)
{
    // 1. 加载组件文件(.dll or .so)，并将handle添加到HandleArray中(待释放handle)
#if WIN32
    HMODULE handle = LoadLibrary(dllName.c_str());
#else
    void *handle = dlopen(dllName.c_str(), RTLD_LAZY);
    if (handle == NULL) {
        LOG_INFO("%s load failed: [%s].", dllName.c_str(), dlerror());
    }
#endif
    if (handle == NULL) {
        RAISE_ERROR(APP_DYNAMIC_LOAD_FAILED);
        LOG_INFO("%s load faild.", dllName.c_str());
        return ERROR_CODE(APP_DYNAMIC_LOAD_FAILED);
    }
    m_handleArray.push_back(handle);

    // 2. 取得组件中Create函数和GetCompVersion函数
#if WIN32
    PCreateFunc funCreate = (PCreateFunc)GetProcAddress(handle, "Create");
    if (funCreate == NULL) {
        RAISE_ERROR(APP_CALL_FUNC_FAILED);
        LOG_INFO("The function [Create] called failed in the [%s] library.",
                 dllName.c_str());
	return ERROR_CODE(APP_CALL_FUNC_FAILED);
    }

    //typedef int (__cdecl *PGetCompVersionFunc)(VersionInfo*);
    PGetCompVersionFunc funGetCompVersion =
        (PGetCompVersionFunc)GetProcAddress(handle, "GetCompVersion");
    if (funGetCompVersion == NULL) {
        RAISE_ERROR(APP_CALL_FUNC_FAILED);
        LOG_INFO("The function [GetCompVersion] called failed "
                 "in the [%s] library.",
                 dllName.c_str());
        return ERROR_CODE(APP_CALL_FUNC_FAILED);
    }
#else
    PCreateFunc funCreate = (PCreateFunc)dlsym(handle, "Create");
    if (funCreate == NULL) {
        RAISE_ERROR(APP_CALL_FUNC_FAILED);
        LOG_INFO("The function [Create] called failed in %s lib. [%s]",
            dllName.c_str(),
                 dlerror());
        return ERROR_CODE(APP_CALL_FUNC_FAILED);
    }

    PGetCompVersionFunc funGetCompVersion =
        (PGetCompVersionFunc)dlsym(handle, "GetCompVersion");
    if (funGetCompVersion == NULL) {
        RAISE_ERROR(APP_CALL_FUNC_FAILED);
        LOG_INFO("The function [GetCompVersion] called failed in %s lib. [%s]",
            dllName.c_str(),
                 dlerror());
        return ERROR_CODE(APP_CALL_FUNC_FAILED);
    }
#endif
    // 3.1 通过Create函数获得组件对象
    assert(component);
    *component = (funCreate)();
    if (*component == NULL) {
        RAISE_ERROR(APP_CREATE_COMP_FAILED);
        LOG_INFO("the object that is returned by the Create func in %s lib.",
            	dllName.c_str());
        return ERROR_CODE(APP_CREATE_COMP_FAILED);
    }

    // 3.2 通过GetCompVersion函数获得版本信息
    VersionInfo versionInfo;
    int retValue = (funGetCompVersion)(&versionInfo);
    if (retValue != UTILITY::SUCCESS) {
        RAISE_ERROR(APP_GET_VERSION_FAILED);
        LOG_INFO("the return value that is returned "
                 "by the function [GetCompVersion] in the %s library.",
                 dllName.c_str());
        return ERROR_CODE(APP_GET_VERSION_FAILED);
    }

    // tbd 版本信息如何验证
    //if (component == NULL && ) {
    //}

    return UTILITY::SUCCESS;
}

// 添加新的组件对象到组件列表(multimap)中
int CompMgr::AddCompObject(Component* component)
{
    // 1. 从组件对象中获取组件类型
    std::string type = "";
    std::string subType = "";
    if (component->GetType(type, subType) != UTILITY::SUCCESS) {
        RAISE_ERROR(APP_COMP_TYPE_FAILED);
        return ERROR_CODE(APP_COMP_TYPE_FAILED);
    }
    LOG_INFO("Add the component [type: %s, subtype: %s] object "
             "to the component list.",
             type.c_str(),
             subType.c_str());

    // 2. 将组件对象放入CompMap中
    LibType libType(type, subType);
    m_compMap.insert(std::make_pair(libType, component));
    // LOG_INFO("Finish to add the component [type: %s, subtype: %s] object.",
    //          type.c_str(),
    //          subType.c_str());

    return UTILITY::SUCCESS;
}

int CompMgr::Load(const std::string& configName)
{
    int retValue;

    // 1. 判断存入参数的合法性
    if (configName == "") {
        RAISE_ERROR(APP_NULL_CONFIG_NAME);
        return ERROR_CODE(APP_NULL_CONFIG_NAME);
    }
    // 2. 加载配置文件
    ConfigFile configFile;
    if (!configFile.Load((char *)configName.c_str())) {
        RAISE_ERROR(APP_LOAD_CONFIG_FAILED);
        LOG_INFO("Load config file %s failed.", configName.c_str());
        return ERROR_CODE(APP_LOAD_CONFIG_FAILED);
    }
    LOG_INFO("Load config file [%s] succeeded.", configName.c_str());

    // 3. 通过AddCompType的组件类型读取配置文件对应的小节，并加载指定的组件
    ConfigItemArray itemArray;
    for (ConfigItemArray::iterator iter = m_sectionArray.begin();
         iter != m_sectionArray.end(); iter++) {
        // 3.1. 读取配置文件中指定项的值
        itemArray.clear();
        retValue = ReadConfigInfo(*iter, configFile, itemArray);
        if (retValue != UTILITY::SUCCESS) {
            RAISE_ERROR(APP_READ_CONFIG_INFO_FAILED);
            LOG_INFO("Read the config item in the [%s] section.",
                (*iter).c_str());
            return ERROR_CODE(APP_READ_CONFIG_INFO_FAILED);
        }
        LOG_INFO("Fetch the section [%s] in the configure file [%s].",
            (*iter).c_str(),
            configName.c_str());

        // 3.2. 加载组件，并获得组件对象
        Component *pComponent = NULL;
        retValue = LoadComp(itemArray[0], &pComponent);
        if (retValue != UTILITY::SUCCESS) {
            RAISE_ERROR(APP_READ_CONFIG_INFO_FAILED);
            LOG_INFO("Load comp %s failed.", itemArray[0].c_str());
            return ERROR_CODE(APP_READ_CONFIG_INFO_FAILED);
        }
        LOG_INFO("Succeed to load the component [%s] in the section [%s] "
            "of the configure file [%s].",
            itemArray[0].c_str(),
            (*iter).c_str(),
            configName.c_str());

        // 4. 添加组件对象到组件列表中
        retValue = AddCompObject(pComponent);
        if (retValue != UTILITY::SUCCESS) {
            LOG_INFO("Add the component [%s] to the component list failed.",
                itemArray[0].c_str());
            RAISE_ERROR(APP_ADD_COMP_OBJ_FAILED);
            return ERROR_CODE(APP_ADD_COMP_OBJ_FAILED);
        }
        LOG_INFO("Succeed to add the component [%s] to the component list.",
            itemArray[0].c_str());
    }

    return UTILITY::SUCCESS;
}

int CompMgr::UnLoad()
{
    // 1. 释放CompMap中的组件对象
    // multimap中存放的是指针对象，clear不负责释放指针
    // if (!m_compMap.empty()) {
    //     m_compMap.clear();
    // }
    LOG_INFO("Release the component object pointer and "
             "clear the component object container.");
    CompMap::iterator iterCM;
    for (iterCM = m_compMap.begin(); iterCM != m_compMap.end(); ) {
        if (iterCM->second != NULL) {
            delete iterCM->second;
            iterCM->second = NULL;
            m_compMap.erase(iterCM++);
        }
    }

    // 2. 释放加载组件时所用到的handle
    LOG_INFO("Release the handler (pointer) of dynamic loading function "
             "in the dynamic library.");
    HandleArray::iterator iterHA;
    for (iterHA = m_handleArray.begin(); iterHA != m_handleArray.end();
         iterHA++) {
        if ((*iterHA) != NULL) {
#if WIN32
            FreeLibrary(*iterHA);
#else
            dlclose(*iterHA);
#endif
        }
    }

    // 清空所加载的type，也就是section名
    LOG_INFO("Clear the list that you added the [type] in the step "
             "you called [AddCompType] function.");
    if (!m_sectionArray.empty()) {
        m_sectionArray.clear();
    }

    return UTILITY::SUCCESS;
}

Component* CompMgr::GetCompObject(const std::string& type,
                                  const std::string& subType)
{
    if (type.empty() || subType.empty()) {
		LOG_ERROR(0,"%s",ERROR_MSG(APP_COMP_TYPE_NULL));
        return NULL;
    }
    // tbd 后期应重构该函数。通过何种模式加载，如何加载（加载算法）

    // 1. 根据type和subType确认CompMap中组件对象的个数（是否存在）
    CompMap::iterator iter;
    LibType searchKey(type, subType);
    int nSearchResult = m_compMap.count(searchKey);
    if (nSearchResult <= 0) {
        LOG_ERROR(0,"%s",ERROR_MSG(APP_COMP_NOT_EXIST));
        LOG_INFO("Can't find the specified component [type: %s, subtype: %s] "
                 "in the component list.",
                 type.c_str(),
                 subType.c_str());
        return NULL;
    }
    // 上面已判断过，存在该组件，此处不用再做判断
    iter = m_compMap.find(searchKey);
    // LOG_INFO("The component [type: %s, subtype: %s] is in the component list.",
    //         type.c_str(),
    //         subType.c_str());

    // 2. 根据对该组件的使用方式和组件的引用计数，选择返回的对象
    // tbd 如果是MULTIPLE使用方式，则如何选定组件对象
    for (int i = 0; i < nSearchResult; i++) {
        if (iter->second->GetUsedMode() == Component::SINGLE &&
            iter->second->GetRefCount() == 0) {
            // LOG_INFO("Get the component [type: %s, subtype: %s] in the SINGLE mode.",
            // type.c_str(),
            // subType.c_str());
            return iter->second;
        } else if (iter->second->GetUsedMode() == Component::MULTIPLE
                   && iter->second->GetRefCount() >= 0) {
            LOG_INFO("Get the component [type: %s, subtype: %s] "
                     "in the MULTIPLE mode.",
                     type.c_str(),
                     subType.c_str());
            return iter->second;
        }
        //iter++;
    }

   LOG_ERROR(0,"%s",ERROR_MSG(APP_COMP_NOT_EXIST));
    return NULL;
}
