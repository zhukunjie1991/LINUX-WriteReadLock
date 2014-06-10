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
    // ���type�Ѵ��ڣ��������
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

// ��ȡ�����ļ��ж�Ӧ���ֵ������ֵ���浽�����з���
int CompMgr::ReadConfigInfo(const std::string& type,
                            ConfigFile& configFile,
                            ConfigItemArray& configItemArray)
{
    // 1. �������ļ��У���ȡ���������⣩��������·�����ļ�����
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
    // ����ȡ�������������
    configItemArray.push_back(dllName);

    // 2. �������ļ��У���ȡ�������������
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
    // ����ȡ�������������
    configItemArray.push_back(subType);

	return UTILITY::SUCCESS;
}

// ����ָ����·�����������(.dll or .so)
typedef Component* (*PCreateFunc)(void);
typedef int (*PGetCompVersionFunc)(VersionInfo*);

int CompMgr::LoadComp(const std::string& dllName, Component** component)
{
    // 1. ��������ļ�(.dll or .so)������handle��ӵ�HandleArray��(���ͷ�handle)
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

    // 2. ȡ�������Create������GetCompVersion����
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
    // 3.1 ͨ��Create��������������
    assert(component);
    *component = (funCreate)();
    if (*component == NULL) {
        RAISE_ERROR(APP_CREATE_COMP_FAILED);
        LOG_INFO("the object that is returned by the Create func in %s lib.",
            	dllName.c_str());
        return ERROR_CODE(APP_CREATE_COMP_FAILED);
    }

    // 3.2 ͨ��GetCompVersion������ð汾��Ϣ
    VersionInfo versionInfo;
    int retValue = (funGetCompVersion)(&versionInfo);
    if (retValue != UTILITY::SUCCESS) {
        RAISE_ERROR(APP_GET_VERSION_FAILED);
        LOG_INFO("the return value that is returned "
                 "by the function [GetCompVersion] in the %s library.",
                 dllName.c_str());
        return ERROR_CODE(APP_GET_VERSION_FAILED);
    }

    // tbd �汾��Ϣ�����֤
    //if (component == NULL && ) {
    //}

    return UTILITY::SUCCESS;
}

// ����µ������������б�(multimap)��
int CompMgr::AddCompObject(Component* component)
{
    // 1. ����������л�ȡ�������
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

    // 2. ������������CompMap��
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

    // 1. �жϴ�������ĺϷ���
    if (configName == "") {
        RAISE_ERROR(APP_NULL_CONFIG_NAME);
        return ERROR_CODE(APP_NULL_CONFIG_NAME);
    }
    // 2. ���������ļ�
    ConfigFile configFile;
    if (!configFile.Load((char *)configName.c_str())) {
        RAISE_ERROR(APP_LOAD_CONFIG_FAILED);
        LOG_INFO("Load config file %s failed.", configName.c_str());
        return ERROR_CODE(APP_LOAD_CONFIG_FAILED);
    }
    LOG_INFO("Load config file [%s] succeeded.", configName.c_str());

    // 3. ͨ��AddCompType��������Ͷ�ȡ�����ļ���Ӧ��С�ڣ�������ָ�������
    ConfigItemArray itemArray;
    for (ConfigItemArray::iterator iter = m_sectionArray.begin();
         iter != m_sectionArray.end(); iter++) {
        // 3.1. ��ȡ�����ļ���ָ�����ֵ
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

        // 3.2. ���������������������
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

        // 4. ��������������б���
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
    // 1. �ͷ�CompMap�е��������
    // multimap�д�ŵ���ָ�����clear�������ͷ�ָ��
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

    // 2. �ͷż������ʱ���õ���handle
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

    // ��������ص�type��Ҳ����section��
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
    // tbd ����Ӧ�ع��ú�����ͨ������ģʽ���أ���μ��أ������㷨��

    // 1. ����type��subTypeȷ��CompMap���������ĸ������Ƿ���ڣ�
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
    // �������жϹ������ڸ�������˴����������ж�
    iter = m_compMap.find(searchKey);
    // LOG_INFO("The component [type: %s, subtype: %s] is in the component list.",
    //         type.c_str(),
    //         subType.c_str());

    // 2. ���ݶԸ������ʹ�÷�ʽ����������ü�����ѡ�񷵻صĶ���
    // tbd �����MULTIPLEʹ�÷�ʽ�������ѡ���������
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
