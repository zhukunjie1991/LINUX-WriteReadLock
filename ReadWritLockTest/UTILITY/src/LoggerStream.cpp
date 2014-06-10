/*******************************************************************************
 * Copyright ? 2000-2010 China Foreign Exchange Trade System
 * & National Interbank Funding Center.All Rights Reserved.
 * 文件名称 : LoggerStream.cpp
 * 创建日期 : 2013-5-8
 * 说	 明 : 
 * 作	 者 : ZhanYi (yi.zhan@accenture.com) 
 * 修改记录 : 
 * 			日    期	版    本	修改人		修改摘要
 * 			2013-5-8	v1.0.1	ZhanYi		Create
 ********************************************************************************/

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "LoggerStream.h"
#include "logfile.h"

namespace UTILITY
{

LoggerStream _err_ss;
LoggerStream _warn_ss;
LoggerStream _info_ss;
LoggerStream _debug_ss;
LoggerStream _none_ss;

LoggerStream::LoggerStream()
{
	level = LOGGER_LEVEL_INFO;
	buffer[0] = 0;
	log = &g_LogFile;
	line = 0;
	file = "";
}

LoggerStream::LoggerStream(int lv,  CLogFile *_log)
{
	level = lv;
	buffer[0] = 0;
	log = _log;

	line = 0;
	file = "";
}

LoggerStream& LoggerStream::operator <<(unsigned int i)
{
	sprintf(buffer, "%s%u", buffer, i);
	return *this;
}

LoggerStream& LoggerStream::operator <<(int i)
{
	sprintf(buffer, "%s%d", buffer, i);
	return *this;
}

LoggerStream& LoggerStream::operator <<(unsigned short i)
{
	sprintf(buffer, "%s%u", buffer, i);
	return *this;
}

LoggerStream& LoggerStream::operator <<(short i)
{
	sprintf(buffer, "%s%d", buffer, i);
	return *this;
}

LoggerStream& LoggerStream::operator <<(unsigned long i)
{
	sprintf(buffer, "%s%lu", buffer, i);
	return *this;
}

LoggerStream& LoggerStream::operator <<(long i)
{
	sprintf(buffer, "%s%ld", buffer, i);
	return *this;
}

LoggerStream& LoggerStream::operator <<(double i)
{
	sprintf(buffer, "%s%.6lf", buffer, i);
	return *this;
}


LoggerStream& LoggerStream::operator <<(float i)
{
	sprintf(buffer, "%s%.4f", buffer, i);
	return *this;
}

LoggerStream& LoggerStream::operator <<(unsigned char i)
{
	sprintf(buffer, "%s%c", buffer, i);
	return *this;
}

LoggerStream& LoggerStream::operator <<(char i)
{
	sprintf(buffer, "%s%c", buffer, i);
	return *this;
}

LoggerStream& LoggerStream::operator <<(char* s)
{
	sprintf(buffer, "%s%s", buffer, s);
	return *this;
}

LoggerStream& LoggerStream::operator <<(const string &s)
{
	sprintf(buffer, "%s%s", buffer, s.c_str());
	return *this;
}


LoggerStream& LoggerStream::operator <<(string &s)
{
	sprintf(buffer, "%s%s", buffer, s.c_str());
	return *this;
}

void LoggerStream::flush()
{
	if (strlen(buffer) != 0)
		log->WriteLogEx(level, 0, file.c_str(), line, "%s", buffer);
	reset();
}

LoggerStream& LoggerStream::operator <<(const ENDL& e)
{
	flush();
	return *this;
}

void LoggerStream::reset()
{
	buffer[0] = 0;
	file = "";
	line = 0;
	mutex.UnLock();
}

LoggerStream& LoggerStream::lock()
{
	mutex.Lock();

	return *this;
}

#ifdef _WIN32
# define PATH_SPERATOR_CHAR '\\'
#else
# define PATH_SPERATOR_CHAR '/'
#endif

void LoggerStream::SetSource(const string& file, int line)
{
	int index = file.find_last_of(PATH_SPERATOR_CHAR);
	if (index == -1)
	{
		this->file = file;
	}
	else
	{
		this->file = file.substr(index + 1, file.length() - index - 1);
	}
	this->line = line;
}

LoggerStream::~LoggerStream()
{

}

void InitLoggerStream(UTILITY::CLogFile *log)
{
	UTILITY::_err_ss = UTILITY::LoggerStream(LOGGER_LEVEL_ERROR, log);
	UTILITY::_warn_ss = UTILITY::LoggerStream(LOGGER_LEVEL_WARN, log);
	UTILITY::_info_ss = UTILITY::LoggerStream(LOGGER_LEVEL_INFO, log);
	UTILITY::_debug_ss = UTILITY::LoggerStream(LOGGER_LEVEL_DEBUG, log);
	UTILITY::_none_ss = UTILITY::LoggerStream(LOGGER_LEVEL_NONE, log);
}

} /* namespace UTILITY */

