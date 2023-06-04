/* AutoConfig.cpp */

#include "stdafx.h"
#include <math.h>

#include "AutoConfig.h"

#ifndef _WIN32
extern long file_length(char *f);
#endif

int AutoConfig::AddStr(const char *_name, const char *_format, void *_ptr, int _nb)
{	if(n >= MAX_N_ITEMS)
		return -1;
	if(_name)
		it[n].name = _strdup(_name);
	if(_format)
		it[n].format = _strdup(_format);
	it[n].ptr = _ptr;
	it[n].type = _STR;
	it[n].nb = _nb;
	n++;
	return 0;
}


int AutoConfig::AddV(const char *_name, const char *_format, void *_ptr, Mytpes _type)
{	if(n >= MAX_N_ITEMS)
		return -1;
	if(_name)
		it[n].name = _strdup(_name);
	if(_format)
		it[n].format = _strdup(_format);
	it[n].ptr = _ptr;
	it[n].type = _type;
	n++;
	return 0;
}

int AutoConfig::AddC(const char *_format, ...)
{  int i,  nptr;
	void *ptr;
	void * * ptr0;
	if(n >= MAX_N_ITEMS)
		return -1;
   va_list ap;
   va_start(ap, _format);
   nptr = 0;
   do
   { ptr = va_arg(ap, void *);
	 nptr++;
   } while(ptr);

   va_end(ap);

   it[n].pptr = (void **)calloc(nptr, sizeof(void *));
   it[n].np = nptr-1;
   it[n].type = _COMMENT; //comment
   va_start(ap, _format);
   i = 0;
   ptr0 = it[n].pptr;
   for(i=0; i< it[n].np; i++) 
   { ptr = va_arg(ap, void *);
	 *ptr0 = ptr;
	 ptr0++;
   } 

   va_end(ap);

	if(_format)
		it[n].format = _strdup(_format);

	n++;

	return 0;
}
int AutoConfig::WriteN(FILE *fp, int id)
{	void *p[8];
	void * *ppp;
	if(it[id].np == 0)
		return 1;
	ppp = it[id].pptr;
	switch(it[id].np)
	{	case 1:
		{	p[0] = *ppp;
			fprintf(fp,it[id].format, p[0]);
		}
		break;
		case 2:
		{	p[0] = *ppp++;
			p[1] = *ppp;
			fprintf(fp,it[id].format, p[0], p[1]);
		}
		break;
		case 3:
		{	p[0] = *ppp++;
			p[1] = *ppp++;
			p[2] = *ppp;
			fprintf(fp,it[id].format, p[0], p[1], p[2]);
		}
		break;
	}

	return 0;
}

int AutoConfig::Write(char *fname)
{	int i;
	FILE *fp;
   fp = fopen(fname,"w");
   if(fp == NULL)
	   return 1;
   for(i=0; i<n; i++)
   {	if(it[i].np)
		{	WriteN(fp, i);
			continue;
		}
	   if(it[i].name)
	   {     fprintf(fp,"%s=", it[i].name);
	   }
	   if(it[i].format)
	   {	if(it[i].ptr)
			{	switch(it[i].type)
				{	case _INT:
				fprintf(fp,it[i].format, *((int *)it[i].ptr));
						break;
					case _DOUBLE:
				fprintf(fp,it[i].format, *((double *)it[i].ptr));
						break;
					case _STR:
				fprintf(fp,it[i].format, ((char *)it[i].ptr));
						break;
				default:
					printf("Unknown type %d\n", it[i].type);
					exit(1);
				}
			}
			else
				fprintf(fp, "%s",it[i].format);
//				fprintf(fp, (const char *)it[i].format);
	   }
   }
   fclose(fp);
	return 0;
}

int AutoConfig::Read(char *fname)
{  FILE *fp;
   int  rc, rc1, flen, nstr;
   char str[260], name[128],par[128];
   fp = fopen(fname,"r");
	if(fp == NULL)
	{// printf("File %s not found\n", fname);
		return 1;
	}
   if(fp == NULL)
   {  //если  умудрились убить конфиг - попробовать найти .bak
      sprintf(str,"%s.bak",fname);
      fp = fopen(str,"r");
      if(fp == NULL)
           return 1;
   } else {
//если умудрились убить конфиг в файло длиной 0 - попробовать найти .bak
#ifdef _WIN32
      flen = _filelength(_fileno(fp));
#else
		flen = file_length(fname);
#endif
      if(flen < 16)
      {  fclose(fp);
         sprintf(str,"%s.bak",fname);
         fp = fopen(str,"r");
         if(fp == NULL)
                 return 1;
      }
   }

   configNparR = 0;
   nstr = 0;
   do
   {  rc = ReadStr(fp, str, name, par);
      if(!rc)
	  {	rc1 = AnalizeRecodrRead(name,par);
	    if(!rc1) 
		{	configNparR++;
			nstr++;
		}
	  }
   } while(!rc);

   fclose(fp);
   if(nstr == 0)
	    return 2;
/*
	if(configversion == -1 || configversion != CONFIG_VERSION)
	{	configversion = CONFIG_VERSION;
		return 3;
    }
	if(configNparR != configNparW ||configNparR != configNparNeed)
	{	return 4;
	}
*/
	return 0;
}

int AutoConfig::RenameToBak(char *fname)
{  int  flen, rc;
   FILE *fp;
   char fnameold[256], fnameold2[256];

#ifdef _WIN32
   fp = fopen(fname,"r");
   if(fp)
   {  flen = _filelength(_fileno(fp));
      fclose(fp);
   } else {
      flen = 0;
   }
#else
		flen = file_length(fname);
#endif

   if(flen > 16 ) /* переименовываем файл */
   {  sprintf(fnameold2,"%s.bak2",fname);
      sprintf(fnameold,"%s.bak",fname);
      rc=remove(fnameold2);
      rc =rename(fnameold,fnameold2);

      if(rc)
      {   remove(fnameold);
      }
      rename(fname,fnameold);
   }
   return 0;
}

int AutoConfig::ReadStr(FILE *fp, char *str, char *name, char *par )
{  int i,iscomment,l;
   char *pstr;
/* читаем строку */
M: pstr= fgets(str,128,fp);
   if(pstr == NULL) return 1; //EOF
/* игнорируем строки нулевой длины */
   l = strlen(str);
   if(str[l-1] == '\n')
   {  str[--l] = 0;
   }
   if(l == 0) goto M;
/* игнорируем комментарии */
   iscomment = 0;
   for(i=0;i<l;i++)
   { if(str[i] > 32)
     {   if(str[i] == ';') iscomment = 1;
         break;
     }
   }
   if(iscomment) goto M;
   pstr = strstr(str,"=");
   if(pstr == NULL)
        goto M; // игнорируем строки без "="
   *pstr = 0;
//пропускаем пробелы
   do
   { pstr++;
     if(*pstr != ' ')
		  break;
   } while(pstr);

   strcpy(par,pstr);      // читаем параметры
   sscanf(str,"%s",name);
   return 0;
}
