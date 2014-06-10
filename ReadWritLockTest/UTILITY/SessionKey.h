/*******************************************************************************
* 版权所有(C) 2012 China Foreign Exchange Trade System  All rights reserved.
* 文件名称 : session_key.h
* 当前版本 : 1.0.0.1
* 作    者 : zhangyunchao (zhangyunchao@chinamoney.com.cn)
* 设计日期 : 2012年9月17日
* 内容摘要 :
* 修改记录 :
* 日    期  版    本  修改人   修改摘要

********************************************************************************/

#ifndef __SESSION_KEY_H__
#define __SESSION_KEY_H__

#include <stdio.h>
#include <string>
#include "PubFunc.h"

class SessionKey
{
public:
    SessionKey()
    {

    }

    SessionKey(const SessionKey & key)
    {
        m_senderCompID     = key.m_senderCompID;
        m_senderSubID      = key.m_senderSubID;
        m_targetCompID     = key.m_targetCompID;
        m_targetSubID      = key.m_targetSubID;
        m_onBehalfOfCompID = key.m_onBehalfOfCompID;
        m_onBehalfOfSubID  = key.m_onBehalfOfSubID;
        m_deliverToCompID  = key.m_deliverToCompID;
        m_deliverToSubID   = key.m_deliverToSubID;
    }

    SessionKey & operator = (const SessionKey & key)
    {
        m_senderCompID     = key.m_senderCompID;
        m_senderSubID      = key.m_senderSubID;
        m_targetCompID     = key.m_targetCompID;
        m_targetSubID      = key.m_targetSubID;
        m_onBehalfOfCompID = key.m_onBehalfOfCompID;
        m_onBehalfOfSubID  = key.m_onBehalfOfSubID;
        m_deliverToCompID  = key.m_deliverToCompID;
        m_deliverToSubID   = key.m_deliverToSubID;

        return *this;
    }

    bool operator < (const SessionKey& key)
    {
        // mdf by wujinghe on 2013-07-15
        //if (m_senderCompID < key->m_senderCompID)
        //    return true;
        //else if (m_senderCompID == key->m_senderCompID)
        //{
        //    if (m_senderSubID < key->m_senderSubID)
        //        return true;
        //    else if (m_senderSubID == key->m_senderSubID)
        //    {
        //        if (m_targetCompID < key->m_targetCompID)
        //           return true;
        //    }
        //    // else if (m_targetCompID == key->m_targetCompID)
        //    // {
        //    //     if (m_targetSubID < key->m_targetSubID)
        //    //         return true;
        //    //       if (onBehalfOfCompID < key.onBehalfOfCompID)
        //    //       {
        //    //           return true;
        //    //       }
        //    //       else if (onBehalfOfCompID == key.onBehalfOfCompID)
        //    //       {
        //    //           if (onBehalfOfSubID < key.onBehalfOfSubID)
        //    //               return true;
        //    //           else if (deliverToCompID < key.deliverToCompID)
        //    //               return true;
        //    //           else if (deliverToCompID == key.deliverToCompID)
        //    //           {
        //    //               if (deliverToSubID < key.deliverToSubID)
        //    //                   return true;
        //    //           }
        //    //       }
        //    // }
        //}
        // end mdf on 2013-07-15

        if (m_senderCompID != key.m_senderCompID)
            return m_senderCompID < key.m_senderCompID;
        if (m_senderSubID != key.m_senderSubID)
            return m_senderSubID < key.m_senderSubID;
        if (m_targetCompID != key.m_targetCompID)
            return m_targetCompID < key.m_targetCompID;
        return false;
    }

    bool operator == (const SessionKey& key) const
    {
	return 	(m_senderCompID == key.m_senderCompID &&
                 m_senderSubID == key.m_senderSubID &&
                 m_targetCompID == key.m_targetCompID);
                 // &&
                 // m_targetSubID == key.m_targetSubID &&
                 // m_onBehalfOfCompID == key.m_onBehalfOfCompID &&
                 // m_onBehalfOfSubID == key.m_onBehalfOfSubID &&
                 // m_deliverToCompID == key.m_deliverToCompID &&
                 // m_deliverToSubID == key.m_deliverToSubID);
    }

    std::string toString()
    {
        char buf[2048] = {0};
        UTILITY::Snprintf(buf, sizeof(buf), "SenderCompID: %s, SenderSubID: %s,  TargetCompID: %s, TargetSubID: %s,"
                "OnBehalfOfCompID: %s, OnBehalfOfSubID: %s, DeliverToCompID: %s,"
                "DeliverToSubID: %s",
                m_senderCompID.c_str(),
				m_senderSubID.c_str(),
				m_targetCompID.c_str(),
				m_targetSubID.c_str(),
				m_onBehalfOfCompID.c_str(),
				m_onBehalfOfSubID.c_str(),
				m_deliverToCompID.c_str(),
				m_deliverToSubID.c_str());

        return std::string(buf);
    }

public:
    std::string m_senderCompID;
    std::string m_senderSubID;
    std::string m_targetCompID;
    std::string m_targetSubID;

    // 最初发送方标识符（可加密），用于经第三方中转发送
    std::string m_onBehalfOfCompID;
    // 最初发送方子标识符（可加密）, 用于经第三方中转发送
    std::string m_onBehalfOfSubID;
    // 最终接收方标识符（可加密），用于经第三方中转发送
    std::string m_deliverToCompID;
    // 最终接收方子标识符（可加密）, 用于经第三方中转发送
    std::string m_deliverToSubID;
};

class SessionKeySort
{
public:
    bool operator()(SessionKey* const &_A, SessionKey* const &_B) const
    {
        return (*_A) < (*_B);
    }
};

#endif //__SESSION_KEY_H__
