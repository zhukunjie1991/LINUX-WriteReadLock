/*******************************************************************************
 * Copyright ? 2000-2010 China Foreign Exchange Trade System
 * & National Interbank Funding Center.All Rights Reserved.
 * 文件名称 : LoggerStream.h
 * 创建日期 : 2013-5-8
 * 说	 明 : 
 * 作	 者 : ZhanYi (yi.zhan@accenture.com) 
 * 修改记录 : 
 * 			日    期	修改人		修改摘要
 * 			2013-5-8	ZhanYi		Create
 ********************************************************************************/

#ifndef LOGGERSTREAM_H_
#define LOGGERSTREAM_H_

#include <string>
#include "logfile.h"
#include "Mutex.h"

using namespace std;

#define LOGGER_LEVEL_NONE  0
#define LOGGER_LEVEL_DEBUG 1
#define LOGGER_LEVEL_INFO  2
#define LOGGER_LEVEL_WARN  3
#define LOGGER_LEVEL_ERROR 4

#define LS_BUFFER_SIZE 4096
/*
 *
 */
namespace UTILITY
{
class CLogFile;
class LoggerStream
{
public:
	class ENDL
	{
		public:
		ENDL() {};
	};

public:

	LoggerStream();
	LoggerStream(int lv, CLogFile *_log = &g_LogFile);

	LoggerStream &operator<<(unsigned int i);
	LoggerStream &operator<<(int i);
	LoggerStream &operator<<(unsigned short i);
	LoggerStream &operator<<(short i);
	LoggerStream &operator<<(unsigned long i);
	LoggerStream &operator<<(long i);
	LoggerStream &operator<<(double i);
	LoggerStream &operator<<(float i);
	LoggerStream &operator<<(unsigned char i);
	LoggerStream &operator<<(char i);
	LoggerStream &operator<<(char *s);
	LoggerStream &operator<<(const string &s);
	LoggerStream &operator<<(string &s);
	LoggerStream &operator<<(const ENDL &e);

	LoggerStream &lock();

	void SetSource(const string &file, int line);

	void reset();
	void flush();

	virtual ~LoggerStream();
protected:
	int   level;
	CLogFile *log;

	string file;
	unsigned int line;
	char  buffer[LS_BUFFER_SIZE];
	CMUTEX mutex;
};

typedef LoggerStream CLoggerStream;

#ifdef _WIN32
# ifdef LOGGER_STREAM_EXPORT
#  define LOGGER_STREAM_API __declspec(dllexport)
# else
#  define LOGGER_STREAM_API __declspec(dllimport)
# endif
#else
# define LOGGER_STREAM_API extern
#endif

extern LoggerStream _err_ss;
extern LoggerStream _warn_ss;
extern LoggerStream _info_ss;
extern LoggerStream _debug_ss;
extern LoggerStream _none_ss;

static const LoggerStream::ENDL endl;

LOGGER_STREAM_API void InitLoggerStream(UTILITY::CLogFile *log);

} /* namespace UTILITY */


#define ERROR_LOGGER \
	UTILITY::_err_ss.flush(); \
	UTILITY::_err_ss.lock(); \
	UTILITY::_err_ss.SetSource(__FILE__, __LINE__); \
	UTILITY::_err_ss

#define WARN_LOGGER  \
	UTILITY::_warn_ss.flush(); \
	UTILITY::_warn_ss.lock(); \
	UTILITY::_warn_ss.SetSource(__FILE__, __LINE__); \
	UTILITY::_warn_ss

#define INFO_LOGGER  \
	UTILITY::_info_ss.flush(); \
	UTILITY::_info_ss.lock(); \
	UTILITY::_info_ss.SetSource(__FILE__, __LINE__); \
	UTILITY::_info_ss

#define DEBUG_LOGGER \
	UTILITY::_debug_ss.flush(); \
	UTILITY::_debug_ss.lock(); \
	UTILITY::_debug_ss.SetSource(__FILE__, __LINE__); \
	UTILITY::_debug_ss

#define NONE_LOGGER \
	UTILITY::_none_ss.flush(); \
	UTILITY::_none_ss.lock(); \
	UTILITY::_none_ss.SetSource(__FILE__, __LINE__); \
	UTILITY::_none_ss

#define LOGSTREAM_INIT() UTILITY::InitLoggerStream(&(UTILITY::g_LogFile))


#endif /* LOGGERSTREAM_H_ */
