/********************************************************
	读取配置文件类
	文件名: profile.h

配置文件命名规范为：XXX.ini
配置文件内容格式为:
[SectionName]
EntryName=EntryValue

其中：
	1.section名不能为空
	2.entryName不能重复

change history:
2012-10-11 keizhao 
修改参数为string类型
修改pragma once
修改类定义
增加注释

2013-7-31  zhanyi  增加函数默认值(ReadInt,ReadBool,ReadString)
**********************************************************/
#pragma once

#include <stdio.h>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
using namespace std;
typedef map<std::string,std::string> LoadLibMap;

class ConfigFile
{
public:
	/*
	构造函数
	*/
	ConfigFile();
	/*
	析构函数
	*/
	virtual ~ConfigFile();

	/*
	*打开配置文件
	输入参数: string fileName,文件名
	输出参数: 打开文件成功与否，不对文件内容做检查。
	*/
	bool Load(const std::string &filename);
	/*
	* 函数名: ReadTString
	* 描述  : 新增的读取定义字段的字符串，且过滤左右空格和'\t'，如果不存在该值。则返回传入的默认值defaultString。
	*		  如果sectionName或者entryName重名，则返回第一个值
			  另外，配置项不区分大小写
	* 返回  entreyValue: string，如果不存在该值。则返回空字符串""。
	* 参数  :string sectionName
	* 参数  :string entryName
	* 参数  :string defaultString, 默认值
	*/
	std::string ReadString(std::string sectionName,std::string entryName,std::string defaultString = "");

	/*
	* 函数名: ReadInt
	* 描述  : 新增的读取定义字段的字符串，且过滤左右空格和'\t'，如果不存在该值。则返回传入的默认值defaultInt
	*		  如果sectionName或者entryName重名，则返回第一个值
			  另外，配置项不区分大小写
	* 返回  entreyValue: string，如果不存在该值。则返回空字符串""。
	* 参数  :string sectionName
	* 参数  :string entryName
	* 参数  :string defaultInt, 默认值
	*/
	int ReadInt(std::string sectionName,std::string entryName, int defaultInt = 0);

	/*
	* 函数名: ReadBool
	* 描述  : 新增的读取定义字段的字符串，且过滤左右空格和'\t'，如果不存在该值。则返回传入的默认值defaultBool
	*		  如果sectionName或者entryName重名，则返回第一个值
			  注意，由于兼容性的关系，true或者Y都返回true,false或者N返回false.
	* 返回  entreyValue: string，如果不存在该值。则返回空字符串""。
	* 参数  :string sectionName
	* 参数  :string entryName
	* 参数  :string defaultBool, 默认值
	*/
	bool ReadBool(std::string sectionName,std::string entryName, bool defaultBool = false);

	
	/*
	* 函数名: LoadConfig
	* 描述  : 根据配置文件，加载Image的so库的路径
			  
	* 返回   map<string,string>* 。
	* 参数  :const string& filename
	*/
	LoadLibMap* LoadConfig(const std::string& filename);

private:
	//配置文件名称
	std::string m_fileName;
	//输入输出流
   FILE *fin,*fout;
   LoadLibMap m_map;

   //Decrypted File's full path
   char m_acDecryptedFile[256];

//私有方法定义
private:
	//返回指向section名称的字符串指针
	char *titlePos( char *buf, int *len );
	//是否是一个section字符串
	bool isTitleLine( char *bufPtr );
	//是否是指定的section
	bool containTitle( char *buf, const char *section );
	//移动文件指针到相应的section位置
	bool gotoSection(const char *section );
	//返回指向entry名称的字符串指针
	char *textPos( char *buf, const char *entry );
	//去除字符串前面的空格
	void stripQuotationChar( char *buf );
	//读取一个entry的值
	int _ReadEntry( const char *entry, char *buf, int bufSize, bool strip);
	std::string _ReadString( std::string section,std::string entry,std::string defaultString);
	std::string _LRTrim(std::string str);
	//写一个entry
	//关闭配置文件
	void _Close();
	char * FGetS(char *pbuf,int size,FILE *fp);
	void out_close();
	void in_close();
	char * percolate(char *str);
    //处理文件的每一行数据，解析成map的key和value
	bool ProcessLine(const std::string& line,std::string& key,std::string& value);
};
