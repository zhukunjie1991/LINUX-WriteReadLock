/********************************************************
	读取配置文件类实现
	文件名: profile.cpp

修改记录：
 编号    修改人   修改时间      修改说明
 01      占翼     2013/7/31     增加环境变量的支持。修改 
                                _ReadString函数返回值部分
**********************************************************/
#include "ConfigFile.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "PubFunc.h"

#ifndef WIN32
#include "EncryptDecryptFunc.h"
#endif

static std::string _expand_vars(std::string &str);
static int _find_first_var(std::string &str, int &len, int start = 0);

ConfigFile::ConfigFile()
{
/*
	功能：关闭输入输出流
	参数：无
	返回值：无
*/
   fin = NULL;
   fout = NULL;
   memset(m_acDecryptedFile, 0, sizeof(m_acDecryptedFile));
}

ConfigFile::~ConfigFile()
{
/*
	功能：关闭输入输出流
	参数：无
	返回值：无
*/
   _Close();

#ifndef WIN32
   //delete decrypt file
   if(m_acDecryptedFile[0] != '\0')
   {
	   remove(m_acDecryptedFile);
   }
#endif
}

void ConfigFile::_Close()
{
/*
	功能：关闭输入输出流
	参数：无
	返回值：无
*/
   in_close();
   out_close();
}

bool ConfigFile::Load(const std::string &filename)
{
	if (filename.empty())
    {
        return false;
    }
    in_close();
    fin=fopen(filename.c_str(),"rt");

#ifndef WIN32
	if (fin == NULL)
	{			
		//add some logic for encryption file
		//if file not exit
		//add .enc to filename;
		//try to open the encrypted file again.
		//if still not exit,return false;
		//else decrypt the file to a new file named fileName.tmp,add tempfileName=ileName.tmp
		//open the temp file to fin.
		//if still not exit,return false;

		std::string strEncryptedFile;
		char        acDecryptedFile[256];
		bool        status=false;
		char        acBaseName[256];
		char        acTmpBuf[256];
		char        *szDstCur = acTmpBuf;
		const char  *szSrcCur = filename.c_str() + filename.length() - 1;

		memset(acTmpBuf, 0, sizeof(acTmpBuf));		
		while(szSrcCur >= filename.c_str())
		{			
			if('\\' == *szSrcCur || '/' == *szSrcCur) break;
			if(szSrcCur == filename.c_str())
			{
				*szDstCur = *szSrcCur;
				break;
			}
			*szDstCur++ = *szSrcCur--;
		}
		
		if(strlen(acTmpBuf) > 0)
		{
			szSrcCur = acTmpBuf + strlen(acTmpBuf) - 1;
			szDstCur = acBaseName;
			while(szSrcCur >= acTmpBuf) *szDstCur++ = *szSrcCur--;
			*szDstCur = '\0';
		}
#ifdef WIN32
		UTILITY::Snprintf(acDecryptedFile, sizeof(acDecryptedFile), "%s.Dec", acBaseName);
#else
		UTILITY::Snprintf(acDecryptedFile, sizeof(acDecryptedFile), "/tmp/%s.Dec", acBaseName);
#endif
		strEncryptedFile = filename;
		strEncryptedFile.append(".enc");
		fin = fopen(strEncryptedFile.c_str(), "rt");
		if (NULL == fin)
		{
			printf("File[%s] Not Found\n", strEncryptedFile.c_str());
			return false;
		}

		status = EncryptDecryptFile((char*)strEncryptedFile.c_str(), acDecryptedFile, "CFETS", INDC_ONLY_DECRYPT);
		if (status != true)
		{
			printf("Error in Decrypting File[%s]\n",strEncryptedFile.c_str());
			fclose(fin);
			return false;
		}
		fclose(fin);
		fin = NULL;

		UTILITY::Snprintf(m_acDecryptedFile, sizeof(m_acDecryptedFile), "%s", acDecryptedFile);
		fin = fopen(acDecryptedFile, "rt");
		if (NULL == fin)
		{
			printf("Decrypted File[%s] Not Found\n", acDecryptedFile);
			return false;
		}
	}
#else
	if(NULL == fin)
	{
		printf("Config file[%s] not found\n", filename);
		return false;
	}
#endif
	return true;
}

//此函数用于判断是不是[session]行。如果是段落行返回段落
//名称的起始位置，并且计算段落名的长度
char *ConfigFile::titlePos( char *buf, int *len )
{
/*
	功能：取得指向section的开始的指针，及在buf中的偏移量
	参数：
		buf[IN]：源串
		len[OUT]：偏移量
	返回值：
		是一个section则返回指向该section起始的指针
		否则返回0
*/
	char *p = buf, *q;

	while( *p && isspace(*p) ) p++;
	if( *p != '[' )
		return 0;

	q = p+1;
	while( *q && *q != ']' ) q++;
	if( *q != ']' )
		return 0;
	if( len )
		*len = int(q - p - 1);
	return p+1;
}

bool ConfigFile::isTitleLine( char *bufPtr )
{
/*
	功能：字符串中是否是一个section
	参数：
		bufPtr[IN]：源串
	返回值：
		成功返回true
		失败返回false
*/
	return titlePos( bufPtr, 0 ) != 0;
}

bool ConfigFile::containTitle( char *buf, const char *section )
{
/*
	功能：字符串中是否包含指定的section
	参数：
		buf[IN]：源串
		section[IN]：要查找的section
	返回值：
		找到返回true
		否则返回false
*/
	char *p;
	int len;

	p = titlePos( buf, &len );
	if( p )
	{
		//hauk -- 2013-10-29 modified
	    //if( (int)strlen( section ) == len && strncmp( section, p, len ) == 0 )
		   //return true;
		if((int)strlen(section) != len) return false;
		char acDst[256], acSrc[256];
		memset(acDst, 0, sizeof(acDst));
		memset(acSrc, 0, sizeof(acSrc));
		memcpy(acDst, section, len);
		memcpy(acSrc, p, len);
#ifdef WIN32
        if(0 == stricmp(acDst, acSrc)) return true;
#else
		if(0 == strcasecmp(acDst, acSrc)) return true;
#endif
		//end of modification
	}
	return false;
}

bool ConfigFile::gotoSection(const char *section )
{
/*
	功能：移动文件指针到指定section的开始处
	参数：
		section[IN]：要查找的section
	返回值：
		找到并定位返回true
		否则返回false
*/
	char line[1024];
   fseek(fin,0,SEEK_SET);

	while( FGetS(line,1024,fin)!=NULL)
   {
	   if( containTitle( line, section ) )
		   return true;
   }
	return false;
}

//如果找到对应的属性配置行，返回该属性的值(字符串形式)
//否则返回空
char *ConfigFile::textPos( char *buf, const char *entry )
{
/*
	功能：移动指针到相应的entry处
	参数：
		buf[IN]：源串
		entry[IN]：要查找的entry
	返回值：
		找到并定位返回指向该entry起始的指针
		以';'注释或者未找到返回0
*/
   char tmp[1024];
   memset(tmp,0,1024);
   int i;
   //modified by hauk at 2013-10-29
   //if( buf[0] == ';' ) // it fis comment line
   if('#' == buf[0] || ';' == buf[0])
		return 0;
   //end of modification

   for (i=0;buf[i];i++)
   {
      if (buf[i]=='=')
         break;
      tmp[i]=buf[i];
   }
   if (buf[i]!='=')
      return(0);

   tmp[i]=0;

   TrimBlank(tmp);

   //modified by hauk at 2013-10-29
   //if( strcmp( tmp, entry) == 0 )
		//return buf+i+1;   
#ifdef WIN32
   if(0 == stricmp(tmp, entry)) return buf+i+1;
#else
   if(0 == strcasecmp(tmp, entry)) return buf+i+1;
#endif
   //end of modification

   return 0;
}

void ConfigFile::stripQuotationChar( char *buf )
{
/*
	功能：去除字符串前的空格
	参数：
		buf[IN]：源串
	返回值：无
*/
	char *p;
	char *q;

	p = buf;
	while( *p && isspace(*p) ) p++;

	if( !(*p == '\"' || *p == '\'') )
		return;

	q = p+strlen(p);
	while( *q != *p && q > p ) q--;
	if( q == p )
		return;
	int len = int(q - p - 1);
	memmove( buf, p+1, len );
	buf[len] = 0;
}

int ConfigFile::_ReadEntry(const char *entry, char *buf, int bufSize,bool strip )
{
/*
  功能：读取entry的值
  参数：
  entry[IN]：entry名称
  buf[IN]：存放entry的值的缓冲区
  bufSize[IN]：缓冲区长度
  strip[IN]：去除空格标志
  返回值：
  成功返回读取的长度
  失败返回-1
*/
    char lineBuf[1024];
    //memset(lineBuf,0,1024);
    char *p, *cur;
    int  len;

    cur  = buf;
    *cur = '\0';
    len  = -1;
    while(FGetS(lineBuf,1023,fin))
    {
        if( isTitleLine( lineBuf ) )       // section fis ended
            break;

        p = textPos( lineBuf, entry );     // not equal this entry
        if( p == 0 )
            continue;

        if( strip )
            stripQuotationChar( p );

        len = strlen(p);
        if( bufSize-1 < len )
            len = bufSize-1;

        strncpy( cur, p, len );
        cur[len] = 0;
        break;
    }

    return len;
}

std::string ConfigFile::_ReadString(std::string section,std::string entry,std::string defaultString)
{
/*
  功能：读取指定的section的指定entry的值到buffer中，未找到则使用缺省配置
  参数：
  section[IN]：指定的section
  entry[IN]：指定的entry
  defaultstring[IN]：缺省配置
  buffer[IN]：存放entry值的缓冲区
  bufLen[IN]：缓冲区长度
  返回值：
  成功返回buffer中的值的长度
  失败返回-1
*/

    int len = -1;

    fseek(fin,0,SEEK_SET);

    char buffer[1024];
    memset(buffer,0,1024);
    int bufLen = sizeof(buffer) -1;
    if( gotoSection( section.c_str() ) )
    {
        len = _ReadEntry(entry.c_str(), buffer, bufLen, true);
    }


	//后面的空格也要去。前面的空格等于没去.
    //hauk 2013-10-29 modified
    //percolate(buffer);
	char *szTmp = percolate(buffer);
	if(szTmp && strlen(szTmp) != strlen(buffer)) UTILITY::Snprintf(buffer, sizeof(buffer), "%s", szTmp);
	//end of modification
    
	len = strlen(buffer);

    if( len <= 0 ) //can not read entry, use default string
    {
        strncpy( buffer, defaultString.c_str(), bufLen-1 );
        buffer[bufLen-1] = 0;
        len = strlen(buffer);
    }

/**
 * 支持配置文件值中环境变量扩展
 * 环境变量格式 ${环境变量}
 */
    string str(buffer);
    return _expand_vars(str);
//    return buffer;
}

int ConfigFile::ReadInt(std::string section,std::string entry,int defaultInt)
{
/*
  功能：读取指定的section的指定entry的值以整数形式返回，未找到则使用缺省配置
  参数：
  section[IN]：指定的section
  entry[IN]：指定的entry
  defaultInt[IN]：缺省配置
  返回值：
  没有找到指定entry则返回缺省值
  否则返回相应的配置值
*/

    fseek(fin,0,SEEK_SET);
    if( gotoSection( section.c_str() ) )
    {
        string tmp = ReadString( section.c_str(), entry.c_str() , "-");
        if (tmp != "-")
            defaultInt = atoi(tmp.c_str());
    }
    return defaultInt;
}


char * ConfigFile::percolate(char *str)
{
    int i,len;
    char *pc;
    len=strlen(str);
	//去掉ASCII码小于20的字符
    for(i=len-1;i>=0;i--)
    {
        if(str[i]<20)
            str[i]=0;
        else
            break;
    }
	//去掉后面的注释
    for (i=0;str[i]!=0;i++)
    {
        if (str[i]=='/' && str[i+1]=='/')
        {
            str[i]=0;
            break;
        }
    }
	//去掉头部的空格和制表符
    for (pc=str;*pc!=0;pc++)
    {
        if (*pc!=' '&&*pc!='\t')
            break;
    }
    return pc;
}

void ConfigFile::in_close()
{
   if (fin!=NULL)
   {
      fclose(fin);
      fin = NULL;
   }
}

void ConfigFile::out_close()
{
   if (fout!=NULL)
   {
      fclose(fout);
      fout = NULL;
   }
}

char * ConfigFile::FGetS(char *pbuf, int size, FILE *fp)
{
   char tmp[1024];
   memset(tmp,0,1024);
   char *pc;
   if (fgets(tmp,1023,fp)==NULL)
      return(NULL);
   pc = percolate(tmp);
   if (pc==NULL)
   {
      pbuf[0]=0;
   }
   else
   {
      strncpy(pbuf,pc,size);
   }
   return(pbuf);
}


// 函数名: TIniFile::LRTrim
// 描述  : 从str中将左右空格和'\t'截取掉
// 返回  : char *
// 参数  : char *str
std::string ConfigFile::_LRTrim(std::string str)
{
    // mdf by Wujinghe 2012-12-21 对形参添加错误处理
    if (str == "")
        return str;
    // end mdf 2012-12-21
    int startBlank = str.find_first_not_of(' ');
    int startTab = str.find_first_not_of(' ');
    int start = max(startBlank,startTab);

    int endBlank = str.find_last_not_of(' ');
    int endTab = str.find_last_not_of('\t');
    int end = min(endBlank,endTab);

	//modified by hauk at 2013-10-29
    //return str.substr(start,end-start+1);
	if(end >= start) return str.substr(start,end-start+1);
	return "";
	//end of modification 
}


// 函数名: TIniFile::ReadTString
// 描述  : 新增的读取定义字段的字符串，且过滤左右空格和'\t'
// 返回  : char *
// 参数  : const char *section
// 参数  : const char *entry
// 参数  : const char *defaultString
// 参数  : char *buffer
// 参数  : int bufLen
std::string ConfigFile::ReadString(std::string section, std::string entry, string defaultString )
{
   return _LRTrim(_ReadString(section,entry,defaultString));
}

bool ConfigFile::ReadBool( std::string sectionName,std::string entryName, bool defaultBool )
{
	std::string boolValue = string_toLower(_LRTrim(_ReadString(sectionName,entryName,"-")));
	//没有适当的配置项
	if(boolValue == "-")
		return defaultBool;

	//返回 true
	if(boolValue == "true" || boolValue == "y" || boolValue == "yes")
		return true;

	//返回false
	if(boolValue == "false" || boolValue == "n" || boolValue == "no")
		return false;

	return defaultBool;

}


bool ConfigFile::ProcessLine(const std::string& line,std::string& key,std::string& value)
{
	if(line.empty())
		return false;

	int start_pos=0,end_pos=line.size()-1,pos;
	std::string new_line=line;
	//等号是key和value的分隔符
	if((pos=new_line.find('='))==-1)
		return false;
    //把=前的字符串截取为map的key
	key=new_line.substr(0,pos);
    //把=后的字符串截取为map的value
	value=new_line.substr(pos+1,end_pos+1-(pos+1));

	//去空格
	key = _LRTrim(key);
	value = _LRTrim(value);

	//key和value不能有任何一个为空
	if(key.empty()||value.empty())
	{

		return false;
	}

	return true;

}


LoadLibMap* ConfigFile::LoadConfig(const std::string& filename)
{
	m_map.clear();
	//打开文件
	ifstream infile(filename.c_str());
	if(!infile)
	{
		//std::cout<<"file open err"<<std::endl;
		return false;

	}
	std::string line,key,value;
	while(getline(infile,line))
	{
		//读取文件的每行解析成map的key和value
		if(ProcessLine(line,key,value))
		{
			m_map[key]=value;
		}
	}
	infile.close();
	return &m_map;
}

std::string _expand_vars(std::string &str)
{
	string retstr = "";
	string lstr = "";
	string var;

#ifdef _DEBUG_
//	printf("Expand string %s start...\n", str.c_str());
#endif
	char *value;
	int index, len, start;
	start = 0;
	index = 0;
	while ((index = _find_first_var(str, len, index)) != -1)
	{
		retstr.append(str.substr(start, index - start));
		var = str.substr(index + 2, len);
		value = getenv(var.c_str());
		if (value != NULL)
			retstr.append(value);
		else
			retstr.append("");
		start = index + 3 + len;
		index = start;
	}
	// 无环境变量
	if (start == 0 && !str.empty())
	{
		return str;
	}
	else
	{
		// 把剩下的部分拼上
		if (start < str.length())
		{
			retstr.append(str.substr(start));
		}
	}
#ifdef _DEBUG_
//	printf("Expand string %s to value %s\n", str.c_str(), retstr.c_str());
#endif
	return retstr;
}

#define VAR_PREFIX "${"
#define VAR_SUFFIX "}"

int _find_first_var(std::string &str, int &len, int start)
{
	std::string::size_type index, vend;
	if ((index = str.find(VAR_PREFIX, start)) != std::string::npos)
	{
		if ((vend = str.find(VAR_SUFFIX, index + 2)) != std::string::npos)
		{
			len = vend - index - 2;
#ifdef _DEBUG_
//			printf("_find_first_var: %s,%d. index=%d, len=%d\n", str.c_str(), start, index, len);
#endif
			return index;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}
