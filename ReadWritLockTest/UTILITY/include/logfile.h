/********************************************************************
	created:	2004/02/13
	created:	13:2:2004   10:16
	filename: 	E:\proj\goldmem\ksbcc\logfile.h
	file path:	E:\proj\goldmem\ksbcc
	file base:	LogFile
	file ext:	h
	author:		CHENYH
	
	purpose:	
      20040405��
      ���ı���־�ļ�����Ϊ����ÿ��һ����־�ļ����Ա��ѯ�ȴ���
*********************************************************************/
// logfile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string>
#include "PubFunc.h"
#include "Mutex.h"

#ifdef WIN32

#include <process.h>
#include <direct.h>
#include <windows.h>
#include <io.h>
#else

#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/utsname.h>

#endif



typedef void (*FUNC_LOGSHOW)(const char *logtxt);

#ifndef BUDLL
#define BUPUB_API 
#else
#ifdef BUPUB_EXPORTS
#define BUPUB_API __declspec(dllexport)
#else
#ifdef _MSC_VER
#define BUPUB_API __declspec(dllimport)
#else
#define BUPUB_API 
#endif
#endif
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define LOG_FILE_PATH_LEN__ 1024
#define LOG_FILE_NAME_LEN__ 256
//�����־�г���
#define MAX_LOG_LINE_SIZE__ 1500

//20130922ע��
//#define _STD_OUT__
using namespace std;

//kikin add ���� UTILITY
namespace UTILITY
{
	static const int SUCCESS = 0;
	class BUPUB_API CLogFile  
	{
	private:
		FILE *fp;      // ��־�ļ����
		char logpath[LOG_FILE_PATH_LEN__];   // ��־�ļ����·��
		int  logdate;        // ��ǰ��־�ļ�����
		time_t lastftime;
        //��һ�ζ�ȡ����������ʱ��
		time_t m_iLastReadEnvTime;
		int flashdelay;      // ˢ����ʱ, ��λ��
		//����־���𻷾��������� 2013-09-22
		int m_iReadEnvInterval;
		int mode;            // 0:����PID��1:��PID
		int m_FileNumber;  // CYH Added for controlling file length
		int m_fileLen;
		char m_fileName[LOG_FILE_NAME_LEN__];

		int m_writeLevel;    // 2007-10-31 17:28:54 for disable writing
		int m_iShowLevel;    //2013-09-23 ��־��ʾ����
		int m_timeShift;

		bool m_initFlag;

		int m_iLogExpiredDay; //��־��������

		FUNC_LOGSHOW pLogShow;

		CMUTEX m_mutex;   // ������

		char *m_buffer;  // ��־������
		int m_bufferSize; // ��������С

	private:
		bool open();
		void close();
		bool RollingLog();
		//��ס������־���ļ����к�
		void SaveLogTrace(const char *szFile, int iLine, bool bStart);
		bool RemoveExpiredFile(int iExpiredDay, const char *szDir);
		bool DeleteExpiredLog(void);

	public:
		void SetLogShowFunc(FUNC_LOGSHOW pShowFunc);  // ��Ϊͼ�ν����ʱ�򣬿���ͨ������logshowlevel�������ϵ���־��Ϣ��ʾ��ָ��������
		void SetWriteLevel(int level);
		void ToFlush();
		int RemoveOverdueLogFile(int overdue=30);
		bool Open(char *szLogPath=NULL,int LogFileMode=0);
		void WriteLogEx(int level, int errorcode, const char* __file__, int __line__, const char *pFormat, ...);
		void SetFlushDelay(int iFlushDelayInSecond);
		void Close();
		CLogFile(char *szLogPath=NULL,int fdelay=1);
		virtual ~CLogFile();

 		bool Init(const string &appName, int fileSize, int level=2, int timeShift=1);
		bool InitCfg(const string &fileName);

		//������ 2013-09-22 added
        int GetWriteLevel(void);
		int GetShowLevel(void);
		void SetShowLevel(int iShowLevel);
	};

	BUPUB_API extern CLogFile g_LogFile;
	
	// 
	inline bool StdPrintLog(bool flag)
	{
		return false;
	}

	// �Ӵ�����Ϣ������ȡ�ô�����
	inline int GetErrorCode(int code,  int act, const char * msg)
	{
	    return code;
	}

	// �Ӵ�����Ϣ������ȡ�ô�������
	inline const char* GetErrorMsg(int code, int act, const char * msg)
	{
	    return msg;
	}

	// �Ӵ�����Ϣ������ȡ�ô�����ʽ
	inline int GetErrorAction(int code, int act, const char * msg)
	{
	    return act;
	}

	// ������
	inline void RaiseError(const char* srcFile,
	                int line,
	                int level,
	                int type,
	                int code,
	                int act,
	                const char* msg,
			...)
	{
	    return;
	}

	BUPUB_API void SetLogShowLevel(int logshowlevel);
};

//�ŵ�namespace����--2013-09-22  ��CQ
//BUPUB_API void SetLogShowLevel(int logshowlevel);

#undef BUPUB_API

//in vc6, compiler cannot support variable parameter macro.
#if defined(_MSC_VER) && (_MSC_VER <= 1200)
#define DEBUG_MSG printf
#define WARNING_MSG printf
#define FATAL_MSG printf
#define INFO_MSG  printf
#define ERROR_MSG printf

#else
#define LOG_INIT UTILITY::g_LogFile.Init
#define LOG_INITCFG UTILITY::g_LogFile.InitCfg

// ���ڷ��ش������ĺ�
#define ERROR_CODE(ERR) UTILITY::GetErrorCode(ERR)
// ���ڷ��ش������ĺ�
#define ERROR_MSG(ERR) UTILITY::GetErrorMsg(ERR)
// ����������ĺ�
///tbd ��Ҫʵ��
#define RAISE_ERROR(ERR, ...) UTILITY::RaiseError(__FILE__,__LINE__,0, 0, ERR, ##__VA_ARGS__)

#define STD_PRINT_LOG_ON	UTILITY::g_LogFile.StdPrintLog(true)
#define STD_PRINT_LOG_OFF	UTILITY::g_LogFile.StdPrintLog(false)

//�ں�������ǰ���Ӽ����ж�
#if 0

#define LOG_DEBUG(fmt,...) UTILITY::g_LogFile.WriteLogEx(1, 0,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt,...) UTILITY::g_LogFile.WriteLogEx(2, 0, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt,...) UTILITY::g_LogFile.WriteLogEx(3, 0, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(code,fmt,...) UTILITY::g_LogFile.WriteLogEx(4, code, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#else

#define LOG_DEBUG(fmt,...) if(1 >= UTILITY::g_LogFile.GetWriteLevel()) \
	UTILITY::g_LogFile.WriteLogEx(1, 0,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt,...) if(2 >= UTILITY::g_LogFile.GetWriteLevel()) \
	UTILITY::g_LogFile.WriteLogEx(2, 0, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt,...) if(3 >= UTILITY::g_LogFile.GetWriteLevel()) \
	UTILITY::g_LogFile.WriteLogEx(3, 0, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(code,fmt,...) if(4 >= UTILITY::g_LogFile.GetWriteLevel()) \
	UTILITY::g_LogFile.WriteLogEx(4, code, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif

#endif

//#include "logfile.hpp"

