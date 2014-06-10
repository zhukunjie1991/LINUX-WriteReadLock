#ifndef PUBFUNC_H
#define PUBFUNC_H

#include <stdio.h>
#include <string>
#include <time.h>
#include <ctime>
#include <map>

#ifdef WIN32
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#endif

using namespace std;

typedef map<string, string> STRING_MAP_TYPE;

void string_replace( const std::string& oldValue,
		     const std::string& newValue,
		     std::string& value );
std::string string_toLower( const std::string& value );
std::string string_toUpper( const std::string& value );

//convert to int
int string_toInt( const std::string& value);
//convert to bool
bool string_toBool( const std::string& value);

//for C style

#ifdef __cplusplus
extern "C" {
#endif


#ifndef BYTE
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
#endif
#ifndef INT4
typedef char			INT1;
typedef short			INT2;
typedef int				INT4;
#endif
#ifndef REAL4
typedef float			REAL4;
typedef double			REAL8;
#endif


//在Windows下，重新定义MAX和MIN宏
#ifdef WIN32
#define MAX(a,b) _MAX(a,b)
#define MIN(a,b) _MIN(a,b)
#endif

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(booleanExpression )					\
	if(!(booleanExpression)){						\
		RAISE_DESIGN_ERROR(#booleanExpression);		\
	}

#define DebugMessageDisplay(msg)   printf("%s:%d <%s>\n",__FILE__,__LINE__,msg)

#else
#define ASSERT(booleanExpression)
#define DebugMessageDisplay(msg)
#endif
#endif

#ifdef WIN32
#include <share.h>
#define sh_fopen(a,b,c) _fsopen(a,b,c)
#else
#define SH_DENYNO    0x40
#define SH_DENYWR    0x20
#define sh_fopen(a,b,c) fopen(a,b)
#endif

#ifdef WIN32
#include <direct.h>
#include <stdlib.h> 
#define MAKEPATH(a) mkdir(a)
#else
#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h>
 
#define MAKEPATH(a) mkdir(a,0777)
#endif


#ifndef WIN32
// 函数名: itoa
// 描述  : 为了和WINDOWS平台兼容，将字符串转换为指定进制的整数
// 返回  : char*
char* itoa(int value, char* result, int base);
#endif

/* 去除左右空格   */
char *TrimBlank(char *s);


// 函数名: *Strncpy
// 描述  : 从source中最多复制count字符到dest，但是在dest[count-1]被赋值为'\0'字符串结束符
// 返回  : char 
// 参数  : char *dest
// 参数  : char *source
// 参数  : size_t count
char *Strncpy(char *dest,char *source,unsigned int count);

// 函数名: *Strncpy_t
// 描述  : 从source中最多复制count字符到dest，但是在dest[count-1]被赋值为'\0'字符串结束符
//        同时去除左右空格
// 返回  : char 
// 参数  : char *dest
// 参数  : char *source
// 参数  : size_t count
char *Strncpy_trim(char *dest,char *source,unsigned int count);

// 函数名: mytrim_r
// 描述  : 清除字符串右边的' '和'\t'
// 返回  : char *
// 参数  : char *dest
char *TrimRight(char *dest);

// 函数名: Strncpy_rt
// 描述  : 复制并清除dest右边的' '和'\t'
// 返回  : char * 就是dest的指针
// 参数  : char *dest
// 参数  : char *source
// 参数  : size_t count
char *Strncpy_rt(char *dest,char *source,unsigned int count);


// 函数名: percolate
// 描述  : 从字符串中将非法字符过滤掉，同时将以//作为后续为注释描述也同//一起过滤掉
// 返回  : int 结束串的长度
// 参数  : char *str In、Out
int percolate(char *str);

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
char *getfmttime(void *ptm,int fmttype,char *outstr);


// 函数名: mysleep
// 描述  : 本线程进入睡眠状态milliseconds毫秒
// 返回  : void 
// 参数  : unsigned int milliseconds
void mysleep(unsigned int milliseconds);

#ifndef WIN32
// 函数名: GetTickCount
// 描述  : 为了和WINDOWS平台兼容，获取当前的时间点
// 返回  : unsigned int 
unsigned int GetTickCount();
unsigned int GetTickCountUS();
#endif


// 函数名: tc2_tc1
// 描述  : 以GetTickCount()得到的毫秒时间点的差，（毫秒计） t2-t1 ，对于非WINDOWS，时间差实际不能超过1天
// 返回  : unsigned int   tc2-tc1 
// 参数  : unsigned int tc2
// 参数  : unsigned int tc1
unsigned int tc2_tc1(unsigned int tc2,unsigned int tc1);

// 函数名: OutErrorFile
// 描述  : 将错误信息记录到错误记录文件中
// 返回  : void 
// 参数  : char *efilename
// 参数  : char *msg
// 参数  : char *sourcefile
// 参数  : int lineno
void OutErrorFile(char *efilename,char *msg,char *sourcefile,int lineno);


// 函数名: DebugFileOut
// 描述  : 将信息记录到跟踪调试文件中，以方便调试和问题的查询
// 返回  : void 
// 参数  : char *dfilename 跟踪调试文件生成文件名
// 参数  : char *msg 跟踪调试信息
// 参数  : char *sourcefile 源文件名称
// 参数  : int lineno 源文件的行号
void DebugFileOut(char *dfilename,char *msg,char *sourcefile,int lineno);


/***增加路径串的尾分割符
* @param			路径串.
* @return	char *  路径串指针
* 如果原串已经包含尾分割符，则无变化
*/
char *IncludeDelimiter(char *fname);

/**
 * 获取文件路径
 * @param       path  路径
 * @param       fname 文件名
 * @return
 */
//std::string MakeFilePath(const std::string &path, const std::string &fname);



//抛出一个CDesignError异常
//@参数	msg	char*	指定的错误信息
//@异常	CDesignError	抛出此异常，错误位置在当前文件当前行
#define RAISE_DESIGN_ERROR(msg) { OutErrorFile("DesignError",msg,__FILE__,__LINE__); exit(-1000);}

//抛出一个CRuntimeError
//@参数	msg	char*	指定的错误信息
//@异常	CRuntimeError	抛出此异常，错误位置在当前文件当前行
#define RAISE_RUNTIME_ERROR(msg) { OutErrorFile("RuntimeError",msg,__FILE__,__LINE__); exit(-1001);}

#ifdef WIN32
	void gettimeofday(struct timeval *pt,void *ptr);
#endif


#ifdef __cplusplus
}
#endif

//增加对64位系统的判断和对高低字节序的判断 
#define _BIG_ENDIAN_ 1
#define _SMALL_ENDIAN_ 2
#define _UNKNOWN_ENDIAN_ 0

bool checkSystem(void);
int getEndian(void);
//用于把高字节序的网络字符转成系统可用的字符
int _getIntEndian(int value);

long _getLongEndian(long value);
long _getLongEndian32(long value);
long _getLongEndian64(long value);

double _getDoubleEndian(double value);

//用于把系统可同字符转变成网络传输的高字节序。
void _toIntEndian(int &value);

void _toLongEndian(long &value);
void _toLongEndian32(long &value);
void _toLongEndian64(long &value);

void _toDoubleEndian(double &value);

namespace UTILITY
{
unsigned long GetTid();
int Snprintf(char *buffer, int buff_size, const char *format, ...);
int Vsnprintf(char *buffer, int buff_size, const char *format, va_list ap);
const char* GetProcName(void);
} // end namespace UTILITY

#endif
