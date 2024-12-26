/* SmartServer.cpp */
#include "stdafx.h"
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/stat.h>

#include "SmartServer.hpp"

const char Controllers_File[] = "controllers.sts";

#ifdef _WIN32
#else
long file_length(char *f);
#endif


/*****************************vvvvv Config vvvvvvv*************************************/
#define CFG_AddV(form, name, type) cfg.AddV(#name, form, &name, type)
#define CFG_AddS(form, name, _nb)  cfg.AddStr(#name, form, &name, _nb)

int SmartServer::init(char *ConfigFile)
{	int rc, need;
static int configNparW=0,  configNparR=0, configNparNeed=0;
	char str[200];
	configNparW = 0;

	sprintf(str, "%s v %d.%d.%d build %s", (char *)Appname, Version, SubVersion, SubVersion1, (char *)BuildDate);
	cfg.AddC(";%s\n", str, NULL);
	cfg.AddC(";configversion\n",  NULL);
	if(configversion == -1)
	   configversion = CONFIG_VERSION;
    CFG_AddV("%d\n",configversion, _INT); configNparW++;
    cfg.AddC(";************ Set parameters here ****************\n",  NULL);
	cfg.AddC(";server port (int)\n",  NULL);
    CFG_AddV("%i\n", tcp_port, _INT);  configNparW++;
	cfg.AddC(";connection timeout, mc (int)\n",  NULL);
    CFG_AddV("%i\n", timeout, _INT);  configNparW++;
	cfg.AddC(";answer timeout, mc (int)\n",  NULL);
    CFG_AddV("%i\n", timeoutAnswer, _INT);  configNparW++;
	cfg.AddC(";verbose 0/1 (int)\n",  NULL);
    CFG_AddV("%i\n", verbose, _INT);  configNparW++;
	cfg.AddC(";default controller to server report period, sec\n",  NULL);
    CFG_AddV("%i\n", controller_report_period, _INT);  configNparW++;
	cfg.AddC(";default user online controller to server report period, sec\n",  NULL);
    CFG_AddV("%i\n", controller_user_online_report_period, _INT);  configNparW++;
	cfg.AddC(";default application ask server report period, sec\n",  NULL);
    CFG_AddV("%i\n", application_report_period, _INT);  configNparW++;
	

   cfg.AddC(";*********************************\n",  NULL);
   cfg.AddC(";Last parameter in config: number of parameters. Don't change with hands\n",  NULL);
   CFG_AddV("%d\n", configNparW, _INT);  configNparW++;

   configNparNeed = configNparW;
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
		cfg.RenameToBak(ConfigFile);
		rc = cfg.Write(ConfigFile);
		return 1;
	}

	Read_Smt();

	return 0;
}

/*****************************^^^^^ Config ^^^^^^**************************************/
extern int InitTCPIP(void);
extern void CloseTCPIP(void);

int SmartServer::Start(void)
{ 	void *ptr;
	int i;
	if(InitTCPIP())
		return 1;
	ptr = this;
	for(i=0; i < MAX_CLIENTS; i++) sclient[i].parent = this;
	for(i=0; i < MAX_DEVICES; i++) smt[i].parent = this;
	for(i=0; i < MAX_APPLICATIONS; i++) sma[i].parent = this;

	sts = -1;
	t_start = time(NULL);

	Server();
	
	exit(1);

	return 0;
}

extern int ReadStr(FILE *fp, char *str, char *nameClass, char *name, char *par );

int SmartServer::Read_Smt(void)
{	FILE *fp;
    char str[256], nameClass[128],name[128], par[128];
	int i, rc, n;

    printf("SmartServer::Read_Smt\n");
	need_save_smt = 0;
	fp = fopen(Controllers_File,"r");
	if(fp == NULL)
	   return 1;
	n = -1;
    do
    {  rc = ReadStr(fp, str, nameClass,name,par);
      if(!rc)
      {  // if(!_wcsicmp(nameClass,L"prg"))
        //    AnalizeRecodrRead(name,par);
		  if(!strcmp(nameClass, "ss"))
			  AnalizeRecodrRead(name, par);
		  else if(!strcmp(nameClass, "smt"))
		  { if(!strcmp(name, "MAC"))
			{	n++;
				if(n >= Nsmt)
					break;
				smt[n].AnalizeRecodrRead(name, par);
			} else if(n>=0) {
				smt[n].AnalizeRecodrRead(name, par);
			}
		  }

      }
    } while(!rc);


	fclose(fp);

	for(i=0; i<Nsmt; i++)
	{	smt[i].TCPserver_repot_period = controller_report_period;
		smt[i].sts = STS_INIT;
	}

	return 0;
}

int SmartServer::Write_Smt(void)
{	FILE *fp;
	int i;
	printf("SmartServer::Write_Smt\n");
	need_save_smt = 0;
   
	fp = fopen(Controllers_File,"w");
	if(fp == NULL)
	   return 1;
	fprintf(fp,";Controllers\n");
	fprintf(fp,"ss.Nsmt=%d\n", Nsmt);
	for(i=0; i<Nsmt; i++)
	{
		fprintf(fp,";%d\n",i);
		smt[i].write(fp);
	}

	fclose(fp);
//
	return 0;
}

int SmartServer::AnalizeRecodrRead(char *name, char *par)
{  int i,is=0,npar=0, rc;

static const char *lprgsNames[]=
{ "Nsmt",  "todo", "todo1", "todo2", "todo3",
  NULL
};


  for(i=0;lprgsNames[i];i++)
  {
      if(!strcmp(name,lprgsNames[i]))
      {  is = 1;
         npar = i;
         break;
      }
  }
  if(!is) return 1;
  switch(npar)
  {  case 0:  /* Nsmt */
      { int v;
        rc = sscanf(par,"%i",&v);
		if(rc > 0)
				Nsmt = v;
	  }
       break;

     case 1: 
/*
      { int tmp;
        rc = sscanf(par,"%i",&tmp);
		if(rc > 0)
			baudrate = tmp;
      }
*/	   
       break;
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

