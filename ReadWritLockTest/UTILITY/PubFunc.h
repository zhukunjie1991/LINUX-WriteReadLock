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


//��Windows�£����¶���MAX��MIN��
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
// ������: itoa
// ����  : Ϊ�˺�WINDOWSƽ̨���ݣ����ַ���ת��Ϊָ�����Ƶ�����
// ����  : char*
char* itoa(int value, char* result, int base);
#endif

/* ȥ�����ҿո�   */
char *TrimBlank(char *s);


// ������: *Strncpy
// ����  : ��source����ิ��count�ַ���dest��������dest[count-1]����ֵΪ'\0'�ַ���������
// ����  : char 
// ����  : char *dest
// ����  : char *source
// ����  : size_t count
char *Strncpy(char *dest,char *source,unsigned int count);

// ������: *Strncpy_t
// ����  : ��source����ิ��count�ַ���dest��������dest[count-1]����ֵΪ'\0'�ַ���������
//        ͬʱȥ�����ҿո�
// ����  : char 
// ����  : char *dest
// ����  : char *source
// ����  : size_t count
char *Strncpy_trim(char *dest,char *source,unsigned int count);

// ������: mytrim_r
// ����  : ����ַ����ұߵ�' '��'\t'
// ����  : char *
// ����  : char *dest
char *TrimRight(char *dest);

// ������: Strncpy_rt
// ����  : ���Ʋ����dest�ұߵ�' '��'\t'
// ����  : char * ����dest��ָ��
// ����  : char *dest
// ����  : char *source
// ����  : size_t count
char *Strncpy_rt(char *dest,char *source,unsigned int count);


// ������: percolate
// ����  : ���ַ����н��Ƿ��ַ����˵���ͬʱ����//��Ϊ����Ϊע������Ҳͬ//һ����˵�
// ����  : int �������ĳ���
// ����  : char *str In��Out
int percolate(char *str);

// ������: *getfmttime
// ����  : ��tm��ʱ��ת��Ϊ����ʱ��ĸ�ʽ���ַ���������ϵͳ��non-thread-safe
// ����  : char * ��ʽ�����ַ���ָ��, NULL ��ʹ���ڲ���̬�ռ��Ÿ�ʽ�����ַ�����
// ����  : time_t *ptm ����Ҫת����ʱ�� ==NULL ��Ϊת��ʱʱ���
// ����  : int fmttype 
/*
            0 : hh:mm:ss
            1 : MM/DD-hh:mm:ss
            2 : YYYY/MM/DD-hh:mm:ss
            3 : YYYYMMDD
            4 : hhmmss
            ���� : YYYYMMDDhhmmss
*/
// ����  : char *outstr ����ַ����Ļ��棬=NULL��ʹ���ڲ��ľ�̬���棬�䳤����Ҫ��fmttype����
char *getfmttime(void *ptm,int fmttype,char *outstr);


// ������: mysleep
// ����  : ���߳̽���˯��״̬milliseconds����
// ����  : void 
// ����  : unsigned int milliseconds
void mysleep(unsigned int milliseconds);

#ifndef WIN32
// ������: GetTickCount
// ����  : Ϊ�˺�WINDOWSƽ̨���ݣ���ȡ��ǰ��ʱ���
// ����  : unsigned int 
unsigned int GetTickCount();
unsigned int GetTickCountUS();
#endif


// ������: tc2_tc1
// ����  : ��GetTickCount()�õ��ĺ���ʱ���Ĳ������ƣ� t2-t1 �����ڷ�WINDOWS��ʱ���ʵ�ʲ��ܳ���1��
// ����  : unsigned int   tc2-tc1 
// ����  : unsigned int tc2
// ����  : unsigned int tc1
unsigned int tc2_tc1(unsigned int tc2,unsigned int tc1);

// ������: OutErrorFile
// ����  : ��������Ϣ��¼�������¼�ļ���
// ����  : void 
// ����  : char *efilename
// ����  : char *msg
// ����  : char *sourcefile
// ����  : int lineno
void OutErrorFile(char *efilename,char *msg,char *sourcefile,int lineno);


// ������: DebugFileOut
// ����  : ����Ϣ��¼�����ٵ����ļ��У��Է�����Ժ�����Ĳ�ѯ
// ����  : void 
// ����  : char *dfilename ���ٵ����ļ������ļ���
// ����  : char *msg ���ٵ�����Ϣ
// ����  : char *sourcefile Դ�ļ�����
// ����  : int lineno Դ�ļ����к�
void DebugFileOut(char *dfilename,char *msg,char *sourcefile,int lineno);


/***����·������β�ָ��
* @param			·����.
* @return	char *  ·����ָ��
* ���ԭ���Ѿ�����β�ָ�������ޱ仯
*/
char *IncludeDelimiter(char *fname);

/**
 * ��ȡ�ļ�·��
 * @param       path  ·��
 * @param       fname �ļ���
 * @return
 */
//std::string MakeFilePath(const std::string &path, const std::string &fname);



//�׳�һ��CDesignError�쳣
//@����	msg	char*	ָ���Ĵ�����Ϣ
//@�쳣	CDesignError	�׳����쳣������λ���ڵ�ǰ�ļ���ǰ��
#define RAISE_DESIGN_ERROR(msg) { OutErrorFile("DesignError",msg,__FILE__,__LINE__); exit(-1000);}

//�׳�һ��CRuntimeError
//@����	msg	char*	ָ���Ĵ�����Ϣ
//@�쳣	CRuntimeError	�׳����쳣������λ���ڵ�ǰ�ļ���ǰ��
#define RAISE_RUNTIME_ERROR(msg) { OutErrorFile("RuntimeError",msg,__FILE__,__LINE__); exit(-1001);}

#ifdef WIN32
	void gettimeofday(struct timeval *pt,void *ptr);
#endif


#ifdef __cplusplus
}
#endif

//���Ӷ�64λϵͳ���жϺͶԸߵ��ֽ�����ж� 
#define _BIG_ENDIAN_ 1
#define _SMALL_ENDIAN_ 2
#define _UNKNOWN_ENDIAN_ 0

bool checkSystem(void);
int getEndian(void);
//���ڰѸ��ֽ���������ַ�ת��ϵͳ���õ��ַ�
int _getIntEndian(int value);

long _getLongEndian(long value);
long _getLongEndian32(long value);
long _getLongEndian64(long value);

double _getDoubleEndian(double value);

//���ڰ�ϵͳ��ͬ�ַ�ת������紫��ĸ��ֽ���
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
