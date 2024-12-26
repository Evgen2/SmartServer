// Server2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SmartServer.hpp"

extern void CloseTCPIP(void);
void CL_cleanup(void);

const char ConfigFile[] = "SmartServer.cfg";
char * SmartServer::Appname = (char *)APPLICATION_NAME;
int  SmartServer::Version = SERVER_VERSION;
int  SmartServer::SubVersion = SERVER_SUBVERSION;
int  SmartServer::SubVersion1= SERVER_SUBVERSION1;
int  SmartServer::IdNumber = 0;

const char * SmartServer::BuildDate = __DATE__;

SmartServer server;   

int main(int npar, char* argv[])
{	int rc;
#ifdef _WIN32
rc = SetConsoleCP(1251);
rc = SetConsoleOutputCP(1251);
#endif
   atexit(CL_cleanup);

	rc = server.init((char *)ConfigFile);
	if(rc == 0)
		server.Start();

	return 0;
}


void CL_cleanup(void)
{   int rc;

#ifdef _WIN32
    rc = SetConsoleCP(866);
#endif

    CloseTCPIP();
}

/* Find controller*/
int FindSmartTherm(unsigned char mac[6])
{	int i,rc=-1;
	for(i=0; i<server.Nsmt; i++)
	{	if(!memcmp(server.smt[i].mac, mac,6))
		{  return i;
		}
	}
	return rc;
}

/* Find controller*/
int FindSmartThermId(int _id)
{	int i,rc=-1;
	for(i=0; i<server.Nsmt; i++)
	{	if(_id == server.smt[i].ClientId)
		{  return i;
		}
//todo use	int ClientId_k;  
	}
	return rc;
}

/* Find Application*/
int FindSmartApp(unsigned char mac[6])
{	int i,rc=-1;
	for(i=0; i< server.Nsma; i++)
	{	if(!memcmp(server.sma[i].mac, mac,6))
		{  return i;
		}
	}
	return rc;
}

/* Find App */
int FindSmartAppId(int _id)
{	int i,rc=-1;
	for(i=0; i<server.Nsma; i++)
	{	if(_id == server.sma[i].ClientId)
		{  return i;
		}
//todo use	int ClientId_k;  
	}
	return rc;
}

SmartDevice_stub *AddSmartTherm(unsigned char mac[6], char *_ipaddr)
{	SmartDevice_stub *pst;
	if(server.Nsmt <MAX_DEVICES)
	{	server.smt[server.Nsmt].type = 1;
		server.smt[server.Nsmt].TCPserver_repot_period = server.controller_report_period;
		memcpy(server.smt[server.Nsmt].mac, mac,6);
		server.smt[server.Nsmt].MakeClientId(server.Nsmt);
		pst = &server.smt[server.Nsmt];
		pst->sts = STS_INIT;
		strcpy(pst->ipaddr, _ipaddr);
		server.Nsmt++;
		server.need_save_smt = 1;

		return pst;
	} else {
		return NULL;
	}
}

SmartDevice_stub *GetSmartDev(int _ind, int idclient, char *_ipaddr, int mode)
{	SmartDevice_stub *pst;
	int ind;
	if(_ind & 0x80000000)
	{	ind = _ind & (~ 0x80000000);
		if(ind>=0 && ind < server.Nsma)
		{	pst = &server.sma[ind];
			if(pst->ClientId != _ind)
				server.sclient[idclient].Log(5, "Warning: GetSmartDevA _ind %x  pst->ClientId %x\n", _ind, pst->ClientId);
			if(mode)
				strcpy(pst->ipaddr, _ipaddr);
			else if(strcmp(pst->ipaddr, _ipaddr)) 
			{	server.sclient[idclient].Log(3, "Warning: GetSmartDevA current IP addr %s != prev \n", _ipaddr, pst->ipaddr);
//				return NULL;
			}
			return pst;
		} else {
			return NULL;
		}
	} else {
		if(_ind >=0 && _ind < server.Nsmt)
		{	pst = &server.smt[_ind];
			if(pst->ClientId != _ind)
				server.sclient[idclient].Log(5, "Warning: GetSmartDevD _ind %x  pst->ClientId %x\n", _ind, pst->ClientId);
			if(mode)
				strcpy(pst->ipaddr, _ipaddr);
			else if(strcmp(pst->ipaddr, _ipaddr)) 
			{	server.sclient[idclient].Log(3, "Warning: GetSmartDevD current IP addr %s != prev %s\n", _ipaddr, pst->ipaddr);
				strcpy(pst->ipaddr, _ipaddr);
				server.need_save_smt++;
			//	return NULL;
			}
			return pst;
		} else {
			return NULL;
		}
	}
}


SmartDevice_stub *AddSmartApp(unsigned char mac[6], char *_ipaddr,  int ind)
{	SmartApp *psa;
	if(server.Nsma <256)
	{	server.sma[server.Nsma].type = 2;
		server.sma[server.Nsma].TCPserver_repot_period = server.controller_report_period;
		memcpy(server.sma[server.Nsma].mac, mac,6);
		server.sma[server.Nsma].MakeClientId(server.Nsma);
		psa = &server.sma[server.Nsma];
		psa->pSmt =&server.smt[ind];
		strcpy(psa->ipaddr, _ipaddr);
		psa->sts = STS_INIT;
		server.Nsma++;
		return (SmartDevice_stub *)psa;
	} else {
		return NULL;
	}
}

SmartDevice_stub *GetSmartAdd(int ind)
{	SmartDevice_stub *pst;
	if(ind>=0 && ind <256)
	{	pst = &server.sma[ind];
		return pst;
	} else {
		return NULL;
	}
}

