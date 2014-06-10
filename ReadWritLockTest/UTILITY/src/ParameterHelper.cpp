/*******************************************************************************
 * Copyright ? 2000-2010 China Foreign Exchange Trade System
 * & National Interbank Funding Center.All Rights Reserved.
 * 文件名称 : ParameterHelper.cpp
 * 创建日期 : 2013-7-10
 * 说	 明 : 
 * 作	 者 : ZhanYi (yi.zhan@accenture.com) 
 * 修改记录 : 
 * 修改编号        日    期   修改人   修改摘要
 * 00000001    2013-7-10      ZhanYi        Create
 ********************************************************************************/


#if defined(_WIN32)
#define PATH_SEPARETOR "\\"
#else
#define PATH_SEPARETOR "/"
# include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <string.h>
#include <stdlib.h>
#include "ParameterHelper.h"


#if defined(_WIN32)
#if defined(__GNUC__)
#include <getopt.h>
#else
#include <stdio.h>

#define NULL	0
#define EOF	(-1)
#define ERR(s, c)	if(opterr){\
		char errbuf[2];\
		errbuf[0] = c; errbuf[1] = '\n';\
		fputs(argv[0], stderr);\
		fputs(s, stderr);\
		fputc(c, stderr);}

static int	opterr = 1;
static int	optind = 1;
static int	optopt;
static char	*optarg;

static int getopt(int, char **, char *);

int getopt(int	argc, char **argv, char *opts)
{
	static int sp = 1;
	register int c;
	register char *cp;

	if(sp == 1)
		if(optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(EOF);
		else if(strcmp(argv[optind], "--") == NULL) {
			optind++;
			return(EOF);
		}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == NULL) {
		ERR(": illegal option -- ", c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			ERR(": option requires an argument -- ", c);
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}

#endif /* __GUNC__ */
#endif /* _WIN32 */


using namespace std;

namespace UTILITY
{

////////////////////////////////////////////////////////////////////////
// IMPLEMENT CLASS DECLARE

class ParameterHelperImpl : public ParameterHelper
{
public:
	ParameterHelperImpl();
	virtual ~ParameterHelperImpl();

	int Parse(int argc, char **argv);

	int Register(char opt,
			bool needValue,
			ParameterCallBackFunction cb,
			const std::string &desc = "");

	void DefaultShowHelp(const string &program);

protected:
	map<char, ParameterCallBackFunction> m_cb;
	stringstream m_helpstream;
	char m_opts[256];
};


ParameterHelperImpl::ParameterHelperImpl()
{
	m_opts[0] = '\0';
}

ParameterHelperImpl::~ParameterHelperImpl()
{
}

int ParameterHelperImpl::Parse(int argc, char** argv)
{
	int ch;
	opterr = 0;

	if (strchr(m_opts, 'h') == NULL)
	{
		strcat(m_opts, "h");
	}
	if (strchr(m_opts, 'H') == NULL)
	{
		strcat(m_opts, "h");
	}

	while ((ch = getopt(argc, argv, m_opts)) != -1)
	{
		map<char, ParameterCallBackFunction>::iterator it =  m_cb.find((char)ch);
		if (it != m_cb.end())
		{
			if (optarg != NULL && *optarg == '-')
			{
				cerr << "Invalid option -" << (char)ch << ": " << optarg << endl;
				exit(1);
			}

			(*it->second)(ch, (char *)optarg);
		}
		else
		{
			if ('h' == ch || 'H' == ch)
			{
				DefaultShowHelp(argv[0]);
			}
			else
			{
				continue;
			}
		}
	}

	return 0;
}

void ParameterHelperImpl::DefaultShowHelp(const string &program)
{
	int index;
	string prog;
	if (-1 != (index = program.find(PATH_SEPARETOR)))
	{
		prog = program.substr(index + 1);
	}
	else
	{
		prog = program;
	}
	cout << prog << " help information:" << endl;
	cout << m_helpstream.str() << endl;

	exit(0);
}

int ParameterHelperImpl::Register(char opt,
		bool needValue,
		ParameterCallBackFunction cb, const std::string& desc)
{
	int len = strlen(m_opts);

	m_opts[len] = opt;
	m_opts[++len] = 0;

	if (needValue)
	{
		m_opts[len] = ':';
		m_opts[++len] = 0;
	}

	m_helpstream << "\t-" << opt << "\t-\t" << desc << '\n';

	m_cb.insert(map<char, ParameterCallBackFunction>::value_type(opt, cb));

	return 0;
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// ParameterHelper FUNCTIONS

ParameterHelper::ParameterHelper()
{
}

ParameterHelper::~ParameterHelper()
{
}

ParameterHelper* ParameterHelper::CreateParameterHelper()
{
	return new ParameterHelperImpl;
}

void ParameterHelper::DeleteParameterHelper(ParameterHelper* helper)
{
	delete (ParameterHelperImpl *)helper;
}


////////////////////////////////////////////////////////////////////////

} /* namespace UTILITY */

