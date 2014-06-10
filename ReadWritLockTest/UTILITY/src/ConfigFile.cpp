/********************************************************
	��ȡ�����ļ���ʵ��
	�ļ���: profile.cpp

�޸ļ�¼��
 ���    �޸���   �޸�ʱ��      �޸�˵��
 01      ռ��     2013/7/31     ���ӻ���������֧�֡��޸� 
                                _ReadString��������ֵ����
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
	���ܣ��ر����������
	��������
	����ֵ����
*/
   fin = NULL;
   fout = NULL;
   memset(m_acDecryptedFile, 0, sizeof(m_acDecryptedFile));
}

ConfigFile::~ConfigFile()
{
/*
	���ܣ��ر����������
	��������
	����ֵ����
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
	���ܣ��ر����������
	��������
	����ֵ����
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

//�˺��������ж��ǲ���[session]�С�����Ƕ����з��ض���
//���Ƶ���ʼλ�ã����Ҽ���������ĳ���
char *ConfigFile::titlePos( char *buf, int *len )
{
/*
	���ܣ�ȡ��ָ��section�Ŀ�ʼ��ָ�룬����buf�е�ƫ����
	������
		buf[IN]��Դ��
		len[OUT]��ƫ����
	����ֵ��
		��һ��section�򷵻�ָ���section��ʼ��ָ��
		���򷵻�0
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
	���ܣ��ַ������Ƿ���һ��section
	������
		bufPtr[IN]��Դ��
	����ֵ��
		�ɹ�����true
		ʧ�ܷ���false
*/
	return titlePos( bufPtr, 0 ) != 0;
}

bool ConfigFile::containTitle( char *buf, const char *section )
{
/*
	���ܣ��ַ������Ƿ����ָ����section
	������
		buf[IN]��Դ��
		section[IN]��Ҫ���ҵ�section
	����ֵ��
		�ҵ�����true
		���򷵻�false
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
	���ܣ��ƶ��ļ�ָ�뵽ָ��section�Ŀ�ʼ��
	������
		section[IN]��Ҫ���ҵ�section
	����ֵ��
		�ҵ�����λ����true
		���򷵻�false
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

//����ҵ���Ӧ�����������У����ظ����Ե�ֵ(�ַ�����ʽ)
//���򷵻ؿ�
char *ConfigFile::textPos( char *buf, const char *entry )
{
/*
	���ܣ��ƶ�ָ�뵽��Ӧ��entry��
	������
		buf[IN]��Դ��
		entry[IN]��Ҫ���ҵ�entry
	����ֵ��
		�ҵ�����λ����ָ���entry��ʼ��ָ��
		��';'ע�ͻ���δ�ҵ�����0
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
	���ܣ�ȥ���ַ���ǰ�Ŀո�
	������
		buf[IN]��Դ��
	����ֵ����
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
  ���ܣ���ȡentry��ֵ
  ������
  entry[IN]��entry����
  buf[IN]�����entry��ֵ�Ļ�����
  bufSize[IN]������������
  strip[IN]��ȥ���ո��־
  ����ֵ��
  �ɹ����ض�ȡ�ĳ���
  ʧ�ܷ���-1
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
  ���ܣ���ȡָ����section��ָ��entry��ֵ��buffer�У�δ�ҵ���ʹ��ȱʡ����
  ������
  section[IN]��ָ����section
  entry[IN]��ָ����entry
  defaultstring[IN]��ȱʡ����
  buffer[IN]�����entryֵ�Ļ�����
  bufLen[IN]������������
  ����ֵ��
  �ɹ�����buffer�е�ֵ�ĳ���
  ʧ�ܷ���-1
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


	//����Ŀո�ҲҪȥ��ǰ��Ŀո����ûȥ.
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
 * ֧�������ļ�ֵ�л���������չ
 * ����������ʽ ${��������}
 */
    string str(buffer);
    return _expand_vars(str);
//    return buffer;
}

int ConfigFile::ReadInt(std::string section,std::string entry,int defaultInt)
{
/*
  ���ܣ���ȡָ����section��ָ��entry��ֵ��������ʽ���أ�δ�ҵ���ʹ��ȱʡ����
  ������
  section[IN]��ָ����section
  entry[IN]��ָ����entry
  defaultInt[IN]��ȱʡ����
  ����ֵ��
  û���ҵ�ָ��entry�򷵻�ȱʡֵ
  ���򷵻���Ӧ������ֵ
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
	//ȥ��ASCII��С��20���ַ�
    for(i=len-1;i>=0;i--)
    {
        if(str[i]<20)
            str[i]=0;
        else
            break;
    }
	//ȥ�������ע��
    for (i=0;str[i]!=0;i++)
    {
        if (str[i]=='/' && str[i+1]=='/')
        {
            str[i]=0;
            break;
        }
    }
	//ȥ��ͷ���Ŀո���Ʊ��
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


// ������: TIniFile::LRTrim
// ����  : ��str�н����ҿո��'\t'��ȡ��
// ����  : char *
// ����  : char *str
std::string ConfigFile::_LRTrim(std::string str)
{
    // mdf by Wujinghe 2012-12-21 ���β���Ӵ�����
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


// ������: TIniFile::ReadTString
// ����  : �����Ķ�ȡ�����ֶε��ַ������ҹ������ҿո��'\t'
// ����  : char *
// ����  : const char *section
// ����  : const char *entry
// ����  : const char *defaultString
// ����  : char *buffer
// ����  : int bufLen
std::string ConfigFile::ReadString(std::string section, std::string entry, string defaultString )
{
   return _LRTrim(_ReadString(section,entry,defaultString));
}

bool ConfigFile::ReadBool( std::string sectionName,std::string entryName, bool defaultBool )
{
	std::string boolValue = string_toLower(_LRTrim(_ReadString(sectionName,entryName,"-")));
	//û���ʵ���������
	if(boolValue == "-")
		return defaultBool;

	//���� true
	if(boolValue == "true" || boolValue == "y" || boolValue == "yes")
		return true;

	//����false
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
	//�Ⱥ���key��value�ķָ���
	if((pos=new_line.find('='))==-1)
		return false;
    //��=ǰ���ַ�����ȡΪmap��key
	key=new_line.substr(0,pos);
    //��=����ַ�����ȡΪmap��value
	value=new_line.substr(pos+1,end_pos+1-(pos+1));

	//ȥ�ո�
	key = _LRTrim(key);
	value = _LRTrim(value);

	//key��value�������κ�һ��Ϊ��
	if(key.empty()||value.empty())
	{

		return false;
	}

	return true;

}


LoadLibMap* ConfigFile::LoadConfig(const std::string& filename)
{
	m_map.clear();
	//���ļ�
	ifstream infile(filename.c_str());
	if(!infile)
	{
		//std::cout<<"file open err"<<std::endl;
		return false;

	}
	std::string line,key,value;
	while(getline(infile,line))
	{
		//��ȡ�ļ���ÿ�н�����map��key��value
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
	// �޻�������
	if (start == 0 && !str.empty())
	{
		return str;
	}
	else
	{
		// ��ʣ�µĲ���ƴ��
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
