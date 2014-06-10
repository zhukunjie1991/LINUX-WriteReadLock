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

//算数计算函数。
//用于把字符串转变为double
bool string_to_double(string str,double &a);

//定义hash_map<string,*>的hash算法

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
