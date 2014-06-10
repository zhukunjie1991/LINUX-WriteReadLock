#ifndef __COMPMGR_H__
#define __COMPMGR_H__

#include <string>
#include <map>
#include <vector>

#ifdef WIN32
#include <TCHAR.h>
#include <atlstr.h>
#else
#include <dlfcn.h>
#endif

class ConfigFile;
class Component;

// 一个组件由type和subtype唯一标识
struct LibType
{
    std::string m_type;
    std::string m_subType;

    LibType(std::string type, std::string subType)
    {
        m_type = type;
        m_subType = subType;
    }

    LibType(const LibType& libType)
    {
        m_type = libType.m_type;
        m_subType = libType.m_subType;
    }

    bool operator<(const LibType & t)const
    {
        if (m_type < t.m_type)
            return true;
        else if (m_type == t.m_type)
        {
            return t.m_subType < m_subType;
        }
        return false;
    }
};

#define LibPath "LibPath"
#define SubType "SubType"

class CompMgr {
    template <typename T> friend class Singleton;
#ifdef WIN32
    typedef std::vector<HMODULE> HandleArray;
#else
    typedef std::vector<void *> HandleArray;
#endif

    // typedef std::pair<std::string, std::string> LibType;
    typedef std::multimap<LibType, Component *> CompMap;
    typedef std::vector<std::string> ConfigItemArray;

    // singleton pattern
private:
    CompMgr(void);
    ~CompMgr(void);
    CompMgr(const CompMgr&){};
    CompMgr& operator=(const CompMgr&){return *this;};
    // static CompMgr *m_compMgr;

public:
    static CompMgr* GetInstance();

    // 添加主类型(type)。
    // 加载配置文件之前必须把要用的几个组件的主类型type先调用该方法Add
    int AddCompType(const std::string& type);
    // 通过配置文件将已add进去的type的组件加载进来
    int Load(const std::string& configName);
    // 卸载所有已添加进来的组件
    int UnLoad();
    // 通过主类型type和subtype获取到唯一的组件对象
    Component* GetCompObject(const std::string& type,
                             const std::string& subType);
    // 将已加载的组件对象添加到列表中
    int AddCompObject(Component* component);

private:
    // 读取配置文件中的项，并保存到array中
    int ReadConfigInfo(const std::string& type,
        ConfigFile& configFile,
        ConfigItemArray& configItem);
    // 通过组件的名字加载组件
    int LoadComp(const std::string& dllName, Component** component);

private:
#ifdef WIN32
    // 保存动态加载返回的对象指针，并在析构里删除
    HandleArray m_handleArray;
#else
    // 保存动态加载返回的对象指针，并在析构里删除
    HandleArray m_handleArray;
#endif
    // tbd 改为结构体应该比较合适
    // 保存配置文件中的项
    ConfigItemArray m_sectionArray;
    // 以组件的主类型、次类型为key，componet为Value
    CompMap m_compMap;
};

#endif //__COMPMGR_H__
