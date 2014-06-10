/**
修改内容：
1.去掉所有
2.把头文件引用移到.h中，去掉不必要的头文件
3.把CHAR cBaseErrType移到头文件中。增加宏定义
4.把变量定义移到头文件中
5.增加一个方法屏蔽掉
6.2012-2-17 增加日期的打印
*/
//#include "stdafx.h"
#include "Errorlog.h"

#include <stdlib.h>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
namespace UtilLib {
#define LOG_ERROR_TYPE_LEN 10
CHAR cBaseErrType[][LOG_ERROR_TYPE_LEN] = {  "TRC","APL","DB ","MQ ", "UQ ","OPT", "SOC","SHM","INT"};
BOOL bErrLogKeyInit = false;
//struct ERROR_LOG sGlobalErrorLog;
//struct ERROR_LOG * pThreadErrorLog;

static pthread_key_t KET;
pthread_mutex_t MET = PTHREAD_MUTEX_INITIALIZER;

/* Close the log file pointer THREAD_LOG. */
void close_thread_log (void* pThreadErrorLog)
{
	//fclose((FILE*) ((ERROR_LOG*) pThreadErrorLog)->pErrorLog);
}

VOID RegisterErrorLog(CHAR * pFileName,
		      CHAR * pProcessName,
		      LONG32 lSegmentId,
		      LONG32 lInstanceId)
{
	//LONG32		 status        = 0			;
	//memset(&sGlobalErrorLog,0,sizeof(sGlobalErrorLog))	;
	RegisterErrorLogT(pFileName   ,
			  pProcessName,
			  lSegmentId  ,
			  lInstanceId ,
			  LOG_DEFAULT_LANGUAGE   ,
			  LOG_DEFAULT_TRACE_LEVEL);
	return;
}

/******************************************************************************
*******************************************************************************
**   FUNCTION NAME     : RegisterErrorLogT                                   **
**                                                                           **
**   DESCRIPTION       :  此方法按照                                         **
**                                                                           **
**   NON_LOCAL DATA REF:                                                     **
**					                                     **
**   INPUT PARAMETER   :					 	     **
**									     **
**   OUTPUT PARAMETER  :						     **
**                                                                           **
**   CALLING FUNCTIONS :                                                     **
**                                                                           **
**   CALLED FUNCTIONS  :                                                     **
**                                                                           **
**   RETURN VALUE      : Error Code                                          **
**									     **
**   MOD HISTORY       :   						     **
*******************************************************************************
******************************************************************************/
VOID RegisterErrorLogT(CHAR * pFileName,
                       CHAR * pProcessName,
                       LONG32 lSegmentId,
                       LONG32 lInstanceId,
                       LONG32 lLanguage,
                       LONG32 lTraceLevel)
{
    LONG32		status = 0;
    time_t		lTime;
    CHAR		cTempPath[LOG_MAX_PATH_LEN];
    CHAR		cParamString[LOG_MAX_PATH_LEN];
    CHAR		cLogFile[LOG_MAX_PATH_LEN];

    _ERROR_LOG * pThreadErrorLog = NULL;

    //此处实际上类似singleton模式，考虑到多线程的环境因此通过加锁这一段来保证不会发生重入
    //如果没有初始化线程局部变量(bErrLogKeyInit!=!= TRUE),则TlsAlloc

    pthread_mutex_lock(&MET);
    if(!bErrLogKeyInit)
    {
        pthread_key_create(&KET,close_thread_log);
        bErrLogKeyInit = true;
    }
    pthread_mutex_unlock(&MET);

    pThreadErrorLog = (_ERROR_LOG *)malloc(sizeof(_ERROR_LOG));

    pthread_setspecific(KET,pThreadErrorLog);

    //初始化线程局部变量
    memset(pThreadErrorLog,0,sizeof(_ERROR_LOG));

    strcpy(pThreadErrorLog->cLogFileName,pFileName);
    strcpy(pThreadErrorLog->cProcessName,pProcessName);

    pThreadErrorLog->lSegmentId   = lSegmentId;
    pThreadErrorLog->lInstanceId  = lInstanceId;
    pThreadErrorLog->lLanguage    = lLanguage;
    pThreadErrorLog->lTraceLevel  = lTraceLevel;
    pThreadErrorLog->MsgSeqNumber = 0;
    sprintf(pThreadErrorLog->cStatement,
            "%s_%d_%d",
            pProcessName,
            lSegmentId,
            lInstanceId);

#ifdef WIN32
    time(&lTime);
    pThreadErrorLog->tm = *localtime(&lTime);

#else
    time(&lTime);
    localtime_r(&lTime,&pThreadErrorLog->tm);
#endif
    pThreadErrorLog->tm.tm_mon  += LOG_TM_MONTH_OFFSET;
    pThreadErrorLog->tm.tm_year += LOG_TM_YEAR_OFFSET;

    //更新文件名并打开文件
    memset(cTempPath,0,sizeof(cTempPath));
    //增加保护
    strcpy(cTempPath,"");
    //在filename中增加thread_name
    strcpy(cParamString,"%s%s");
    //在filename中增加threadid

    memset(cLogFile,0,sizeof(cLogFile));
    sprintf(cLogFile,
            cParamString,
            cTempPath,
            pThreadErrorLog->cLogFileName);

    //创建不存在的目录
    char * tag;
    for(tag=cLogFile;*tag;tag++)
    {
        if (*tag=='\\'||*tag=='/')
        {
            char buf[1000],path[1000];
            strcpy(buf,cLogFile);
            buf[strlen(cLogFile)-strlen(tag)+1] = 0 ;
            strcpy(path,buf);
        #ifdef WIN32
            if (access(path,6)==-1)
            {
                mkdir(path);
            }
        #else
            mkdir(path,0777);
        #endif
        }
    }

    pThreadErrorLog->pErrorLog = fopen(cLogFile,"a+");

/*
//如果返回空，表示该线程上的pErrorLog未被初始化。此处初始化
//保护，重用原来的pErrorLog
if(pThreadErrorLog == NULL)
{

}
*/
    return;
}

/*******************************************************************************
* 函数名称 : VProcessEventLog
* 功能描述 : 在一条日志记录中填充当前时间、文件名、行号、错误类型等，
				并将已填充的日志记录通过参数带出
* 参　　数 : const CHAR* FileName
* 参　　数 : LONG32 LineNo
* 参　　数 : LONG32 lTraceLevel
* 参　　数 : LONG32 lErrorType
* 参　　数 : LONG32 lErrorCode
* 参　　数 : _ERROR_LOG** pErrorLog
* 参　　数 : const CHAR* fmtList
* 参　　数 : va_list argList
* 参　　数 : CHAR* logBuf
* 返 回 值 : VOID
* 作　　者 : wujinghe
* 设计日期 : 2012年9月17日
* 修改日期     修改人    修改内容
*******************************************************************************/
VOID VProcessEventLog(const CHAR* FileName,
                      LONG32 LineNo,
                      LONG32 lTraceLevel,
                      LONG32 lErrorType,
                      LONG32 lErrorCode,
                      _ERROR_LOG** pErrorLog,
                      const CHAR* fmtList,
                      va_list argList,
                      CHAR* logBuf)
{
    CHAR *cLogBuffer = logBuf;
    CHAR cLogParamString[LOG_MAX_MSG_LEN];

    time_t lTime;
    struct tm tm;
    CHAR cErrorType[LOG_ERROR_TYPE_LEN];
    LONG32 iStrLen;
    const CHAR *BaseFileName = NULL;

    *pErrorLog = (_ERROR_LOG *)pthread_getspecific(KET);
    if (*pErrorLog == NULL || (*pErrorLog)->pErrorLog == NULL) {
        printf("Error in Writing ProcessEventLog file1 \n");
        return;
    }
    //如果为空，则表示需要初始化
    if (lTraceLevel > (*pErrorLog)->lTraceLevel) {
        return;
    }

    //basename用于只打印文件名

    //BaseFileName = basename(FileName);
    BaseFileName = FileName;

#ifdef WIN32
    time(&lTime);
    tm = *localtime(&lTime);
#else
    time(&lTime);
    localtime_r(&lTime, &tm);
#endif

    tm.tm_mon += LOG_TM_MONTH_OFFSET;
    tm.tm_year += LOG_TM_YEAR_OFFSET;

    if (lErrorCode < 0) {
        strcpy(cErrorType,"F");
    } else if(lErrorCode > 0) {
        strcpy(cErrorType,"W");
    } else {
        strcpy(cErrorType,"I");
    }

    //增加日期
    strcpy(cLogParamString,"{%d-%d-%d");
    if(tm.tm_hour < 10) {
        strcat(cLogParamString,".0%d");
    } else {
        strcat(cLogParamString,".%d");
    }

    if(tm.tm_min < 10) {
        strcat(cLogParamString,":0%d");
    } else {
        strcat(cLogParamString,":%d");
    }

    if(tm.tm_sec < 10) {
        strcat(cLogParamString,":0%d}");
    } else {
        strcat(cLogParamString,":%d}");
    }

    //增加日期打印
    strcat(cLogParamString, " {%s} {%s} {%d} {%s} [%ld] {");
    //增加日期
    iStrLen = sprintf(cLogBuffer,
                      cLogParamString,
                      tm.tm_year,
                      tm.tm_mon,
                      tm.tm_mday,
                      tm.tm_hour,
                      tm.tm_min,
                      tm.tm_sec,
                      (*pErrorLog)->cStatement,
                      cBaseErrType[lErrorType],
                      lErrorCode,
                      cErrorType,
                      (*pErrorLog)->MsgSeqNumber);

    iStrLen += vsprintf((cLogBuffer+iStrLen), fmtList, argList);
    iStrLen += sprintf(cLogBuffer+iStrLen, "} [%s:%d]", BaseFileName, LineNo);

    cLogBuffer[iStrLen] = '\n';
    cLogBuffer[iStrLen+1] = 0;

    return;
}

/*******************************************************************************
* 函数名称 : ProcessEventLogToFile
* 功能描述 : 将错误信息存入日志文件中
* 参　　数 : const CHAR* logBuf
* 参　　数 : const _ERROR_LOG* pErrorLog
* 参　　数 : LONG32 lTraceLevel
* 返 回 值 : VOID
* 作　　者 : wujinghe
* 设计日期 : 2012年9月17日
* 修改日期     修改人    修改内容
*******************************************************************************/
VOID ProcessEventLogToFile(const CHAR* logBuf,
                           const _ERROR_LOG* pErrorLog,
                           LONG32 lTraceLevel)
{
    if (logBuf == NULL) {
        printf("logBuf is NULL in ProcessEventLogToFile.\n");
        return;
    }

    if (pErrorLog->pErrorLog != NULL) {
        fprintf(pErrorLog->pErrorLog, logBuf);
        fflush(pErrorLog->pErrorLog);
    } else {
        printf("Error in Writing ProcessEventLog file2 \n");
    }

    return;
}


/*******************************************************************************
* 函数名称 : ProcessEventLogToScr
* 功能描述 : 将错误信息打印到屏幕中
* 参　　数 : const CHAR* logBuf
* 返 回 值 : VOID
* 作　　者 : wujinghe
* 设计日期 : 2012年9月17日
* 修改日期     修改人    修改内容
*******************************************************************************/
VOID ProcessEventLogToScr(const CHAR* logBuf)
{
    if (logBuf == NULL) {
        printf("logBuf is NULL in ProcessEventLogToScr.\n");
        return;
    }
    printf(logBuf);

    return;
}

/******************************************************************************
*******************************************************************************
**   FUNCTION NAME     : ProcessEventLog                                     **
**                                                                           **
**   DESCRIPTION       :                                                     **
**                                                                           **
**   NON_LOCAL DATA REF:                                                     **
**					                                     **
**   INPUT PARAMETER   :					 	     **
**									     **
**   OUTPUT PARAMETER  :						     **
**                                                                           **
**   CALLING FUNCTIONS :                                                     **
**                                                                           **
**   CALLED FUNCTIONS  :                                                     **
**                                                                           **
**   RETURN VALUE      : Error Code                                          **
**									     **
**   MOD HISTORY       :   						     **
*******************************************************************************
******************************************************************************/
VOID ProcessEventLog(CHAR* FileName,LONG32 LineNo,LONG32 lTraceLevel,
		     LONG32 lErrorType,
		     LONG32 lErrorCode,
		     const CHAR * pArgList, ...)
{
	CHAR		cLogBuffer[LOG_MAX_MSG_LEN];
	CHAR cLogParamString[LOG_MAX_MSG_LEN];
	//CHAR		cParamString[LOG_MAX_PATH_LEN];
	//CHAR		cLogFile[LOG_MAX_PATH_LEN]			;

	time_t		lTime					;
	struct tm	tm					;
	CHAR	        cErrorType[LOG_ERROR_TYPE_LEN]		;
	va_list		vlErrorList				;
	LONG32		iStrLen					;
	CHAR*			BaseFileName = NULL;
	_ERROR_LOG * pErrorLog = NULL			;

	pErrorLog = (_ERROR_LOG *)pthread_getspecific(KET);

	if (pErrorLog == NULL || pErrorLog->pErrorLog == NULL)
	{
		printf("Error in Writing ProcessEventLog file1 \n");
		return;
	}

	//如果为空，则表示需要初始化
	if(lTraceLevel > pErrorLog->lTraceLevel)
	{
		return;
	}

	//basename用于只打印文件名

	//BaseFileName = basename(FileName);
	BaseFileName = FileName;

#ifdef WIN32
	time(&lTime)						;
	tm = *localtime(&lTime)			;
#else
	time(&lTime)						;
	localtime_r(&lTime,&tm)			;
#endif


	tm.tm_mon  += LOG_TM_MONTH_OFFSET;
	tm.tm_year += LOG_TM_YEAR_OFFSET;
/*
						if((tm.tm_mday != pErrorLog->tm.tm_mday)
								||
						   (tm.tm_mon != pErrorLog->tm.tm_mon)
								||
						   (tm.tm_year != pErrorLog->tm.tm_year))
						{
							CHAR cTempPath[LOG_MAX_PATH_LEN];
							CHAR cLogFile[LOG_MAX_PATH_LEN];
							CHAR cParamString[LOG_MAX_PATH_LEN];
							//待增加逻辑
							fclose(pErrorLog->pErrorLog);

							memset(cTempPath,NULL,sizeof(cTempPath));
							memset(cParamString,NULL,sizeof(cParamString));

							//增加保护
							CHAR * pLogPath = getenv("LOG_PATH");
							if(pLogPath != NULL)
								strcpy(cTempPath,pLogPath)		  	;
							else
								strcpy(cTempPath,"");
					#ifdef WIN32
							DWORD threadId = ::GetCurrentThreadId();
					#else
							pthread_t threadId = pthread_self();
					#endif
							strcpy(cParamString,"%s%s_%d")			;
									memset(cLogFile,NULL,sizeof(cLogFile))          ;
									sprintf(cLogFile,
											cParamString,
											cTempPath,
											pErrorLog->cLogFileName,threadId);

        						pErrorLog->pErrorLog = fopen(cLogFile,"a+") 	;
						}
	*/
	if(lErrorCode < 0)
	{
		strcpy(cErrorType,"F")			;
	}
	else if(lErrorCode > 0)
	{
		strcpy(cErrorType,"W")			;
	}
	else
	{
		strcpy(cErrorType,"I")			;
	}
	//增加日期
	strcpy(cLogParamString,"{%d-%d-%d") ;

        if(tm.tm_hour < 10)
        {
			strcat(cLogParamString,".0%d")             	;
        }
        else
       	{
                strcat(cLogParamString,".%d")              	;
        }

        if(tm.tm_min < 10)
        {
                strcat(cLogParamString,":0%d")             	;
        }
        else
        {
                strcat(cLogParamString,":%d")              	;
        }

	if(tm.tm_sec < 10)
        {
                strcat(cLogParamString,":0%d}")          	;
        }
        else
        {
                strcat(cLogParamString,":%d}")                 	;
        }

	//增加日期打印
	strcat(cLogParamString," {%s} {%s} {%d} {%s} [%ld] {") 		;
	//增加日期
	iStrLen = sprintf(cLogBuffer,
			  cLogParamString,
			  tm.tm_year,
			  tm.tm_mon,
			  tm.tm_mday,
			  tm.tm_hour,
			  tm.tm_min,
			  tm.tm_sec,
				pErrorLog->cStatement,
				cBaseErrType[lErrorType],
			  lErrorCode,
			  cErrorType,
				pErrorLog->MsgSeqNumber
				);

	va_start(vlErrorList,pArgList)					;

	iStrLen += vsprintf((cLogBuffer+iStrLen),pArgList,vlErrorList)	;

	va_end(vlErrorList)						;

	iStrLen += sprintf(cLogBuffer+iStrLen,"} [%s:%d]",BaseFileName,LineNo);

	cLogBuffer[iStrLen]='\n'					;
	cLogBuffer[iStrLen+1]=0;

	if (pErrorLog->pErrorLog != NULL)
	{
		fprintf(pErrorLog->pErrorLog,cLogBuffer)			;
		fflush(pErrorLog->pErrorLog)					;
		printf(cLogBuffer);
	}
	else{
		printf("Error in Writing ProcessEventLog file2 \n");
	}
	return;
}


//增加注销的方法
VOID ExitErrorLogT()
{
	_ERROR_LOG * pErrorLog = NULL;

	pErrorLog = (_ERROR_LOG *)pthread_getspecific(KET);

	//如果没有被初始化，则直接返回
	if(pErrorLog == NULL)
		return;

	//进行delete,并关闭文件。
	if (pErrorLog->pErrorLog != NULL)
		fclose(pErrorLog->pErrorLog);

	delete pErrorLog;
}
} // end namespace
