/* SmartServer.cpp */

#include "stdafx.h"
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

#include "UDPconnection.hpp"
#include "TCPconnection.hpp"
#include "SmartServer.hpp"
#include "SmartHosts.hpp"

int InitWinsok(void);
int ShowMyIp(void);
//void sgLogError(char *format, ...);

#ifdef _WIN32
extern void TCPUDP_LServer( void * lpParam ); /* tcpudp_server thread  */
#else
extern void *TCPUDP_LServer( void * lpParam );
long file_length(char *f);
#endif


static int configNparW=0,  configNparR=0, configNparNeed=0;
#define CFG_AddV(form, name, type) cfg.AddV(#name, form, &name, type)
#define CFG_AddS(form, name, _nb)  cfg.AddStr(#name, form, &name, _nb)

int SmartServer::init(char *ConfigFile)
{	int rc, need;
//	rc = cfg.Add(NULL,";%s:", Appname, 1);
	configNparW = 0;
	cfg.AddC(";%s v %s %s\n", (char *)Appname, (char *)Version, (char *)SubVersion, NULL);
	cfg.AddC(";configversion\n",  NULL);
	if(configversion == -1)
	   configversion = CONFIG_VERSION;

    CFG_AddV("%d\n",configversion, _INT); configNparW++;
    cfg.AddC(";************ Set parameters here ****************\n",  NULL);
    cfg.AddC(";server type:  -1 - uninit, 2 - UDP, 4 - TCP; 6 = UDP+TCP,  7 = COM+UDP+TCP\n",  NULL);
    CFG_AddV("%i\n", type, _INT);  configNparW++;
    cfg.AddC(";mode:  1 - client, 2 - server, 3 - client, then server\n",  NULL);
    CFG_AddV("%i\n", mode, _INT);  configNparW++;
    cfg.AddC(";ip адрес контроллера, для подключения PC как клиент\n",  NULL);
    CFG_AddS("%s\n", pc_client.ipaddr, sizeof(pc_client.ipaddr));  configNparW++;
    cfg.AddC(";порт контроллера, для подключения PC как клиент\n",  NULL);
    CFG_AddV("%i\n", pc_client.port, _INT);  configNparW++;
    cfg.AddC(";remote server ip address\n",  NULL);
    CFG_AddS("%s\n", Server_ipaddr, sizeof(Server_ipaddr));  configNparW++;
	cfg.AddC(";server ports (int)\n",  NULL);
    CFG_AddV("%i\n", udp_port, _INT);  configNparW++;
    CFG_AddV("%i\n", tcp_port, _INT);  configNparW++;

   cfg.AddC(";*********************************\n",  NULL);
   cfg.AddC(";Last parameter in config: number of parameters. Don't change with hands\n",  NULL);
   CFG_AddV("%d\n", configNparW, _INT);  configNparW++;

   configNparNeed = configNparW;

//	rc  = cfg.Write(ConfigFile);
    need = 0;
	rc  = cfg.Read(ConfigFile);
	if(rc)
	{  if(rc == 1)
				printf("Config %s not found\n", ConfigFile);
		need = 1;
	} else {
		if(configversion == -1 || configversion != CONFIG_VERSION)
		{	configversion = CONFIG_VERSION;
			printf("Old version of config file\n");
			need = 1;
		} else 
			if(cfg.configNparR != configNparW || cfg.configNparR != configNparNeed)
		{
			printf("Numer of parameters in config file changed\n");
			need = 1;
			configNparW = configNparNeed;
		}
	}

	if(need)
	{	printf("Default config file %s is written\nPlease restart programm\n",ConfigFile);
		RenameToBak(ConfigFile);
		rc = cfg.Write(ConfigFile);
		exit(0);
	}

	return 0;
}


int SmartServer::Start(void)
{   uintptr_t hid;
	void *ptr;
	int rc;

	ptr = this;
	if(type & 0x6)
	{	rc = InitWinsok();
		if(rc)
			return 1;
	}
	if(type & 0x06)
	{	
#ifdef _WIN32
		hid = _beginthread(TCPUDP_LServer,0,(void *) ptr);
#else
		{	
			pthread_t thread;
            pthread_create(&thread, NULL, TCPUDP_LServer, (void *)  ptr);
		}

#endif

		Sleep(100);
		while (server.sts == 0);
		{  Sleep(1000);
		}
	}

	while (server.sts == 1);
	{  Sleep(1000);
	}

	return 0;
}

int InitWinsok(void)
{
#ifdef _WIN32
    WSADATA wsaData = {0};
	int  rc;
   // Initialize Winsock
    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc != 0) 
	{   printf("WSAStartup failed: %d\n", rc);
        return 1;
    }
#endif //_WIN32
	return 0;
}


/* TCPudp_server thread  */
#ifdef _WIN32
void TCPUDP_LServer( void * lpParam )
#else
void *TCPUDP_LServer( void * lpParam )
#endif
{	SmartServer *pss;
	int rc;
	 if(lpParam == NULL)
	 {	printf("Error: lpParam = NULL in %s\n", __FUNCTION__);
	    exit(1);
	 }

	pss =(SmartServer *)lpParam;
	if(pss->sts < 0) pss->sts = 0;

	 pss->server.sts = 0;  /* -1 - не инициализирован 0 - стартовал 1 - онлайн 2 закончился -2 критическая ошибка */
	 pss->server.type = pss->type;
	 if(pss->type == 2)
	 {	 pss->server.port = pss->udp_port;
	 } else if(pss->type == 4)
	 {	 pss->server.port = pss->tcp_port;
	 }
	 rc = pss->server.Start();

     printf("todo end %s\n", __FUNCTION__);
     pss->server.sts = 2;

}


int AutoConfig::AnalizeRecodrRead(char *name, char *par)
{  int i, rc,  is=0,npar=0;
   double dp;
   char str[256];
  for(i=0; i < n; i++)
  {	if(it[i].type == _COMMENT) continue;
  	if(it[i].type == _NUL) continue;
	if(it[i].name == NULL) continue;
    if(!strcmp(name,it[i].name))
      {  is = 1;
         npar = i;
         break;
      }
  }

  if(!is) return 1;
  switch(it[npar].type)
  {	case _INT:
		{  rc = sscanf(par,"%i", &i);
			if(rc == 1)
			{	*((int *)it[npar].ptr) = i;
			}
		 }

		break;
  	case _DOUBLE:
		 {  rc = sscanf(par,"%lf", &dp);
			if(rc == 1)
			{		*((double *)it[npar].ptr) = dp;
			}
		 }

		break;
  	case _STR:
		 {  rc = sscanf(par,"%s", str);
			if(rc == 1)
			{	if(it[npar].ptr)
				{	strncpy((char *)it[npar].ptr, str, it[npar].nb);
				} else {
					printf("it[%d].ptr==NULL\n",npar);
					exit(1);
				}
			}
		 }

		break;
	default:
		printf("Unknown type %d\n",it[npar].type);
		exit(1);
  }

  return 0;
}


int SmartServer::RenameToBak(char *fname)
{  int  flen, rc;
   FILE *fp;
   const char *prefix= "prg.";
   char fnameold[256], fnameold2[256];
#if _WIN32
   fp = fopen(fname,"r");
   if(fp)
   {  flen = _filelength(_fileno(fp));
      fclose(fp);
   } else {
      flen = 0;
   }
#else 
   {	struct stat st;
		if (stat(fname, &st)) /*failure*/
			flen = 0; // when file does not exist or is not accessible
		else
			flen = st.st_size;
   }
#endif //_WIN32
   if(flen > 16 ) /* переименовываем файл */
   {  sprintf(fnameold2,"%s.bak2",fname);
      sprintf(fnameold,"%s.bak",fname);
      rc=remove(fnameold2);
//           printf("remove(%s) rc=%i\n",fnameold2,rc);
      rc =rename(fnameold,fnameold2);
//      printf("rename(%s,%s) rc=%i\n",fnameold,fnameold2,rc);

      if(rc)
      {   remove(fnameold);
//          printf("remove(%s) rc=%i\n",fnameold,rc);
      }
      rename(fname,fnameold);
//      printf("rename(%s,%s) rc=%i\n",fname,fnameold,rc);
   }
   return 0;
}

#ifndef _WIN32

long file_length(char *f)
{
    struct stat st;
    stat(f, &st);
    return st.st_size;
}

#endif

