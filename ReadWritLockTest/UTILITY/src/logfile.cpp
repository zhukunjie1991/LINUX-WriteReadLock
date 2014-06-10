// LogFile.cpp: implementation of the CLogFile class.
//
// 2007-7-4 15:50:19 为了控制日志文件大小不超过系统文件大小限制，特别设置为
//						文件不超过2G，并作相应的修改
// 修改编号   修改人	  修改日期	  修改原因
//		  1   ZhanYi	  2013/7/29   修改备份文件名方法，使用rename函数
//									  替换原有的mv。
//////////////////////////////////////////////////////////////////////


#include <string.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include "ConfigFile.h"

#include "LoggerStream.h"
#include "logfile.h"
#include "PubFunc.h"

using namespace std;

#ifdef WIN32
#define PATHMARK  '\\'
#define MAKEPATH(a) mkdir(a)
#else

#define PATHMARK  '/'
#define MAKEPATH(a) mkdir(a,0777)

#endif

//到时候放到配置文件 -- 用类成员替换全局变量
//int g_ShowLogLevel=10000;

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

// 64K
#define _DEFAULT_BUFFER_SIZE 65536
#define _MIN_BUFFER_SIZE 100
#define _TRUN_STRING "(More...)"

namespace UTILITY
{

	CLogFile g_LogFile;


	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	CLogFile::CLogFile(char *szLogPath,int fdelay)
	{
		if (szLogPath!=NULL && szLogPath[0]!='\0')
		{
			strncpy(logpath,szLogPath,sizeof(logpath));
		}
		else
		{
			strcpy(logpath,".");  // 缺省为当前目录下
		}
		logpath[sizeof(logpath)-1]='\0';

		m_initFlag = false;
		fp = NULL;
		lastftime = 0;
		m_iLastReadEnvTime = 0;
		flashdelay = fdelay;
		logdate = 0;
		mode = 0;
		m_FileNumber = 0;
		m_writeLevel = 1;
		//g_ShowLogLevel = 1;
		m_iShowLevel = 1;
		m_iReadEnvInterval = 5;

		m_iLogExpiredDay = 0;

		m_bufferSize = _DEFAULT_BUFFER_SIZE;
		m_buffer = NULL;
		SetLogShowFunc(NULL);
	}

	CLogFile::~CLogFile()
	{
		DeleteExpiredLog();
		if(fp)
		{
			fflush(fp);
		}
	#ifdef SYSLOG
		closelog();
	#endif 
		Close();

		if (m_buffer != NULL)
		{
			free(m_buffer);
			m_buffer = NULL;
		}
	}

	bool CLogFile::RemoveExpiredFile(int iExpiredDay, const char *szDir)
	{
		char            *szCurrFile;
		char            acFile[512] = {0};
		char            acTmpBuf[32];
		bool            bRet = false;
		struct tm       stLocalTime = {0};
		time_t          stCurTime = time(NULL);

#ifdef WIN32
		WIN32_FIND_DATA stFindData;
		HANDLE          hfind;
		char            acSearch[512];
#else
		DIR             *dp;
		struct dirent   *dirp;
		struct stat     statbuf;
#endif
		if (NULL == szDir)
		{
			LOG_ERROR(-1, "CLogFile::RemoveExpiredFile() null pointer exception");
			return false;
		}

#ifdef WIN32
		localtime_s(&stLocalTime, &stCurTime);
		sprintf(acSearch, "%s\\*.*", szDir);
		hfind = FindFirstFile(acSearch, &stFindData);
		if(INVALID_HANDLE_VALUE == hfind)
		{
			return false;
		}
#else
		localtime_r (&stCurTime, &stLocalTime);
		dp = opendir(szDir);
		if (NULL == dp)
		{
			return false;
		}

		dirp = readdir(dp);
		if (NULL == dirp)
		{
			closedir(dp);
			return false;
		}
#endif
		do
		{
#ifdef WIN32
			szCurrFile = stFindData.cFileName;
#else
			szCurrFile = dirp->d_name;
#endif

			if(0 == strcmp(szCurrFile, ".") || 0 == strcmp(szCurrFile, ".."))
			{
				continue;
			}
			sprintf(acFile, "%s/%s", szDir, szCurrFile);

#ifdef WIN32
			if((stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
#else
			if(lstat(acFile, &statbuf) < 0)
			{
				break;
			}

			if(S_ISDIR(statbuf.st_mode))
#endif
			{
				bRet = RemoveExpiredFile(iExpiredDay, acFile);
				if(false == bRet)
				{
					LOG_ERROR(-1, "CLogFile::GetExpiredFile() get expired file from dir[%s] failed", acFile);
					break;
				}
			}
			else
			{
				struct tm stTmpLocalTime;
				time_t    stFileTime;
				time_t    stTodayStartTime;
				char      *szCur = strstr(szCurrFile, ".log.20");
				int       i = 0;

				if(NULL == szCur)
				{
					continue;
				}
				szCur += 5;
				memset(&stTmpLocalTime, 0, sizeof(stTmpLocalTime));

				//获取年份
				char *szBufCur = acTmpBuf;
				for(i = 0; i < 4 && *szCur != '\0'; i++)
				{
					*szBufCur++ = *szCur++;
				}
				*szBufCur = 0;
				stTmpLocalTime.tm_year = atoi(acTmpBuf)-1900;

				//获取月份
				szBufCur = acTmpBuf;
				for(i = 0; i < 2 && *szCur != '\0'; i++)
				{
					*szBufCur++ = *szCur++;
				}
				*szBufCur = 0;
				stTmpLocalTime.tm_mon = atoi(acTmpBuf)-1;

				//获取日期
				szBufCur = acTmpBuf;
				for(i = 0; i < 2 && *szCur != '\0'; i++)
				{
					*szBufCur++ = *szCur++;
				}
				*szBufCur = 0;
				stTmpLocalTime.tm_mday = atoi(acTmpBuf);
				stFileTime = mktime(&stTmpLocalTime);

				memset(&stTmpLocalTime, 0, sizeof(stTmpLocalTime));
				stTmpLocalTime.tm_year = stLocalTime.tm_year;
				stTmpLocalTime.tm_mon = stLocalTime.tm_mon;
				stTmpLocalTime.tm_mday = stLocalTime.tm_mday;
				stTodayStartTime = mktime(&stTmpLocalTime);

				if(stTodayStartTime > stFileTime && 
					stTodayStartTime - stFileTime > iExpiredDay*24*3600)
				{
					int iTmpRet = remove(acFile);
					if(0 != iTmpRet)
					{
						LOG_ERROR(1, "CLogFile::RemoveExpiredFile() Delete file[%s] failed", acFile);
					}
					LOG_INFO("CLogFile::RemoveExpiredFile() LogFile[%s] has been removed", acFile);
				}
			}
#ifdef WIN32
		}while(FindNextFile(hfind, &stFindData));
		FindClose(hfind);
#else
		}while((dirp = readdir(dp)) != NULL);
		closedir(dp);
#endif
        return true;
	}

	//删除超过规定期限的日志文件
	bool CLogFile::DeleteExpiredLog(void)
	{
		//如果超限日期 <= 0, 不做超限检查
		if(m_iLogExpiredDay <= 0)
		{
			return true;
		}

		return RemoveExpiredFile(m_iLogExpiredDay, logpath);
	}
	
	bool CLogFile::InitCfg(const string &fileName)
	{
		if(m_initFlag)
		{
			return true;
		}

		if (fileName.length() <= 0)
		{
			printf("Log init fileName is NULL\n");
			return false;
		}
		
		ConfigFile configFile;
		if (!configFile.Load(fileName)) 
		{
			printf("Log init fileName is NULL\n");
			return false;
		}

		string appName = configFile.ReadString("default", "Log.AppName", "");
		if (appName == "") 
		{
			printf("Log config Log.AppName NULL,Set Log.AppName = DefaultAppName.\n");
			//hauk modified at 2013-11-08. 
			//如果没有配置进程名，先获取进程名，获取失败后再用默认的名字
			//appName = "DefaultAppName";
			appName = UTILITY::GetProcName();
			if(appName.length() == 0)
			{
				appName = "DefaultAppName";
			}
		}

		m_fileLen = configFile.ReadInt("default", "Log.MaxFileSize", 0);
		if (m_fileLen <= 0) 
		{
			printf("Log config Log.MaxFileSize NULL,Set Default = 200MB\n");
			m_fileLen = 200;
		}

		m_timeShift = configFile.ReadInt("default", "Log.TimeShift", 0);

		m_writeLevel = configFile.ReadInt("default", "Log.PrintLevel", 0);
		if (m_writeLevel <= 0) 
		{
			printf("Log config Log.PrintLevel NULL,Set Default = 2\n");
			m_writeLevel = 2;
		}

		m_iShowLevel = configFile.ReadInt("default", "Log.ShowLevel", 0);
		if (m_iShowLevel <= 0) 
		{
			printf("Log config Log.ShowLevel NULL, Set Default = %d\n", m_writeLevel);
			m_iShowLevel = m_writeLevel;
		}

		m_iReadEnvInterval = configFile.ReadInt("default", "Log.ReadEnvInterval", 0);
		if (m_iReadEnvInterval <= 0) 
		{
			printf("Log config Log.ShowLevel NULL, Set Default = %d\n", m_writeLevel);
			m_iReadEnvInterval = 5;
		}

		/* 增加Log.Path参数, 默认为"" */
		string path = configFile.ReadString("default", "Log.Path", "");
		strncpy(logpath, path.c_str(), sizeof(logpath));
		logpath[sizeof(logpath) - 1] = 0;

		m_bufferSize = configFile.ReadInt("default", "Log.BufferSize", _DEFAULT_BUFFER_SIZE);
		if (m_bufferSize < _MIN_BUFFER_SIZE)
		{
			m_bufferSize = _DEFAULT_BUFFER_SIZE;
		}

		m_iLogExpiredDay = configFile.ReadInt("default", "Log.ExpiredDay", 0);
		if(m_iLogExpiredDay < 0)
		{
			m_iLogExpiredDay = 0;
		}

		if (!Init(appName, m_fileLen, m_writeLevel, m_timeShift))
		{
			printf("Log init error\n");
			return false;
		}
		return true;
	}

	bool CLogFile::Init(const string &appName, int fileSize, int level, int timeShift)
	{
		if(m_initFlag)
		{
			return true;
		}

		//1. check appName para
		if (appName.length() <= 0)
		{
			printf("Log init appName is NULL\n");
		}

		//2. check fileSize para
		if (fileSize <= 0)
		{
			printf("Log init fileSize is zero\n");
			return false;
		}
		
		//3. construct logpath and filename
#ifndef WIN32
		if (logpath[0] == 0)
		{
			char * path = getenv("HOME");
			snprintf(logpath, sizeof(logpath), "%s/log/", path);
		}	

		struct utsname uts;
		if (0 > uname(&uts))
		{
			cout << "get host name error\n";
			return false;
		}
		snprintf(m_fileName, sizeof(m_fileName), "%s.%s.%s.log",\
			uts.nodename, appName.c_str(), (char*)UTILITY::GetProcName());
#else
		if (logpath[0] == 0)
		{
			strcpy(logpath,".\\log\\");
		}
		sprintf(m_fileName, "%s.log", appName.c_str());
#endif
		cout << "fileName:" << m_fileName << std::endl;
		logpath[sizeof(logpath)-1]='\0';

		//4. calculate file size
		m_fileLen = fileSize * 1000 * 1000;

		//5. set print log level and time range
		m_writeLevel = level;
		m_timeShift = timeShift;

		//6. open log file
		if (!Open((char *)logpath, 0))
		{
			printf("Init log error\n");
			return false;
		}
		g_LogFile = *this;

		m_initFlag = true;

		// Open logger stream.
		LOGSTREAM_INIT();

		// Get the buffer.
		m_buffer = (char *)malloc(m_bufferSize);
		if (m_buffer == NULL)
		{
			printf("Error: Out of memory.\n");
			return false;
		}

		return true;
	}


	void CLogFile::Close()
	{
	   AutoLock amutex(&m_mutex);
	   fflush(fp);
	   close();
	}

	bool CLogFile::Open(char *szLogPath, int LogFileMode)
	{
	   if (szLogPath!=NULL && szLogPath[0]!='\0')
		  strncpy(logpath,szLogPath,sizeof(logpath));
	   logpath[sizeof(logpath)-1]='\0';
	   mode = LogFileMode;
	   return(open());
	}


	void CLogFile::SetFlushDelay(int iFlushDelayInSecond)
	{
	   flashdelay = iFlushDelayInSecond;
	}

    #define LOG_SIZE         (8*1024)	//日志大小最大为8KB
    #define PATH_LINE_SIZE   512
    #define LOG_LEVE_ENV     "LOG_LEVEL"

	const char *g_aacLevelName[5] = 
	{
		"",
		"DEBUG",
		"INFO",
		"WARN",
		"ERROR"
	};

	void CLogFile::SaveLogTrace(const char *szFile, int iLine, bool bStart)
	{
		static FILE *fp = NULL;

		if(NULL == fp)
		{
			char acTracePath[256];
			if(strlen(logpath) > 0)
			{
				if(logpath[strlen(logpath)-1] == '\\' || logpath[strlen(logpath)-1] == '/')
				{
					logpath[strlen(logpath)-1] = '\0';
				}
				Snprintf(acTracePath, sizeof(acTracePath), "%s/LogTrace.log", logpath);
			}
			else
			{
				sprintf(acTracePath, "./LogTrace.log");
			}

			fp = fopen(acTracePath, "w+");
            if(NULL == fp)
			{
				return;
			}
		}

		fprintf(fp, "%s: file[%s] line[%d]\n", bStart? "Enter": "Exit", szFile, iLine);
		fflush(fp);
	}


	void CLogFile::WriteLogEx(int level, int errorcode, const char* __file__, int __line__, const char *pFormat, ...)
	{
		if (level < m_writeLevel) return;

#ifdef LOG_SAVE_TRACE
		m_mutex.Lock();
		SaveLogTrace(__file__, __line__, true);
		m_mutex.UnLock();
#endif

		va_list     pArg;
		char        acLog[LOG_SIZE + PATH_LINE_SIZE];
		int         iLogLen = 0;
		char        *szTmp ;
		struct      tm tmloc;
		time_t      curtime;
		
		if ((szTmp = (char *)strrchr(__file__, PATHMARK)) != NULL)
		{
			__file__ = szTmp + 1;
		}

		if (!m_initFlag)
		{
			char _tmpBuf[MAX_LOG_LINE_SIZE__] = {0};
			szTmp = _tmpBuf;
			printf("Log system not init![%s:%d]\n", __file__, __line__);
			va_start(pArg,pFormat);
			int n = Vsnprintf(szTmp, sizeof(_tmpBuf), pFormat, pArg);
			va_end(pArg);

			if (n >= sizeof(_tmpBuf))
			{
				_tmpBuf[sizeof(_tmpBuf) - 1] = 0;
			}
			printf("%s[%s:%d]\n",szTmp, __file__ ,__line__);
#ifdef LOG_SAVE_TRACE
			m_mutex.Lock();
			SaveLogTrace(__file__, __line__, false);
			m_mutex.UnLock();
#endif
			return;
		}

		time(&curtime);

#ifdef WIN32
		//localtime线程不安全
		m_mutex.Lock();
		memcpy(&tmloc,localtime(&curtime),sizeof(tmloc));
		m_mutex.UnLock();
#else
        localtime_r(&curtime, &tmloc);
#endif

		struct timeval CurrentTime;
		gettimeofday(&CurrentTime, NULL);

		if(level > 0 && level < 5)
		{
            iLogLen += sprintf(acLog, "%04d-%02d-%02d %02d:%02d:%02d:%06ld||%s||%d||%d||%lu||", 
				tmloc.tm_year + 1900, tmloc.tm_mon+1, tmloc.tm_mday,
				tmloc.tm_hour, tmloc.tm_min, tmloc.tm_sec, CurrentTime.tv_usec, 
				g_aacLevelName[level], errorcode,
				getpid(), GetTid());
		}
		else
		{
			iLogLen += sprintf(acLog, "%04d-%02d-%02d %02d:%02d:%02d:%06ld||%d||%d||%d||%lu||", 
				tmloc.tm_year + 1900, tmloc.tm_mon+1, tmloc.tm_mday,
				tmloc.tm_hour, tmloc.tm_min, tmloc.tm_sec, CurrentTime.tv_usec, 
				level, errorcode,
				getpid(), GetTid());
		}
		
		va_start(pArg,pFormat);
		int writableSize = LOG_SIZE - iLogLen;
		int n = Vsnprintf(acLog + iLogLen, writableSize, pFormat, pArg);
		va_end(pArg);

		if (n > writableSize)
		{
			memcpy(acLog + LOG_SIZE - sizeof(_TRUN_STRING), _TRUN_STRING, sizeof(_TRUN_STRING));
			iLogLen = LOG_SIZE;
		}
		else if (n < 0)
		{
		    printf("ERROR: write the log line to buffer failed.\n");
#ifdef LOG_SAVE_TRACE
			m_mutex.Lock();
			SaveLogTrace(__file__, __line__, false);
			m_mutex.UnLock();
#endif
		    return;
		}
		else
		{
            iLogLen += n;
		}

#ifdef WIN32
		int iLen = _snprintf_s(acLog+iLogLen, PATH_LINE_SIZE, PATH_LINE_SIZE-1, "[%s:%d]\n", __file__, __line__);
#else
		int iLen = snprintf(acLog+iLogLen, PATH_LINE_SIZE, "[%s:%d]\n", __file__, __line__);
#endif
		if(iLen > 0 && iLen < PATH_LINE_SIZE)
		{
			iLogLen += iLen;
		}

		//上锁
		m_mutex.Lock();

		if ((m_timeShift != 0) && (logdate!=((tmloc.tm_year+1900)*100+tmloc.tm_mon+1)*100+tmloc.tm_mday) && (logdate != 0))
		{
			printf("[%d]write logdate time:%d,\n", __LINE__,logdate);
			m_FileNumber = 0;
			RollingLog();
		}

		size_t tmpFileSize = 0;

		struct stat buf;
		char tmp_name[LOG_FILE_NAME_LEN__];
		sprintf(tmp_name,"%s/%s", logpath, m_fileName);
		stat(tmp_name,&buf);
		tmpFileSize = buf.st_size;

		if ((int)tmpFileSize > m_fileLen)
		{
			printf("size:%d,req:%d\n", (int)tmpFileSize, m_fileLen);
			RollingLog();
		}

		if (fp)
		{
			int iWlen = fwrite(acLog, 1, iLogLen, fp);
			if(iWlen != iLogLen)
			{
				printf("Utility::Write log length erro[%d_%d]\n", iWlen, iLogLen);
			}

			if (curtime - lastftime > flashdelay)
			{
				fflush(fp);
				lastftime = curtime;
			}

			//不间断第读取环境变量，查看日志级别是否被修改
			if(curtime - m_iLastReadEnvTime > m_iReadEnvInterval)
			{
				m_iLastReadEnvTime = curtime;

				//读取环境变量
			#ifndef WIN32
				char *szLevel = getenv(LOG_LEVE_ENV);
				if(szLevel && strlen(szLevel) > 0 && atoi(szLevel) >= 0)
				{
					m_writeLevel = atoi(szLevel);
				}
			#endif
			}
		}
		else
		{
			printf("[%d]fp is NULL\n", __LINE__);
		}

		if (level >= m_iShowLevel)
		{
			if (pLogShow) (*pLogShow)(acLog);
			else printf("%s", acLog);
		}

#ifdef LOG_SAVE_TRACE
		SaveLogTrace(__file__, __line__, false);
#endif

		//解锁
		m_mutex.UnLock();
	}

	void CLogFile::close()
	{
	   if (fp != NULL)
	   {
		  fflush(fp);
		  fclose(fp);
	   }
	   fp = NULL;
	   lastftime = time(NULL);
	}

	bool CLogFile::open()
	{
		AutoLock  amutex = AutoLock(&m_mutex);
	    int       shflag;
	    int       len;
	    struct tm tmloc;
	    
		close();

		//去掉文件夹路径末尾的文件分级符
	    len = strlen(logpath);
	    if (len > 0 && ('\\' == logpath[len-1] || '/' == logpath[len-1]))
		{
		   logpath[len-1] = '\0';
		}

	    time(&lastftime);
	    time(&m_iLastReadEnvTime);
	    memcpy(&tmloc, localtime(&lastftime), sizeof(tmloc));
	   
		//如果日志文件夹不存在，创建文件夹
		if (access(logpath, 0) != 0)
		{
			int ret = MAKEPATH(logpath);
			printf("[%d]ret:%d,logPath:%s\n", __LINE__, ret, logpath);
		}
		else
		{
			printf("[%d]exit logPath:%s\n", __LINE__, logpath);
		}

		struct stat buf;
		char tmp_name[LOG_FILE_NAME_LEN__];
		sprintf(tmp_name,"%s/%s", logpath, m_fileName);

		stat(tmp_name, &buf);
		struct tm *t = localtime(&(buf.st_mtime));
		if (-1 == (access( tmp_name, 0 )))
		{
			printf("logfile.cpp[%d] file[%s] not exsit\n", __LINE__, tmp_name);
		}
		else if ((buf.st_size != 0))
		{
			logdate = ((t->tm_year+1900)*100+t->tm_mon+1)*100+t->tm_mday;
			
			printf("[%d]size:%ld,now:%d,file:%d\n", __LINE__,buf.st_size,tmloc.tm_mday,t->tm_mday);
			if (tmloc.tm_mday != t->tm_mday)
			{
				//added by hauk at 2013-12-05
				//NTSTR00002668
				logdate = ((tmloc.tm_year+1900)*100+tmloc.tm_mon+1)*100+tmloc.tm_mday;
				//endof addition

				char new_name[LOG_FILE_PATH_LEN__];
				sprintf(new_name, "%s.%04d%02d%02d%02d%02d", 
					tmp_name, 
					t->tm_year+1900,
					t->tm_mon+1,
					t->tm_mday,
					t->tm_hour,
					t->tm_min);

				rename(tmp_name, new_name);
			}
			else if (buf.st_size > m_fileLen)
			{
				char new_name[LOG_FILE_PATH_LEN__];
				sprintf(new_name, "%s.%04d%02d%02d%02d%02d", 
					tmp_name, 
					t->tm_year+1900,
					t->tm_mon+1,
					t->tm_mday,
					t->tm_hour,
					t->tm_min);

				rename(tmp_name, new_name);
			}
		}
		else
		{
			logdate = ((tmloc.tm_year+1900)*100+tmloc.tm_mon+1)*100+tmloc.tm_mday;
		}

	   shflag = SH_DENYNO;
	   printf("[%d]pwd:%s\n", __LINE__,tmp_name);
	   fp = sh_fopen(tmp_name,"a+t",shflag);
	   if (fp==NULL)
	   {
		   fp = sh_fopen(tmp_name,"w+t",shflag);
	   }
	   
	   return(fp!=NULL);
	}

	bool CLogFile::RollingLog()
	{
		int shflag;
		if (access(logpath,0) != 0)
		{
			MAKEPATH(logpath);
		}
		
		time_t temTime = lastftime;
		close();
		lastftime = temTime;
		
		char tmp_name[LOG_FILE_NAME_LEN__];
		sprintf(tmp_name,"%s/%s", logpath, m_fileName);

		struct stat buf;

		stat(tmp_name,&buf);
		struct tm *t = localtime(&(buf.st_mtime));

		char new_name[LOG_FILE_PATH_LEN__];
		sprintf(new_name, "%s.%04d%02d%02d%02d%02d", 
			tmp_name, 
			t->tm_year+1900,
			t->tm_mon+1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min);

		rename(tmp_name, new_name);

		shflag = SH_DENYNO;
		printf("[%d]pwd:%s\n", __LINE__,tmp_name);
		fp = sh_fopen(tmp_name,"a+t",shflag);
		if (fp==NULL)
		{
			fp = sh_fopen(tmp_name,"w+t",shflag);
		}
		logdate = ((t->tm_year+1900)*100+t->tm_mon+1)*100+t->tm_mday;
		return(fp!=NULL);
	}

	void SetLogShowLevel(int logshowlevel)
	{
	   //g_ShowLogLevel = logshowlevel;
		g_LogFile.SetShowLevel(logshowlevel);
	}

	void CLogFile::SetWriteLevel(int level)
	{
	   m_writeLevel = level;
	}

	int CLogFile::GetWriteLevel(void)
	{
		return m_writeLevel;
	}

	void CLogFile::SetShowLevel(int level)
	{
		m_iShowLevel = level;
	}

	int CLogFile::GetShowLevel(void)
	{
		return m_iShowLevel;
	}

	void CLogFile::SetLogShowFunc(FUNC_LOGSHOW pShowFunc)
	{
	   pLogShow = pShowFunc;
	}

	// 从错误信息定义中取得错误码
	
};
