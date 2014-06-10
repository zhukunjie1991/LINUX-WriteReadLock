#ifndef UTILS_H
#define UTILS_H

/*
#ifndef FALSE
#define FALSE -1
#endif
#ifndef TRUE
#define TRUE 1

#endif
*/
#include<string>
#include <math.h>
#ifdef WIN32
#include <hash_map>
using namespace stdext; 
#elif defined(__GNU_GCC__) 
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif
using namespace std;

//�������㺯����
//���ڰ��ַ���ת��Ϊdouble
bool string_to_double(string str,double &a);

//����hash_map<string,*>��hash�㷨

#ifdef __GNU_GCC__
struct string_hash   
{   
	size_t operator()(const string& str) const
	{   
		return __stl_hash_string(str.c_str());   
	}   
};
#endif

#endif
