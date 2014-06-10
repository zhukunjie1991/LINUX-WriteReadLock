/**
�޸����ݣ�
1.ȥ������
2.��ͷ�ļ������Ƶ�.h�У�ȥ������Ҫ��ͷ�ļ�
3.��CHAR cBaseErrType�Ƶ�ͷ�ļ��С����Ӻ궨��
4.�ѱ��������Ƶ�ͷ�ļ���
5.����һ���������ε�
6.2012-2-17 �������ڵĴ�ӡ
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
**   DESCRIPTION       :  �˷�������                                         **
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

    //�˴�ʵ��������singletonģʽ�����ǵ����̵߳Ļ������ͨ��������һ������֤���ᷢ������
    //���û�г�ʼ���ֲ߳̾�����(bErrLogKeyInit!=!= TRUE),��TlsAlloc

    pthread_mutex_lock(&MET);
    if(!bErrLogKeyInit)
    {
        pthread_key_create(&KET,close_thread_log);
        bErrLogKeyInit = true;
    }
    pthread_mutex_unlock(&MET);

    pThreadErrorLog = (_ERROR_LOG *)malloc(sizeof(_ERROR_LOG));

    pthread_setspecific(KET,pThreadErrorLog);

    //��ʼ���ֲ߳̾�����
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

    //�����ļ��������ļ�
    memset(cTempPath,0,sizeof(cTempPath));
    //���ӱ���
    strcpy(cTempPath,"");
    //��filename������thread_name
    strcpy(cParamString,"%s%s");
    //��filename������threadid

    memset(cLogFile,0,sizeof(cLogFile));
    sprintf(cLogFile,
            cParamString,
            cTempPath,
            pThreadErrorLog->cLogFileName);

    //���������ڵ�Ŀ¼
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
//������ؿգ���ʾ���߳��ϵ�pErrorLogδ����ʼ�����˴���ʼ��
//����������ԭ����pErrorLog
if(pThreadErrorLog == NULL)
{

}
*/
    return;
}

/*******************************************************************************
* �������� : VProcessEventLog
* �������� : ��һ����־��¼����䵱ǰʱ�䡢�ļ������кš��������͵ȣ�
				������������־��¼ͨ����������
* �Ρ����� : const CHAR* FileName
* �Ρ����� : LONG32 LineNo
* �Ρ����� : LONG32 lTraceLevel
* �Ρ����� : LONG32 lErrorType
* �Ρ����� : LONG32 lErrorCode
* �Ρ����� : _ERROR_LOG** pErrorLog
* �Ρ����� : const CHAR* fmtList
* �Ρ����� : va_list argList
* �Ρ����� : CHAR* logBuf
* �� �� ֵ : VOID
* �������� : wujinghe
* ������� : 2012��9��17��
* �޸�����     �޸���    �޸�����
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
    //���Ϊ�գ����ʾ��Ҫ��ʼ��
    if (lTraceLevel > (*pErrorLog)->lTraceLevel) {
        return;
    }

    //basename����ֻ��ӡ�ļ���

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

    //��������
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

    //�������ڴ�ӡ
    strcat(cLogParamString, " {%s} {%s} {%d} {%s} [%ld] {");
    //��������
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
* �������� : ProcessEventLogToFile
* �������� : ��������Ϣ������־�ļ���
* �Ρ����� : const CHAR* logBuf
* �Ρ����� : const _ERROR_LOG* pErrorLog
* �Ρ����� : LONG32 lTraceLevel
* �� �� ֵ : VOID
* �������� : wujinghe
* ������� : 2012��9��17��
* �޸�����     �޸���    �޸�����
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
* �������� : ProcessEventLogToScr
* �������� : ��������Ϣ��ӡ����Ļ��
* �Ρ����� : const CHAR* logBuf
* �� �� ֵ : VOID
* �������� : wujinghe
* ������� : 2012��9��17��
* �޸�����     �޸���    �޸�����
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

	//���Ϊ�գ����ʾ��Ҫ��ʼ��
	if(lTraceLevel > pErrorLog->lTraceLevel)
	{
		return;
	}

	//basename����ֻ��ӡ�ļ���

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
							//�������߼�
							fclose(pErrorLog->pErrorLog);

							memset(cTempPath,NULL,sizeof(cTempPath));
							memset(cParamString,NULL,sizeof(cParamString));

							//���ӱ���
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
	//��������
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

	//�������ڴ�ӡ
	strcat(cLogParamString," {%s} {%s} {%d} {%s} [%ld] {") 		;
	//��������
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


//����ע���ķ���
VOID ExitErrorLogT()
{
	_ERROR_LOG * pErrorLog = NULL;

	pErrorLog = (_ERROR_LOG *)pthread_getspecific(KET);

	//���û�б���ʼ������ֱ�ӷ���
	if(pErrorLog == NULL)
		return;

	//����delete,���ر��ļ���
	if (pErrorLog->pErrorLog != NULL)
		fclose(pErrorLog->pErrorLog);

	delete pErrorLog;
}
} // end namespace
