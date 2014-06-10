#ifndef UTILITY_SESSION_MESSAGE_H
#define UTILITY_SESSION_MESSAGE_H

// 用于本通讯中间件的基础消息格式

namespace CFETS
{

class SessionMessage
{
public:
	//消息序列号
	int m_seqNo;
	//消息id
	int m_msgId;
	//消息长度
	int m_len;
	//消息主体
	char* m_msgBody;

public:
	SessionMessage();
	virtual ~SessionMessage();
public:
	//纯虚方法，用于从字符串生成消息，由于不限于字符串，因此不采用string
	virtual bool setString(const char*, int){return false;};
	virtual void Init(int msgId, int seqNo, const char* buffer, int len);
	virtual void Clear(){};
	virtual void toBuffer(){};
};

} //end namespace

#endif /* UTILITY_SESSION_MESSAGE_H */
