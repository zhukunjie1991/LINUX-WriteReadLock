#ifndef UTILITY_SESSION_MESSAGE_H
#define UTILITY_SESSION_MESSAGE_H

// ���ڱ�ͨѶ�м���Ļ�����Ϣ��ʽ

namespace CFETS
{

class SessionMessage
{
public:
	//��Ϣ���к�
	int m_seqNo;
	//��Ϣid
	int m_msgId;
	//��Ϣ����
	int m_len;
	//��Ϣ����
	char* m_msgBody;

public:
	SessionMessage();
	virtual ~SessionMessage();
public:
	//���鷽�������ڴ��ַ���������Ϣ�����ڲ������ַ�������˲�����string
	virtual bool setString(const char*, int){return false;};
	virtual void Init(int msgId, int seqNo, const char* buffer, int len);
	virtual void Clear(){};
	virtual void toBuffer(){};
};

} //end namespace

#endif /* UTILITY_SESSION_MESSAGE_H */
