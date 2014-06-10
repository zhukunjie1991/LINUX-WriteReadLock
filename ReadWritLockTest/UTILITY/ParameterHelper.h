/*******************************************************************************
 * Copyright ? 2000-2010 China Foreign Exchange Trade System
 * & National Interbank Funding Center.All Rights Reserved.
 * �ļ����� : ParameterHelper.h
 * �������� : 2013-7-10
 * ˵	 �� : 
 * ��	 �� : ZhanYi (yi.zhan@accenture.com) 
 * �޸ļ�¼ : 
 * �޸ı��        ��    ��   �޸���   �޸�ժҪ
 * 00000001    2013-7-10      ZhanYi        Create
 ********************************************************************************/

#ifndef PARAMETERHELPER_H_
#define PARAMETERHELPER_H_
#include <string>
#include <map>

typedef void (*ParameterCallBackFunction)(char opt, char *value);

namespace UTILITY
{
class ParameterHelper
{
public:
	virtual int Register(char opt,
			bool needValue,
			ParameterCallBackFunction cb,
			const std::string &desc = "") = 0;

	virtual int Parse(int argc, char **argv) = 0;

	static ParameterHelper *CreateParameterHelper();

	static void DeleteParameterHelper(ParameterHelper *helper);

protected:
	ParameterHelper();
	~ParameterHelper();
};

} /* namespace UTILITY */

#endif /* PARAMETERHELPER_H_ */
