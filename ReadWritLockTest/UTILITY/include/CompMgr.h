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

// һ�������type��subtypeΨһ��ʶ
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

    // ���������(type)��
    // ���������ļ�֮ǰ�����Ҫ�õļ��������������type�ȵ��ø÷���Add
    int AddCompType(const std::string& type);
    // ͨ�������ļ�����add��ȥ��type��������ؽ���
    int Load(const std::string& configName);
    // ж����������ӽ��������
    int UnLoad();
    // ͨ��������type��subtype��ȡ��Ψһ���������
    Component* GetCompObject(const std::string& type,
                             const std::string& subType);
    // ���Ѽ��ص����������ӵ��б���
    int AddCompObject(Component* component);

private:
    // ��ȡ�����ļ��е�������浽array��
    int ReadConfigInfo(const std::string& type,
        ConfigFile& configFile,
        ConfigItemArray& configItem);
    // ͨ����������ּ������
    int LoadComp(const std::string& dllName, Component** component);

private:
#ifdef WIN32
    // ���涯̬���ط��صĶ���ָ�룬����������ɾ��
    HandleArray m_handleArray;
#else
    // ���涯̬���ط��صĶ���ָ�룬����������ɾ��
    HandleArray m_handleArray;
#endif
    // tbd ��Ϊ�ṹ��Ӧ�ñȽϺ���
    // ���������ļ��е���
    ConfigItemArray m_sectionArray;
    // ������������͡�������Ϊkey��componetΪValue
    CompMap m_compMap;
};

#endif //__COMPMGR_H__
