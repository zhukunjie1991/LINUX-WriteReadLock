/*******************************************************************************
  * 版权所有(C) 2012 China Foreign Exchange Trade System  All rights reserved.
  * 文件名称 : error_handler.hpp
  * 当前版本 : 1.0.0.1
  * 作    者 : zhangyunchao (zhangyunchao@chinamoney.com.cn)
  * 设计日期 : 2012年8月31日
  * 内容摘要 :
  * 修改记录 :
  * 日    期  版    本  修改人   修改摘要

  ********************************************************************************/
#ifndef __ERROR_HANDLER_HPP__
#define __ERROR_HANDLER_HPP__

#include "Errorlog.h"

namespace CFETS
{
// 从错误信息定义中取得错误码
int GetErrorCode(int code,  int act, const char * msg)
{
    return code;
}

// 从错误信息定义中取得错误描述
const char* GetErrorMsg(int code, int act, const char * msg)
{
    return msg;
}

// 从错误信息定义中取得错误处理方式
int GetErrorAction(int code, int act, const char * msg)
{
    return act;
}

// 错误处理
void RaiseError(const char* srcFile,
                int line,
                int level,
                int type,
                int code,
                int act,
                const char* msg,
		...)
{
    char cLogBuffer[LOG_MAX_MSG_LEN];
    UtilLib::_ERROR_LOG *pErrorLog = NULL;
    va_list vlErrorList;

    va_start(vlErrorList, msg);
    UtilLib::VProcessEventLog(srcFile,
                              line,
                              level,
                              type,
                              code,
                              &pErrorLog,
                              msg,
                              vlErrorList,
                              cLogBuffer);

    if (act & ERROR_HANDLE_LOG)
        UtilLib::ProcessEventLogToFile(cLogBuffer, pErrorLog, level);
    if (act & ERROR_HANDLE_SCR)
        UtilLib::ProcessEventLogToScr(cLogBuffer);
    if (act & ERROR_HANDLE_MAIL) {}

    va_end(vlErrorList);

    return;
}
} // end namespace CFETS

#endif //__ERROR_HANDLER_HPP__
