/**
�޸����ݣ�
1.ȥ������
2.��ͷ�ļ������Ƶ�.h�У�ȥ������Ҫ��ͷ�ļ�
3.��ԭ����GlobalDefs.h��ErrorCodes.h��Ӧ���ڴ˶���ĺ��ҳ����������ڴ˴���
4.ԭ�������INT64�����ͣ���������
5.����windows�������չ��
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

//��Ϊ����Ҫʹ�������ռ��ģʽ
#define PRINT_ERR(...) UtilLib::ProcessEventLog(__FILE__,__LINE__,TRC_ERR,ERR_TRC,0,__VA_ARGS__)
#define PRINT_DBG(...) UtilLib::ProcessEventLog(__FILE__,__LINE__,TRC_DBG,ERR_TRC,0,__VA_ARGS__)

namespace UtilLib {

#ifdef WIN32
#define LOCALTIME_R(t) localtime((t))
#else
#define LOCALTIME_R(t) localtime_r((t), (struct tm*)&tmres)
#endif

//���Ӷ������ͱ��������ַ�������ȫ��δ��Ҫ��
//���ͱ���
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

//�ں궨��ǰ����LOG_ǰ׺�����ٿ��ܷ������ض������
#define LOG_MAX_MSG_LEN 	10000 /** Internal change to increase the log message size - By swapnil **/
#define LOG_ERR_INTAPL          8
#define LOG_MAX_PATH_LEN        200
#define LOG_PROCESS_NAME_LEN    200
//2��3û�����壬��Ҫ�ĵ�
#define LOG_DEFAULT_LANGUAGE    2
#define LOG_DEFAULT_TRACE_LEVEL 3
#define LOG_TIME_LEN            26
#define LOG_TM_YEAR_OFFSET	1900
#define LOG_TM_MONTH_OFFSET	1
//��֪�����á���ʱע�͵�
//SPARCLONG64 gCmltFwdSqNo = 0;

//ע�⣬ԭ���˲���Ӧ�ò��ܰ���ҵ��Ķ�������˽�����ǰ��3�ּ��ɣ����򲻷��Ͽɸ��������Ҫ��������ֶ�Ŀǰûʲô�á�
//Ϊ����չ�ԣ����ǲ����ٶ���
#define	ERR_TRC	  0
#define ERR_APPL  1
#define ERR_DB	  2
#define	ERR_MQ	  3
#define	ERR_UQ	  4
#define ERR_OPT	  5
#define ERR_SOC	  6
#define ERR_SHM	  7
#define ERR_BATCH -1
//ͬ��ֻ��Ҫ����ǰ��3�ּ�
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

//����ӿ�
VOID RegisterErrorLog(CHAR *,CHAR *,LONG32,LONG32);

VOID RegisterErrorLogT(CHAR *,CHAR *,LONG32,LONG32,LONG32,LONG32);

VOID ProcessEventLog(CHAR* FILE,LONG32 LINE,LONG32,LONG32,LONG32,const CHAR *,...);
//����ע���ķ���
VOID ExitErrorLogT();

//����������������������RaiseError����
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
