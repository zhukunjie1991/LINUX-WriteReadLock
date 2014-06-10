#include "PubFunc.h"

#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <sys/timeb.h>

#include <string>
#include <algorithm>
#include <fstream>

#ifdef WIN32
#include <direct.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define PATHMARK  '\\'
#define MAKEPATH(a) mkdir(a)
#else
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#ifndef _AIX_
#include <sys/syscall.h>
#endif
#include <dirent.h>
#include <pthread.h>
#define PATHMARK  '/'
#define MAKEPATH(a) mkdir(a,0777)
#endif

#ifdef _AIX_
#include <unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <procinfo.h>
#if (__IBMCPP__ < 1210)
extern "C" 
{ 
	int getprocs(struct procsinfo*, int , struct fdsinfo*, int, pid_t*, int); 
}
#endif
#endif

using namespace std;

void string_replace(const std::string& oldValue,
                    const std::string& newValue,
                    std::string& value)
{

    for (std::string::size_type pos = value.find(oldValue);
         pos != std::string::npos;
         pos = value.find(oldValue, pos))
    {
        value.replace(pos, oldValue.size(), newValue);
        pos += newValue.size();
    }
}

std::string string_toUpper(const std::string& value)
{
    std::string copy = value;
    std::transform(copy.begin(), copy.end(), copy.begin(),  (int(*)(int)) toupper);
    return copy;
}

std::string string_toLower(const std::string& value)
{
    std::string copy = value;
    std::transform(copy.begin(), copy.end(), copy.begin(),  (int(*)(int))tolower);
    return copy;
}

//convert to int
int string_toInt( const std::string& value)
{
	return atoi(value.c_str());
}
//convert to bool
bool string_toBool( const std::string& value)
{
	if(string_toLower(value) == "true")
		return true;
	return false;
}

#ifndef WIN32
// 函数名: itoa
// 描述  : 为了和WINDOWS平台兼容，将字符串转换为指定进制的整数
// 返回  : char*
char* itoa(int value, char* result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}
#endif


/* 去除左右空格   */
char *TrimBlank(char *s)
{
	int i,len;

	len=strlen(s);
	for (i=len-1;i>=0;i--)
	{
		if ((s[i] != ' ') && (s[i] != '\t'))
			break;
		else
			s[i]=0;
	}
	for (i=0; i<len; i++)
	{
		if ((s[i] != ' ') && (s[i] != '\t'))
			break;
	}
	if (i != 0)
	{
		strncpy(s,s+i,len-i);
		s[len-i]=0;
	}
	return s;
}

// 函数名: *Strncpy
// 描述  : 从source中最多复制count字符到dest，但是在dest[count-1]被赋值为'\0'字符串结束符
// 返回  : char 
// 参数  : char *dest
// 参数  : char *source
// 参数  : size_t count
char *Strncpy(char *dest,char *source,unsigned int count)
{
   strncpy(dest,source,count);
   dest[count-1]='\0';
   return(dest);
}


// 函数名: *Strncpy_t
// 描述  : 从source中最多复制count字符到dest，但是在dest[count-1]被赋值为'\0'字符串结束符
//        同时去除左右空格
// 返回  : char 
// 参数  : char *dest
// 参数  : char *source
// 参数  : size_t count
char *Strncpy_trim(char *dest,char *source,unsigned int count)
{
   TrimBlank(Strncpy(dest,source,count));
   return(dest);
}

// 函数名: mytrim_r
// 描述  : 清除字符串右边的' '和'\t'
// 返回  : char *
// 参数  : char *dest
char *TrimRight(char *dest)
{
   int i;
   i = strlen(dest)-1;
   for (;i>=0;i--)
   {
		if ((dest[i] != ' ') && (dest[i] != '\t'))
			break;
		else
			dest[i]=0;
   }
   return(dest);
}


// 函数名: Strncpy_rt
// 描述  : 复制并清除dest右边的' '和'\t'
// 返回  : char * 就是dest的指针
// 参数  : char *dest
// 参数  : char *source
// 参数  : size_t count
char *Strncpy_rt(char *dest,char *source,unsigned int count)
{
   Strncpy(dest,source,count);
   return(TrimRight(dest));
}

// 函数名: percolate
// 描述  : 从字符串中将非法字符过滤掉，同时将以//作为后续为注释描述也同//一起过滤掉
// 返回  : int 结束串的长度
// 参数  : char *str In、Out
int percolate(char *str)
{
   int l,i;
   l = strlen(str)-1;
   for (;l>=0;l--)
   {
      if ((unsigned char)(str[l])<0x20)
         str[l]='\0';
      else break;
   }
   for (i=0;i<=l;i++)
   {
      if (str[i]=='/' && str[i+1]=='/') 
      {
         str[i]='\0';
         break;
      }
   }
   return(i);
}

// 函数名: mysleep
// 描述  : 本线程进入睡眠状态milliseconds毫秒
// 返回  : void 
// 参数  : unsigned int milliseconds
void mysleep(unsigned int milliseconds)
{
#ifdef WIN32
   Sleep(milliseconds);
#else
#ifdef AIX
	 struct timespec delay;
	 delay.tv_sec =0;
	 delay.tv_nsec = milliseconds*1000;
    pthread_delay_np(&delay);
#else
    usleep(milliseconds*1000);
#endif
#endif
}

// 函数名: *getfmttime
// 描述  : 将tm的时间转换为本地时间的格式化字符串，本子系统中non-thread-safe
// 返回  : char * 格式化的字符串指针, NULL 则使用内部静态空间存放格式化的字符串，
// 参数  : time_t *ptm ：需要转换的时间 ==NULL 则为转换时时间点
// 参数  : int fmttype 
/*
            0 : hh:mm:ss
            1 : MM/DD-hh:mm:ss
            2 : YYYY/MM/DD-hh:mm:ss
            3 : YYYYMMDD
            4 : hhmmss
            其他 : YYYYMMDDhhmmss
*/
// 参数  : char *outstr 输出字符串的缓存，=NULL则使用内部的静态缓存，其长度需要视fmttype而定
char *getfmttime(void *ptm,int fmttype,char *outstr)
{
   static char tstr[80];
   struct tm tmloc;
   time_t timet;
   
   if (ptm==NULL)
   {
      ptm = &timet;
      time(&timet);
   }
   else
   {
      memcpy(&timet,ptm,sizeof(timet));
   }
   if (outstr==NULL)
   {
      outstr = tstr;
   }
   //localtime_r(&tm,&tmloc);
   memcpy(&tmloc,localtime(&timet),sizeof(tmloc));
   if (fmttype==0)
   {
      // %H:%M:%S:
      strftime(outstr,79,"%H:%M:%S",&tmloc);
   }
   else if (fmttype==1)
   {
      // %m/%d-%H:%M:%S
      strftime(outstr,79,"%m/%d %H:%M:%S",&tmloc);
   }
   else if (fmttype==2)
   {
      strftime(outstr,79,"%Y/%m/%d %H:%M:%S",&tmloc);
   }
   else if (fmttype==3)
   {
      strftime(outstr,79,"%Y%m%d",&tmloc);
   }
   else if (fmttype==4)
   {
      strftime(outstr,79,"%H%M%S",&tmloc);
   }
   else 
   {
      strftime(outstr,79,"%Y%m%d%H%M%S",&tmloc);
   }
   return(outstr);
}


void OutErrorFile(char *efilename,char *msg,char *sourcefile,int lineno)
{
   FILE *fp;
   fp = fopen(efilename,"a+t");
   if (fp!=NULL)
   {
      fprintf(fp,
         "%s FILE:%s LINENO:%d -- [%s]\n",
         getfmttime(NULL,1,NULL),
         sourcefile,
         lineno,
         msg
         );      
      fclose(fp);
   }
}

void DebugFileOut(char *dfilename,char *msg,char *sourcefile,int lineno)
{
   char *pmsg;

   {
      pmsg = msg;
   }
   OutErrorFile(dfilename,pmsg,sourcefile,lineno);
   if (pmsg[strlen(pmsg)-2]=='\n')
   {
      printf("%s",pmsg);
   }
   else
   {
      printf("%s\n",pmsg);
   }
}

#ifdef WIN32
unsigned int tc2_tc1(unsigned int tc2,unsigned int tc1)
{
   return(tc2-tc1);
}
#else
unsigned int tc2_tc1(unsigned int tc2,unsigned int tc1)
{
   if (tc2<tc1)
   {
      return(86400000-tc1+tc2);
   }
   else
   {
      return(tc2-tc1);
   }
}

unsigned int GetTickCount()
{
    struct timeval CurrentTime;
    gettimeofday(&CurrentTime, NULL);
    return ((CurrentTime.tv_sec * 1000) + (CurrentTime.tv_usec / 1000));
}

unsigned int GetTickCountUS()
{
    struct timeval CurrentTime;
    gettimeofday(&CurrentTime, NULL);
    return(CurrentTime.tv_usec+(CurrentTime.tv_sec%1000)*1000000);
}

#endif

bool checkSystem(void)
{
	typedef struct
	{
		BYTE a;
		WORD b;
		INT4 c;
	}	testStruct;
	if (sizeof(BYTE)!=1){
		printf("BYTE\n");
		return false;
	}
	if (sizeof(WORD)!=2){
		printf("WORD\n");
		return false;
	}
	if (sizeof(INT4)!=4){
		printf("INT4\n");
		return false;
	}
	if (sizeof(testStruct)!=7){
		printf("testStruct\n");
		return false;
	}
	if (getEndian()==_UNKNOWN_ENDIAN_){
		printf("getEndian\n");
		return false;
	}else
	{
		getEndian();
	}
	return true;
}

int getEndian(void)
{
	static int endian=_UNKNOWN_ENDIAN_;
	union
	{
		char a[4];
		int b;
	}	u;
	if (endian!=_UNKNOWN_ENDIAN_)
		return endian;
	u.a[0]=1;
	u.a[1]=2;
	u.a[2]=3;
	u.a[3]=4;
	if (u.b==0x01020304)
	{
		endian=_BIG_ENDIAN_;
	}
	else if (u.b==0x04030201)
	{
		endian=_SMALL_ENDIAN_;
	}
	return endian;
}

//用于把高字节序的网络字符转成系统可用的字符
int _getIntEndian(int value)
{
	if (getEndian()==_SMALL_ENDIAN_)
	{
		return	((value&0x000000ff)<<24) |
			((value&0x0000ff00)<<8)  |
			((value&0x00ff0000)>>8)  |
			((value&0xff000000)>>24);
	}
	else
		return value;
}

// mdf by WuJinghe on 2013-04-01 增加支持64bit
//long _getLongEndian(long value)
//{
//    if (getEndian()==_SMALL_ENDIAN_)
//    {
//        return	((value&0x000000ff)<<24) |
//            ((value&0x0000ff00)<<8)  |
//            ((value&0x00ff0000)>>8)  |
//            ((value&0xff000000)>>24);
//    }
//    else
//        return value;
//}

long _getLongEndian(long value)
{
#if __x86_64__ || __ppc64__ || _WIN64
    return _getLongEndian64(value);
#else
    return _getLongEndian32(value);
#endif
}

long _getLongEndian32(long value)
{
    if (getEndian()==_SMALL_ENDIAN_)
    {
        return ((value&0x000000ff)<<24)
             | ((value&0x0000ff00)<<8)
             | ((value&0x00ff0000)>>8)
             | ((value&0xff000000)>>24);
    }
    else
        return value;
}

long _getLongEndian64(long value)
{
    if (getEndian()==_SMALL_ENDIAN_)
    {
        return ((value & 0x00000000000000ff) << 56)
            | ((value & 0x000000000000ff00) << 40)
            | ((value & 0x0000000000ff0000) << 24)
            | ((value & 0x00000000ff000000) << 8)
            | ((value & 0x000000ff00000000) >> 8)
            | ((value & 0x0000ff0000000000) >> 24)
            | ((value & 0x00ff000000000000) >> 40)
            | ((value & 0xff00000000000000) >> 56);
    }
    else
        return value;
}
// end mdf 2013-04-01

double _getDoubleEndian(double value)
{
	//需要把double 强制转换成unsigned int来进行转换。
	unsigned int tmp = value;
	if (getEndian()==_SMALL_ENDIAN_)
	{
		return	((tmp&0x000000ff)<<24) |
			((tmp&0x0000ff00)<<8)  |
			((tmp&0x00ff0000)>>8)  |
			((tmp&0xff000000)>>24);
	}
	else
		return value;
}


//用于把系统可同字符转变成网络传输的高字节序。
void _toIntEndian(int &value){
	if (getEndian()==_SMALL_ENDIAN_)
	{
		value = ((value&0x000000ff)<<24) |((value&0x0000ff00)<<8)  |((value&0x00ff0000)>>8)  |((value&0xff000000)>>24);
	}
	else
		return ;
}

// mdf by WuJinghe on 2013-04-01 增加支持64bit
//void _toLongEndian32(long &value){
//    if (getEndian()==_SMALL_ENDIAN_)
//    {
//        value = ((value&0x000000ff)<<24)
//            |((value&0x0000ff00)<<8)
//            |((value&0x00ff0000)>>8)
//            |((value&0xff000000)>>24);
//    }
//    else
//        return ;
//}
void _toLongEndian(long& value)
{
#if __x86_64__ || __ppc64__ || _WIN64
    _toLongEndian64(value);
#else
    _toLongEndian32(value);
#endif
}

void _toLongEndian32(long &value){
    if (getEndian()==_SMALL_ENDIAN_)
    {
        value = ((value&0x000000ff)<<24)
            |((value&0x0000ff00)<<8)
            |((value&0x00ff0000)>>8)
            |((value&0xff000000)>>24);
    }
    else
        return ;
}

void _toLongEndian64(long &value)
{
    if (getEndian()==_SMALL_ENDIAN_)
    {
        value = ((value & 0x00000000000000ff) << 56)
              | ((value & 0x000000000000ff00) << 40)
              | ((value & 0x0000000000ff0000) << 24)
              | ((value & 0x00000000ff000000) << 8)
              | ((value & 0x000000ff00000000) >> 8)
              | ((value & 0x0000ff0000000000) >> 24)
              | ((value & 0x00ff000000000000) >> 40)
              | ((value & 0xff00000000000000) >> 56);
    }
    else
        return ;
}
// end mdf 2013-04-01

union DoubleAndChar {
	double dval;
	unsigned char ucval[8];
};

void _toDoubleEndian(double &value){
	if (getEndian()==_SMALL_ENDIAN_)
	{
		DoubleAndChar dac;
		unsigned char ucTmp;
		dac.dval = value;
		for(int i=0;i<4;i++) {
			ucTmp = dac.ucval[i];
			dac.ucval[i] = dac.ucval[7-i];
			dac.ucval[7-i] = ucTmp;
		}
		value = dac.dval;
	}
	else
		return ;
}


#define WIN_DELIMITER '\\'
#define NIX_DELIMITER '/'
#ifdef WIN32
#define DELIMITER WIN_DELIMITER
#else
#define DELIMITER NIX_DELIMITER
#endif


/***增加路径串的尾分割符
* @param			路径串.
* @return	char *  路径串指针
* 如果原串已经包含尾分割符，则无变化
*/
char *IncludeDelimiter(char *fname)
{
    int len = strlen(fname);
	if (len)
	{
		switch (fname[len-1])
		{
		case WIN_DELIMITER:
		case NIX_DELIMITER:
			return fname;
		}
	}
	fname[len]   = DELIMITER;
	fname[len+1] = 0;
	return fname;
}

/**
 * 获取文件路径
 * @param       path  路径
 * @param       fname 文件名
 * @return
 */
std::string MakeFilePath(const std::string &path, const std::string &fname)
{
    char _path[FILENAME_MAX];
    strcpy(_path, path.c_str());
    
    std::string sFilePath = string(IncludeDelimiter(_path));
    sFilePath.append(fname);
    
    return sFilePath;
}



#ifdef WIN32

bool bNotGetFrenquecy=true;
LARGE_INTEGER frenquecy;
 
void gettimeofday(struct timeval *pt,void *ptr)
{
	LARGE_INTEGER perfCount;
	if (bNotGetFrenquecy)
	{
		QueryPerformanceFrequency(&frenquecy);
		bNotGetFrenquecy = false;
	}
	QueryPerformanceCounter(&perfCount);
	if (frenquecy.QuadPart>0)
	{
		double dd = (double)perfCount.QuadPart/frenquecy.QuadPart;
		pt->tv_sec = (long)dd;
		pt->tv_usec = ((long)((dd-pt->tv_sec)*1000000.))%1000000;
	}
	else
	{
		pt->tv_sec = perfCount.QuadPart/1000000;
		pt->tv_usec = perfCount.QuadPart%1000000;
	}	
}
#endif

namespace UTILITY
{
unsigned long GetTid()
{
#ifdef WIN32
    DWORD tid = GetCurrentThreadId();
#elif _AIX_
    pthread_t tid = pthread_self();
#else
    pid_t tid = static_cast<pid_t>(::syscall(SYS_gettid));
#endif

    return static_cast<unsigned long>(tid);
}

int Snprintf( char *buffer, int buff_size, const char *format, ... )
{
    va_list arg;
    va_start(arg, format);
#ifdef WIN32
    int ret = _vsnprintf( buffer, buff_size, format, arg );
    buffer[buff_size - 1] = '\0';
#else
    int ret = vsnprintf(buffer, buff_size, format, arg );
#endif
    va_end(arg);
    return ret;
}

int Vsnprintf(char *buffer, int buff_size, const char *format, va_list ap)
{
#ifdef WIN32
    int ret = _vsnprintf( buffer, buff_size, format, ap );
    buffer[buff_size - 1] = '\0';
#else
    int ret = ::vsnprintf(buffer, buff_size, format, ap );
#endif
    return ret;

}


#ifdef _AIX_
int GetProcNameByID(pid_t pid, char *name, int len)
{
	pid_t id  = pid;
	struct procsinfo pi;
	if (getprocs(&pi, sizeof(struct procsinfo), 0, 0, &id, 1) != 1)
	{
		return -1;
	}
	if (pi.pi_pid != pid)
	{
		return -1;
	}

	if (len < strlen(pi.pi_comm))
	{
		return -2;
	}

	strncpy(name, pi.pi_comm, len);
	return 0;
}
#endif


//获取程序运行的根目录                                             
const char* GetProcName(void)                                       
{                                                                  
	static bool     bGetName = false;                              
	static char     acName[256];                                   
	int             iPathLen = 0;                                                              

	if (false == bGetName)                                         
	{                                                              
		char acPath[256] = {0};                                    
		char *szCur, *szFormat;                                    
		int  i = 0;

		memset(acName, 0, sizeof(acName)); 

#ifdef _AIX_
		if(0 != GetProcNameByID(getpid(), acName, sizeof(acName)))
		{
			acName[0] = 0;
			return acName;
		}
		bGetName = true;
		return acName;
#endif

#ifdef WIN32
		char acTmpName[256];

		if (GetModuleFileName(NULL, acPath, sizeof(acPath)-1) == 0)
		{
			printf("get app full path error\n");
			acName[0] = 0;
			return acName;
		}                                                          
		iPathLen = strlen(acPath) - 1;		
		memset(acTmpName, 0, sizeof(acTmpName));

		for ( ; iPathLen > 0; iPathLen--)                          
		{                                                          
			if(acPath[iPathLen] == '\\' || acPath[iPathLen] == '/')
			{                                                      
				break;                                             
			}                                                      
			acTmpName[i++] = acPath[iPathLen];                     
		}                                                          

		szFormat = strstr(acTmpName, ".");                         
		szCur = acTmpName + strlen(acTmpName) - 1;                 

		i = 0;                                                     
		while(szCur >= acTmpName && szCur > szFormat)              
		{                                                          
			acName[i++] = *szCur--;                                
		}
		bGetName = true;
		return acName;
#else
		pid_t iPid = getpid();
		char  acParam[128] = {0};
		int   iCount;

		snprintf(acParam, sizeof(acParam), "/proc/%d/exe", iPid);
		iCount = readlink(acParam, acPath, sizeof(acPath));
		if(iCount < 0 || iCount >= sizeof(acPath))
		{
			printf("get app full path error\n");
			acName[0] = 0;
			return acName;
		}

		szCur = strrchr(acPath, '/');
		if(szCur)
		{
			szCur++;
			snprintf(acName, sizeof(acName), "%s", szCur);
		}
		else
		{
			snprintf(acName, sizeof(acName), "%s", acPath);
		}
		bGetName = true;
		return acName;
#endif                                                          
		                                       
	}                                                      

	return acName;                                                 
}

} // end namespace UTILITY
