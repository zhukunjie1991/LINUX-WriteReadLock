/**
修改内容：
1.去掉所有
2.把头文件引用移到.h中，去掉不必要的头文件
3.把原来在GlobalDefs.h和ErrorCodes.h中应该在此定义的宏找出来，定义在此处。
4.原来定义的INT64等类型，做个保护
5.增加windows下面的扩展性
*/
#pragma once

//#include "stdafx.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#ifdef WIN32
#include <Winsock2.h>
#include <windows.h>
#include <process.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <pthread.h>

//改为不需要使用命名空间的模式
#define PRINT_ERR(...) UtilLib::ProcessEventLog(__FILE__,__LINE__,TRC_ERR,ERR_TRC,0,__VA_ARGS__)
#define PRINT_DBG(...) UtilLib::ProcessEventLog(__FILE__,__LINE__,TRC_DBG,ERR_TRC,0,__VA_ARGS__)

namespace UtilLib {

#ifdef WIN32
#define LOCALTIME_R(t) localtime((t))
#else
#define LOCALTIME_R(t) localtime_r((t), (struct tm*)&tmres)
#endif

//增加多项类型保护，这种方法不安全，未来要改
//类型保护
//typedef char CHAR;
#ifndef WIN32
    #ifndef VOID
    #define VOID void
    #endif
#endif
#ifndef INT64
typedef int INT64;
#endif
typedef long LONG32;
#ifndef CHAR
typedef char CHAR;
#endif
#ifndef BOOL
typedef bool BOOL;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

//在宏定义前增加LOG_前缀，减少可能发生的重定义情况
#define LOG_MAX_MSG_LEN 	10000 /** Internal change to increase the log message size - By swapnil **/
#define LOG_ERR_INTAPL          8
#define LOG_MAX_PATH_LEN        200
#define LOG_PROCESS_NAME_LEN    200
//2和3没有意义，需要改掉
#define LOG_DEFAULT_LANGUAGE    2
#define LOG_DEFAULT_TRACE_LEVEL 3
#define LOG_TIME_LEN            26
#define LOG_TM_YEAR_OFFSET	1900
#define LOG_TM_MONTH_OFFSET	1
//不知道何用。暂时注释掉
//SPARCLONG64 gCmltFwdSqNo = 0;

//注意，原来此部分应该不能包括业务的东西。因此仅定义前面3种即可，否则不符合可复用软件的要求。且这个字段目前没什么用。
//为了扩展性，还是不减少定义
#define	ERR_TRC	  0
#define ERR_APPL  1
#define ERR_DB	  2
#define	ERR_MQ	  3
#define	ERR_UQ	  4
#define ERR_OPT	  5
#define ERR_SOC	  6
#define ERR_SHM	  7
#define ERR_BATCH -1
//同样只需要定义前面3种即
//CHAR cBaseErrType[][3] = {  "TRC","APL","DB "};
//
#define TRC_ERR   0
#define	TRC_WRN   1
#define TRC_INF   2
#define TRC_DBG   3

struct ERROR_LOG {
    INT64      MsgSeqNumber;
    FILE      *pErrorLog;
    LONG32     lSegmentId;
    LONG32     lInstanceId;
    LONG32     lLanguage;
    LONG32     lTraceLevel;
    CHAR       cLogFileName[LOG_MAX_PATH_LEN];
    CHAR       cProcessName[LOG_PROCESS_NAME_LEN];
    CHAR       cStatement[LOG_MAX_PATH_LEN];
    struct tm  tm					;
};

typedef struct ERROR_LOG _ERROR_LOG;

//对外接口
VOID RegisterErrorLog(CHAR *,CHAR *,LONG32,LONG32);

VOID RegisterErrorLogT(CHAR *,CHAR *,LONG32,LONG32,LONG32,LONG32);

VOID ProcessEventLog(CHAR* FILE,LONG32 LINE,LONG32,LONG32,LONG32,const CHAR *,...);
//增加注销的方法
VOID ExitErrorLogT();

//增加三个辅助方法，用于RaiseError方法
VOID VProcessEventLog(const CHAR* FileName,
                      LONG32 LineNo,
                      LONG32 lTraceLevel,
                      LONG32 lErrorType,
                      LONG32 lErrorCode,
                      _ERROR_LOG** pErrorLog,
                      const CHAR* fmtList,
                      va_list argList,
                      CHAR* logBuf);

VOID ProcessEventLogToFile(const CHAR* logBuf,
                           const _ERROR_LOG *pErrorLog,
                           LONG32 lTraceLevel);

VOID ProcessEventLogToScr(const CHAR* logBuf);
} // end namespace UtilLib
//#endif
