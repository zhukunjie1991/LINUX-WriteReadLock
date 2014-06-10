#include "Utils.h"
#ifndef WIN32
#include <sys/time.h>
#endif

bool string_to_double(string str,double &a) 
{ 
	for(int i=0;i<str.size();i++)
		if((str[i]<'0'||str[i]>'9')&&str[i]!='.')
			return false;
	double x=0,y=0;
	int position=str.size(),j;
	if(str.find('.')<str.size())position=str.find('.');
	for(j=0;j<position;j++)x+=(int(str[j])-48)*pow((double)10,(double)position-1-j);
	for(j=position+1;j<str.size();j++)y+=(int(str[j])-48)*pow((double)10,(double)position-j);
	a=x+y;
	return true;
}

#ifndef WIN32
/*******************************************************************************
* 函数名称 : GetTickCount
* 功能描述 : 
* 返 回 值 : unsignedint 
* 作　　者 : zhangyunchao
* 设计日期 : 2012年9月24日
* 修改日期     修改人    修改内容
*******************************************************************************/
unsigned int GetTickCount()
{
    struct timeval CurrentTime;
    gettimeofday(&CurrentTime, NULL);
    return ((CurrentTime.tv_sec * 1000) + (CurrentTime.tv_usec / 1000));   
}
#endif

