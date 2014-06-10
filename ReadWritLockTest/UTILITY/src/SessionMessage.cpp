#include "CFETS/SessionMessage.h"

#include <string.h> // memcpy

namespace CFETS
{

void SessionMessage::Init( int msgId,int seqNo,const char * buffer,int len )
{
	this->m_msgId = msgId;
	this->m_seqNo = seqNo;
	if(len<=0)
	{
		this->m_len = 0;
		this->m_msgBody = NULL;
		return;
	}
	this->m_len = len;	
	if(this->m_msgBody != NULL)
		delete []this->m_msgBody ;
	this->m_msgBody = new char[len];
	if(buffer == NULL)
		memset(this->m_msgBody,0,len);
	else
		memcpy(this->m_msgBody,buffer,len);

}

SessionMessage::SessionMessage()
{
	this->m_msgId = 0;
	this->m_seqNo = 0;
	this->m_len = 0;
	this->m_msgBody = NULL;
}

SessionMessage::~SessionMessage()
{
	if(this->m_msgBody != NULL)
		delete[] m_msgBody;
}

}
