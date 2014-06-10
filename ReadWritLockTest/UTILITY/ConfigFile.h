/********************************************************
	��ȡ�����ļ���
	�ļ���: profile.h

�����ļ������淶Ϊ��XXX.ini
�����ļ����ݸ�ʽΪ:
[SectionName]
EntryName=EntryValue

���У�
	1.section������Ϊ��
	2.entryName�����ظ�

change history:
2012-10-11 keizhao 
�޸Ĳ���Ϊstring����
�޸�pragma once
�޸��ඨ��
����ע��

2013-7-31  zhanyi  ���Ӻ���Ĭ��ֵ(ReadInt,ReadBool,ReadString)
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
	���캯��
	*/
	ConfigFile();
	/*
	��������
	*/
	virtual ~ConfigFile();

	/*
	*�������ļ�
	�������: string fileName,�ļ���
	�������: ���ļ��ɹ���񣬲����ļ���������顣
	*/
	bool Load(const std::string &filename);
	/*
	* ������: ReadTString
	* ����  : �����Ķ�ȡ�����ֶε��ַ������ҹ������ҿո��'\t'����������ڸ�ֵ���򷵻ش����Ĭ��ֵdefaultString��
	*		  ���sectionName����entryName�������򷵻ص�һ��ֵ
			  ���⣬��������ִ�Сд
	* ����  entreyValue: string����������ڸ�ֵ���򷵻ؿ��ַ���""��
	* ����  :string sectionName
	* ����  :string entryName
	* ����  :string defaultString, Ĭ��ֵ
	*/
	std::string ReadString(std::string sectionName,std::string entryName,std::string defaultString = "");

	/*
	* ������: ReadInt
	* ����  : �����Ķ�ȡ�����ֶε��ַ������ҹ������ҿո��'\t'����������ڸ�ֵ���򷵻ش����Ĭ��ֵdefaultInt
	*		  ���sectionName����entryName�������򷵻ص�һ��ֵ
			  ���⣬��������ִ�Сд
	* ����  entreyValue: string����������ڸ�ֵ���򷵻ؿ��ַ���""��
	* ����  :string sectionName
	* ����  :string entryName
	* ����  :string defaultInt, Ĭ��ֵ
	*/
	int ReadInt(std::string sectionName,std::string entryName, int defaultInt = 0);

	/*
	* ������: ReadBool
	* ����  : �����Ķ�ȡ�����ֶε��ַ������ҹ������ҿո��'\t'����������ڸ�ֵ���򷵻ش����Ĭ��ֵdefaultBool
	*		  ���sectionName����entryName�������򷵻ص�һ��ֵ
			  ע�⣬���ڼ����ԵĹ�ϵ��true����Y������true,false����N����false.
	* ����  entreyValue: string����������ڸ�ֵ���򷵻ؿ��ַ���""��
	* ����  :string sectionName
	* ����  :string entryName
	* ����  :string defaultBool, Ĭ��ֵ
	*/
	bool ReadBool(std::string sectionName,std::string entryName, bool defaultBool = false);

	
	/*
	* ������: LoadConfig
	* ����  : ���������ļ�������Image��so���·��
			  
	* ����   map<string,string>* ��
	* ����  :const string& filename
	*/
	LoadLibMap* LoadConfig(const std::string& filename);

private:
	//�����ļ�����
	std::string m_fileName;
	//���������
   FILE *fin,*fout;
   LoadLibMap m_map;

   //Decrypted File's full path
   char m_acDecryptedFile[256];

//˽�з�������
private:
	//����ָ��section���Ƶ��ַ���ָ��
	char *titlePos( char *buf, int *len );
	//�Ƿ���һ��section�ַ���
	bool isTitleLine( char *bufPtr );
	//�Ƿ���ָ����section
	bool containTitle( char *buf, const char *section );
	//�ƶ��ļ�ָ�뵽��Ӧ��sectionλ��
	bool gotoSection(const char *section );
	//����ָ��entry���Ƶ��ַ���ָ��
	char *textPos( char *buf, const char *entry );
	//ȥ���ַ���ǰ��Ŀո�
	void stripQuotationChar( char *buf );
	//��ȡһ��entry��ֵ
	int _ReadEntry( const char *entry, char *buf, int bufSize, bool strip);
	std::string _ReadString( std::string section,std::string entry,std::string defaultString);
	std::string _LRTrim(std::string str);
	//дһ��entry
	//�ر������ļ�
	void _Close();
	char * FGetS(char *pbuf,int size,FILE *fp);
	void out_close();
	void in_close();
	char * percolate(char *str);
    //�����ļ���ÿһ�����ݣ�������map��key��value
	bool ProcessLine(const std::string& line,std::string& key,std::string& value);
};
