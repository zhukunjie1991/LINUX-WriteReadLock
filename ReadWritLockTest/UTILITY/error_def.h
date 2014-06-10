/*******************************************************************************
* 版权所有(C) 2012 China Foreign Exchange Trade System  All rights reserved.
* 文件名称 : error_def.h
* 当前版本 : 1.0.0.1
* 作    者 : zhangyunchao (zhangyunchao@chinamoney.com.cn)
* 设计日期 : 2012年8月31日
* 内容摘要 :
* 修改记录 :
* 日    期  版    本  修改人   修改摘要

********************************************************************************/
#ifndef __ERROR_DEF_H__
#define __ERROR_DEF_H__

// 对于错误码的处理方式
enum Error_Action {
    ERROR_HANDLE_LOG  = 1,      //日志
    ERROR_HANDLE_SCR  = 2,      //屏幕
    ERROR_HANDLE_MAIL = 4       //邮件
};

/* 错误码定义规则
*    系统名称              错误宏首写字符             错误码范围
*-------------------------------------------------------------------------------------------
*    框架                  FRM                                     10000-19999
*    协议                  PTC                                     20000-29999
*    应用                  APP                                     30000-
*/
//#define SUCCESS 0
#define APP_NULL_POINTER_PARAM                    30000, ERROR_HANDLE_LOG, "空指针参数"
#define APP_UNREGIST_SESSION                      30001, ERROR_HANDLE_LOG, "注销Session出错"
#define APP_NULL_POINTER_OBJ                      30002, ERROR_HANDLE_LOG, "空指针对象"
// 310xx为组件加载模块
#define APP_NULL_COMP_NAME                        31001, ERROR_HANDLE_LOG, "动态库名为空"
#define APP_DYNAMIC_LOAD_FAILED                   31002, ERROR_HANDLE_LOG, "动态库加载失败。请确认路径和动态库名是否正确"
#define APP_CALL_FUNC_FAILED                      31003, ERROR_HANDLE_LOG, "调用动态库中的函数失败。请确认函数是否存在，函数书写格式是否符合动态加载要求"
#define APP_CREATE_COMP_FAILED                    31004, ERROR_HANDLE_LOG, "动态库中创建Component出错"
#define APP_GET_VERSION_FAILED                    31005, ERROR_HANDLE_LOG, "动态库中版本号出错"
#define APP_COMP_NOT_EXIST                        31006, ERROR_HANDLE_LOG, "组件不存在"
#define APP_COMP_TYPE_NULL                        31007, ERROR_HANDLE_LOG, "组件类型(type or subType)为空"
#define APP_COMP_TYPE_FAILED                      31008, ERROR_HANDLE_LOG, "获取组件类型出错"
//#define APP_COMP_NOT_EXIST_WITH_CONDITION 31009, ERROR_HANDLE_LOG, "不存在符合该条件的组件"
#define APP_UNREAD_CONFIG_ITEM_VALUE              31010, ERROR_HANDLE_LOG, "未读到配置项的值"
#define APP_NULL_CONFIG_NAME                      31011, ERROR_HANDLE_LOG, "配置文件名为空"
#define APP_LOAD_CONFIG_FAILED                    31012, ERROR_HANDLE_LOG, "加载配置失败"
#define APP_READ_CONFIG_INFO_FAILED               31013, ERROR_HANDLE_LOG, "读取配置文件信息失败，请确认配置文件中是否含有"
#define APP_ADD_COMP_OBJ_FAILED                   31014, ERROR_HANDLE_LOG, "添加组件对象失败"
// 311xx为防攻击
#define APP_DF_IP_NULL                            31101, ERROR_HANDLE_LOG, "IP地址为空"
#define APP_EXCEED_MAX_SESSION_NUM                31102, ERROR_HANDLE_LOG, "超过最大的会话连接数"
#define APP_CONN_NOT_EXIST                        31103, ERROR_HANDLE_LOG, "会话连接不存在"
// 312xx为负载均衡
#define APP_LB_SERVICE_NULL                       31201, ERROR_HANDLE_LOG, "负载均衡服务为空"
// 313xx为client处理
#define APP_CLI_EXIST_SESSION                     31301, ERROR_HANDLE_LOG, "Session已存在"
#define APP_COMMSERVER_NOT_FIND_CLIENT                  31302, ERROR_HANDLE_LOG, "Client Session列表中不存在该session"
#define APP_CLI_UPDATE_TOKEN_FAILED               31303, ERROR_HANDLE_LOG, "Token更新失败"
#define APP_CLI_FC_OBJ_NULL                       31304, ERROR_HANDLE_LOG, "流控对象为空"
#define APP_CLI_LOGOUT_FAILED                     31305, ERROR_HANDLE_LOG, "Client注销失败"
// 314xx为server处理
#define APP_INVALID_MSG_TYPE                  31401, ERROR_HANDLE_LOG, "无效消息类型,消息长度为0"
#define APP_UNSUPPORT_MSG_TYPE                31402, ERROR_HANDLE_LOG, "不支持的消息类型"
#define APP_INVALID_MSG_FORMAT	  31403, ERROR_HANDLE_LOG, "无效的消息格式,得到消息的基本要素出错"
#define APP_SRV_INVALID_MSG_FORMAT_SRC_GROUP_NULL 31404, ERROR_HANDLE_LOG, "无效的消息格式,源服务组名称为空"
#define APP_SRV_REGISTER_SERVICE_FAILED           31405, ERROR_HANDLE_LOG, "注册服务失败"

#define APP_SRV_GET_FC_FAILED                     31407, ERROR_HANDLE_LOG, "获取流控组件失败"
#define APP_SRV_CLIENT_CONN_ERR                   31408, ERROR_HANDLE_LOG, "注册客户端连接出错"
#define APP_SRV_NOT_FIND_CA_SERVER                31410, ERROR_HANDLE_LOG, "没有找到认证服务器连接"
#define APP_SRV_NOT_FIND_CONN_IN_CLIENT_LIST      31411, ERROR_HANDLE_LOG, "在客户连接列表中，没有找到该连接"
#define APP_SRV_GROUP_NAME_NULL                   31412, ERROR_HANDLE_LOG, "组名为空"
#define APP_SRV_ADD_SESSION_TO_GROUP_FAILED       31413, ERROR_HANDLE_LOG, "添加Session到组失败"
#define APP_SRV_NOT_FIND_GROUP                    31414, ERROR_HANDLE_LOG, "找不到组"
#define APP_SRV_SESSION_MSG_NULL                  31415, ERROR_HANDLE_LOG, "Session消息为空"
#define APP_SRV_NOT_FIND_SESSION_KEY              31416, ERROR_HANDLE_LOG, "找不到Session key"
#define APP_SRV_NOT_FIND_SERVER                   31417, ERROR_HANDLE_LOG, "没有找到目标服务器连接"
#define APP_SRV_REMOVE_SESSION_FROM_GROUP_FAILED  31418, ERROR_HANDLE_LOG, "从组中删除Session失败"
#define APP_EXISTED_CONN_SESSION                  31419, ERROR_HANDLE_LOG, "Session已存在"
#define APP_SRV_NOT_FIND_SERVICE                  31422, ERROR_HANDLE_LOG, "Can't find the service."
#define APP_MSG_SET_STRING_FAILED                 31423, ERROR_HANDLE_LOG, "消息解析失败"
#define APP_SRV_GEN_LOGOUT_ERR        31424, ERROR_HANDLE_LOG, "生成认证服务器注销消息出错"
// 315xx为protocol manager

#define APP_TRANSFER_PROTOCOL_NULL					20002, ERROR_HANDLE_LOG, "传输协议类型为空值"

// IMIX_FAST protocol
#define PTC_FAST_MSG_FIELD_EMPTY 31432, ERROR_HANDLE_LOG, "The message field is empty."
#define PTC_FAST_MSG_PARSE_FAILED 31435, ERROR_HANDLE_LOG, "The message parsing failed."
// admin message
#define PTC_FAST_GEN_ADMIN_LOGON_MSG_FAILED          31428, ERROR_HANDLE_LOG, "Generate a Logon messsage failed."
#define PTC_FAST_GEN_ADMIN_LOGONRSP_MSG_FAILED          31429, ERROR_HANDLE_LOG, "Generate a LogonRsp messsage failed."
#define PTC_FAST_GEN_ADMIN_LOGOUT_MSG_FAILED          31430, ERROR_HANDLE_LOG, "Generate a Logout messsage failed."
#define PTC_FAST_GEN_ADMIN_LOGOUTRSP_MSG_FAILED          31430, ERROR_HANDLE_LOG, "Generate a LogoutRsp messsage failed."
#define PTC_FAST_GEN_ADMIN_MSG_FAILED          31431, ERROR_HANDLE_LOG, "Generate a admin messsage failed."
// cert message
#define PTC_FAST_GEN_CERT_MSG_FAILED 31427, ERROR_HANDLE_LOG, "Generate a cert messsage failed."
#define PTC_FAST_GEN_CERT_UPDATE_TOKEN_MSG_FAILED 31420, ERROR_HANDLE_LOG, "Generate a UpdateToken messsage failed."
#define PTC_FAST_GEN_CERT_TOKEN_MSG_FAILED 31426, ERROR_HANDLE_LOG, "Generate a Token messsage failed."
#define PTC_FAST_GEN_CERT_CA_MSG_FAILED           31409, ERROR_HANDLE_LOG,  "Generate a CA messsage failed."
#define PTC_FAST_GEN_CERT_LOGOUT_MSG_FAILED        31421, ERROR_HANDLE_LOG, "Generate a CertLogout messsage failed."

// CommServer
// message processing
#define PTC_INVALID_MESSAGE                       20000, ERROR_HANDLE_LOG, "无效报文类型"
#define APP_MSG_PROCESS_HEADER_FAILED 31433, ERROR_HANDLE_LOG, "Process message header failed."
#define APP_MSG_CONVERT_FAILED						20001, ERROR_HANDLE_LOG, "消息转换失败"

#define APP_COMMSERVER_HANDLE_FUNC_NULL 31434, ERROR_HANDLE_LOG, "The handling function is NULL."
#define APP_COMMSERVER_LOGON_FAILED 31435, ERROR_HANDLE_LOG, "CommServer logon to other node failed."
#define APP_COMMSERVER_PROCESS_LOGONRSP_FAILED 31436, ERROR_HANDLE_LOG, "Process logonrsp failed."
#define APP_COMMSERVER_SEND_ERROR_MSG 31436, ERROR_HANDLE_LOG, "Send an error message."


#endif //__ERROR_DEF_H__





