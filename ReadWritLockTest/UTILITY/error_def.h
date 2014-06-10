/*******************************************************************************
* ��Ȩ����(C) 2012 China Foreign Exchange Trade System  All rights reserved.
* �ļ����� : error_def.h
* ��ǰ�汾 : 1.0.0.1
* ��    �� : zhangyunchao (zhangyunchao@chinamoney.com.cn)
* ������� : 2012��8��31��
* ����ժҪ :
* �޸ļ�¼ :
* ��    ��  ��    ��  �޸���   �޸�ժҪ

********************************************************************************/
#ifndef __ERROR_DEF_H__
#define __ERROR_DEF_H__

// ���ڴ�����Ĵ���ʽ
enum Error_Action {
    ERROR_HANDLE_LOG  = 1,      //��־
    ERROR_HANDLE_SCR  = 2,      //��Ļ
    ERROR_HANDLE_MAIL = 4       //�ʼ�
};

/* �����붨�����
*    ϵͳ����              �������д�ַ�             �����뷶Χ
*-------------------------------------------------------------------------------------------
*    ���                  FRM                                     10000-19999
*    Э��                  PTC                                     20000-29999
*    Ӧ��                  APP                                     30000-
*/
//#define SUCCESS 0
#define APP_NULL_POINTER_PARAM                    30000, ERROR_HANDLE_LOG, "��ָ�����"
#define APP_UNREGIST_SESSION                      30001, ERROR_HANDLE_LOG, "ע��Session����"
#define APP_NULL_POINTER_OBJ                      30002, ERROR_HANDLE_LOG, "��ָ�����"
// 310xxΪ�������ģ��
#define APP_NULL_COMP_NAME                        31001, ERROR_HANDLE_LOG, "��̬����Ϊ��"
#define APP_DYNAMIC_LOAD_FAILED                   31002, ERROR_HANDLE_LOG, "��̬�����ʧ�ܡ���ȷ��·���Ͷ�̬�����Ƿ���ȷ"
#define APP_CALL_FUNC_FAILED                      31003, ERROR_HANDLE_LOG, "���ö�̬���еĺ���ʧ�ܡ���ȷ�Ϻ����Ƿ���ڣ�������д��ʽ�Ƿ���϶�̬����Ҫ��"
#define APP_CREATE_COMP_FAILED                    31004, ERROR_HANDLE_LOG, "��̬���д���Component����"
#define APP_GET_VERSION_FAILED                    31005, ERROR_HANDLE_LOG, "��̬���а汾�ų���"
#define APP_COMP_NOT_EXIST                        31006, ERROR_HANDLE_LOG, "���������"
#define APP_COMP_TYPE_NULL                        31007, ERROR_HANDLE_LOG, "�������(type or subType)Ϊ��"
#define APP_COMP_TYPE_FAILED                      31008, ERROR_HANDLE_LOG, "��ȡ������ͳ���"
//#define APP_COMP_NOT_EXIST_WITH_CONDITION 31009, ERROR_HANDLE_LOG, "�����ڷ��ϸ����������"
#define APP_UNREAD_CONFIG_ITEM_VALUE              31010, ERROR_HANDLE_LOG, "δ�����������ֵ"
#define APP_NULL_CONFIG_NAME                      31011, ERROR_HANDLE_LOG, "�����ļ���Ϊ��"
#define APP_LOAD_CONFIG_FAILED                    31012, ERROR_HANDLE_LOG, "��������ʧ��"
#define APP_READ_CONFIG_INFO_FAILED               31013, ERROR_HANDLE_LOG, "��ȡ�����ļ���Ϣʧ�ܣ���ȷ�������ļ����Ƿ���"
#define APP_ADD_COMP_OBJ_FAILED                   31014, ERROR_HANDLE_LOG, "����������ʧ��"
// 311xxΪ������
#define APP_DF_IP_NULL                            31101, ERROR_HANDLE_LOG, "IP��ַΪ��"
#define APP_EXCEED_MAX_SESSION_NUM                31102, ERROR_HANDLE_LOG, "�������ĻỰ������"
#define APP_CONN_NOT_EXIST                        31103, ERROR_HANDLE_LOG, "�Ự���Ӳ�����"
// 312xxΪ���ؾ���
#define APP_LB_SERVICE_NULL                       31201, ERROR_HANDLE_LOG, "���ؾ������Ϊ��"
// 313xxΪclient����
#define APP_CLI_EXIST_SESSION                     31301, ERROR_HANDLE_LOG, "Session�Ѵ���"
#define APP_COMMSERVER_NOT_FIND_CLIENT                  31302, ERROR_HANDLE_LOG, "Client Session�б��в����ڸ�session"
#define APP_CLI_UPDATE_TOKEN_FAILED               31303, ERROR_HANDLE_LOG, "Token����ʧ��"
#define APP_CLI_FC_OBJ_NULL                       31304, ERROR_HANDLE_LOG, "���ض���Ϊ��"
#define APP_CLI_LOGOUT_FAILED                     31305, ERROR_HANDLE_LOG, "Clientע��ʧ��"
// 314xxΪserver����
#define APP_INVALID_MSG_TYPE                  31401, ERROR_HANDLE_LOG, "��Ч��Ϣ����,��Ϣ����Ϊ0"
#define APP_UNSUPPORT_MSG_TYPE                31402, ERROR_HANDLE_LOG, "��֧�ֵ���Ϣ����"
#define APP_INVALID_MSG_FORMAT	  31403, ERROR_HANDLE_LOG, "��Ч����Ϣ��ʽ,�õ���Ϣ�Ļ���Ҫ�س���"
#define APP_SRV_INVALID_MSG_FORMAT_SRC_GROUP_NULL 31404, ERROR_HANDLE_LOG, "��Ч����Ϣ��ʽ,Դ����������Ϊ��"
#define APP_SRV_REGISTER_SERVICE_FAILED           31405, ERROR_HANDLE_LOG, "ע�����ʧ��"

#define APP_SRV_GET_FC_FAILED                     31407, ERROR_HANDLE_LOG, "��ȡ�������ʧ��"
#define APP_SRV_CLIENT_CONN_ERR                   31408, ERROR_HANDLE_LOG, "ע��ͻ������ӳ���"
#define APP_SRV_NOT_FIND_CA_SERVER                31410, ERROR_HANDLE_LOG, "û���ҵ���֤����������"
#define APP_SRV_NOT_FIND_CONN_IN_CLIENT_LIST      31411, ERROR_HANDLE_LOG, "�ڿͻ������б��У�û���ҵ�������"
#define APP_SRV_GROUP_NAME_NULL                   31412, ERROR_HANDLE_LOG, "����Ϊ��"
#define APP_SRV_ADD_SESSION_TO_GROUP_FAILED       31413, ERROR_HANDLE_LOG, "���Session����ʧ��"
#define APP_SRV_NOT_FIND_GROUP                    31414, ERROR_HANDLE_LOG, "�Ҳ�����"
#define APP_SRV_SESSION_MSG_NULL                  31415, ERROR_HANDLE_LOG, "Session��ϢΪ��"
#define APP_SRV_NOT_FIND_SESSION_KEY              31416, ERROR_HANDLE_LOG, "�Ҳ���Session key"
#define APP_SRV_NOT_FIND_SERVER                   31417, ERROR_HANDLE_LOG, "û���ҵ�Ŀ�����������"
#define APP_SRV_REMOVE_SESSION_FROM_GROUP_FAILED  31418, ERROR_HANDLE_LOG, "������ɾ��Sessionʧ��"
#define APP_EXISTED_CONN_SESSION                  31419, ERROR_HANDLE_LOG, "Session�Ѵ���"
#define APP_SRV_NOT_FIND_SERVICE                  31422, ERROR_HANDLE_LOG, "Can't find the service."
#define APP_MSG_SET_STRING_FAILED                 31423, ERROR_HANDLE_LOG, "��Ϣ����ʧ��"
#define APP_SRV_GEN_LOGOUT_ERR        31424, ERROR_HANDLE_LOG, "������֤������ע����Ϣ����"
// 315xxΪprotocol manager

#define APP_TRANSFER_PROTOCOL_NULL					20002, ERROR_HANDLE_LOG, "����Э������Ϊ��ֵ"

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
#define PTC_INVALID_MESSAGE                       20000, ERROR_HANDLE_LOG, "��Ч��������"
#define APP_MSG_PROCESS_HEADER_FAILED 31433, ERROR_HANDLE_LOG, "Process message header failed."
#define APP_MSG_CONVERT_FAILED						20001, ERROR_HANDLE_LOG, "��Ϣת��ʧ��"

#define APP_COMMSERVER_HANDLE_FUNC_NULL 31434, ERROR_HANDLE_LOG, "The handling function is NULL."
#define APP_COMMSERVER_LOGON_FAILED 31435, ERROR_HANDLE_LOG, "CommServer logon to other node failed."
#define APP_COMMSERVER_PROCESS_LOGONRSP_FAILED 31436, ERROR_HANDLE_LOG, "Process logonrsp failed."
#define APP_COMMSERVER_SEND_ERROR_MSG 31436, ERROR_HANDLE_LOG, "Send an error message."


#endif //__ERROR_DEF_H__





