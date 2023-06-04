/* AutoConfig.h */

#ifndef AUTOCONFIG_H
#define AUTOCONFIG_H
#ifdef _WIN32
#include <stdarg.h>
#else                   /* UNIX compatible version          */
//#include <varargs.h>
#include <stdarg.h>
#define stricmp strcasecmp
#define _stricmp strcasecmp
#define strnicmp strncasecmp
#define _strnicmp strncasecmp
#define _strdup strdup
#define	_timeb	timeb
#define	_ftime	ftime
//#define	__time32_t	time32_t 
//#define	__time64_t	time64_t 
//extern char* strdup (const char* s);
#endif

#define MAX_N_ITEMS 256
enum Mytpes
{	_NUL,
	_COMMENT,
	_CHAR,
	_INT,
	_FLOAT,
	_DOUBLE,
	_STR
}; 

class AutoConfigItem
{
public:
	char *name;
	char *format;
	void *ptr;
	Mytpes type;
	void * *pptr;
	int np;
	int nb; //bytes for STR
	AutoConfigItem(void)
	{	name = NULL;
		format = NULL;
		ptr = NULL;
		type = _NUL;
		np = nb = 0;
		pptr = NULL;
	}
};

class AutoConfig
{
public:
	int n; // number of items
	AutoConfigItem it[MAX_N_ITEMS];
	int configNparW;
	int configNparR;
	int configNparNeed;

	AutoConfig(void)
	{ n = 0;
		configNparW = configNparR = configNparNeed = 0;
	}
	~AutoConfig(void)
	{	int i;
		if(n > 0)
		for(i=n-1; i>=0; i--)
		{	if(it[i].format != NULL){ free(it[i].format); it[i].format = NULL;}
			if(it[i].name != NULL){ free(it[i].name); it[i].name = NULL;}
			if(it[i].pptr!= NULL){ free(it[i].pptr); it[i].pptr = NULL; it[i].np = 0;}
		}
	}

	int AddV(const char *_name, const char *_format, void *_ptr, Mytpes _type);
	int AddStr(const char *_name, const char *_format, void *_ptr, int _nb);
	int AddC(const char *_format, ...);
	int Write(char *fname);
	int WriteN(FILE *fp, int id);
	int Read(char *fname);

	int RenameToBak(char *fname);
	int AnalizeRecodrRead(char *name, char *par);
	int ReadStr(FILE *fp, char *str, char *name, char *par);

};

#endif //AUTOCONFIG_H