/*******************************************************************************
  * ��Ȩ����(C) 2012 China Foreign Exchange Trade System  All rights reserved.
  * �ļ����� : error_handler.hpp
  * ��ǰ�汾 : 1.0.0.1
  * ��    �� : zhangyunchao (zhangyunchao@chinamoney.com.cn)
  * ������� : 2012��8��31��
  * ����ժҪ :
  * �޸ļ�¼ :
  * ��    ��  ��    ��  �޸���   �޸�ժҪ

  ********************************************************************************/
#ifndef __ERROR_HANDLER_HPP__
#define __ERROR_HANDLER_HPP__

#include "Errorlog.h"

namespace CFETS
{
// �Ӵ�����Ϣ������ȡ�ô�����
int GetErrorCode(int code,  int act, const char * msg)
{
    return code;
}

// �Ӵ�����Ϣ������ȡ�ô�������
const char* GetErrorMsg(int code, int act, const char * msg)
{
    return msg;
}

// �Ӵ�����Ϣ������ȡ�ô�����ʽ
int GetErrorAction(int code, int act, const char * msg)
{
    return act;
}

// ������
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
