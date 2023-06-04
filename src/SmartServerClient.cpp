/* SmartServerClient.cpp */
#include "stdafx.h"
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include <math.h>

#include "UDPconnection.hpp"
#include "SmartServer.hpp"
#include "SmartHosts.hpp"

extern class my_hosts myhosts;
volatile int tcp_connect_on;

int SmartServerTCPUDP::Start(void)
{	int rc, l, lsend=0, i, istimeout=0;
	int t0;
    socklen_t addr_len;
    int  tclockOld;

	for(i=0; i<myhosts.n;i++)
	{	strcpy(client[i].ipaddr,myhosts.host[i].IPaddr);
		client[i].port = myhosts.hostport;
		client[i].pserv = pserv;
		rc = client[i].Start_Thread();
		Sleep(10);
	}
	if(type == 2)
	{
 		printf("SmartServer start in udp mode\n");
		rc = udpserver.createUDPserverconnection(port, 3000, 500);
	}	else {
 		printf("SmartServer start in tcp mode\n");
		rc = tcpserver.createTCPserverconnection(port, 3000, 500,1);
	}
    if(rc)
       return 0x12;
	l = 500;
	sts = 1;
/******************************************************************************/
if(type == 2)
{	for(;;)
	{   
		rc = udpserver.Read(ReadBuff, l);
	     if(rc == -3)
			    continue;
	     if(rc < 0)
			    return -1;
	     if(rc)
		 { char IPaddr[20];
		    strcpy(IPaddr, inet_ntoa(udpserver.client.sin_addr)); 
			for(i=0; i<myhosts.n;i++)
			{  if(!strcmp(IPaddr,client[i].ipaddr))
					client[i].tlastc = time(NULL);
			}

// 		wprintf(L"Read %i\n", rc);
			 
			rc= ParseFromUDP(ReadBuff, rc);
		 } else {
			for(i=0; i<myhosts.n;i++)
			{	if(client[i].online && difftime(time(NULL), client[i].tlastc) > 120)
				{ client[i].online = 0;
					rc = client[i].Start_Thread();
					Sleep(10);
				}
			}

			 Sleep(10);
		 }

	}
}
/***************************************************************/
if(type == 4)
{	t0 = clock();
	rc = listen(tcpserver.sock0,5);
	     if (rc == -1)
     { 
#ifdef _WIN32
      printf("listen error = %d\n", WSAGetLastError() );
       closesocket(tcpserver.sock0);
#else
			 perror("listen");
#endif
                exit(1);
      }

	for(;;)
	{ 
		{		tcp_connect_on = 0;
                addr_len = sizeof (struct sockaddr_in);
				tcpserver.sock = accept(tcpserver.sock0, (struct sockaddr *) &tcpserver.client, &addr_len);
                if (tcpserver.sock == -1)
				{
#ifdef _WIN32
			      printf("accept(), error = %d\n", WSAGetLastError() );
#else
					perror("accept");
#endif
				   closesocket(tcpserver.sock);
				   continue;
				}
				rc = tcpserver.Read(ReadBuff, l, 5000); /* wait for client */
				if(rc > 0)
				{	char IPaddr[20];
					strcpy(IPaddr, inet_ntoa(tcpserver.client.sin_addr)); 
					for(i=0; i<myhosts.n;i++)
					{  if(!strcmp(IPaddr,client[i].ipaddr))
							client[i].tlastc = time(NULL);
					}
					tcp_connect_on = 1;
					tcpserver.timeLastRW = clock();
					rc= ParseFromTCP(ReadBuff, rc,  IPaddr, lsend);
					if(rc == 0 && lsend > 0)
					{  rc = tcpserver.Send(WriteBuff, lsend);
					}
				   closesocket(tcpserver.sock);
			tclockOld = GetClock();
		    istimeout=0;
				   continue;
				} else {
					closesocket(tcpserver.sock);
					tcp_connect_on = 0;
					continue;
				}
/*******************************************/
//	      printf("client connect todo\n");

		}
	}
/***************************************************************/
}
// 		wprintf(L"todo %S\n", __FUNCTION__);

	return 0;
}

int SmartServerTCPUDP::ParseFromUDP(char *buf, int nb)
{   struct  Msg1 *pucmd;
    int cmd, rc;
	if(nb > sizeof(struct  Msg1))
		return -1;
	if(nb <= 0)
		return -2;
    pucmd = (struct  Msg1 *)buf;
	if(pucmd->cmd0 != 0x22)
		return 1;
	cmd = pucmd->cmd;

	switch(cmd)
	{
		case MCMD_OT_INFO:
// 		wprintf(L"Get CMD_OT_INFO in %i %S\n", cmd, __FUNCTION__);
		rc = OpenThermInfo( pucmd->Buf, nb-6);

	break;

		default:
 		printf("Unknown cmd %i in %s\n", cmd, __FUNCTION__);
		return 2;
	}
	return 0;
}

int SmartServerTCPUDP::ParseFromTCP(char *buf, int nb, char IPaddr[20], int &Lsend)
{   struct  Msg1 *pucmd;
    int cmd, rc;
	if(nb > sizeof(struct  Msg1))
		return -1;
	if(nb <= 0)
		return -2;
    pucmd = (struct  Msg1 *)buf;
	if(pucmd->cmd0 != 0x22)
		return 1;
	cmd = pucmd->cmd;

	switch(cmd)
	{
		case MCMD_OT_INFO:
	printf("Get CMD_OT_INFO from %s in %i %s\n", IPaddr, cmd,__FUNCTION__);
	rc = OpenThermInfo( pucmd->Buf, nb-6);
	{  int  rc, cmd, l;
   struct Msg1  MsgIn, MsgOut;

   l = 20;
   cmd = MCMD_DATA_FROM;
   l = 1;
   memcpy((void *)&MsgIn.Buf[0],(void *)&l, 4);
   l = 2;
   memcpy((void *)&MsgIn.Buf[4],(void *)&l, 4);
   l = 3;
   memcpy((void *)&MsgIn.Buf[8],(void *)&l, 4);
   l = 4;
   memcpy((void *)&MsgIn.Buf[12],(void *)&l, 4);
   l = 5;
   memcpy((void *)&MsgIn.Buf[16],(void *)&l, 4);
   Lsend =  20+6;
   memcpy((void *)WriteBuff,(void *)&MsgIn, Lsend);

/*
   rc = GetSts0(cmd, &MsgIn, 6+l, &MsgOut, 6);
*/
	}

	break;

		default:
 		printf("Unknown cmd %i in %s\n", cmd, __FUNCTION__);
		return 2;
	}
	return 0;
}

int SmartServerTCPUDP::SendCmdToTCP(char *buf, int nb, char IPaddr[20])
{
	return 0;
}


int SmartServerTCPUDP::OpenThermInfo(unsigned char *buf, int nb)
{
  unsigned char mac[6];
  int BoilerStatus;

  float t1;
  float t2;
  int status;
  unsigned int response;
  float BoilerT;
  float RetT;
  float dhw_t;
  float FlameModulation;
  float Pressure;
  unsigned char Fault;

static FILE *fpOT = NULL;
static int start = 1;
  if(start)
  {  fpOT = fopen("OpenTherm.log", "a");
     start = 0;
	 fprintf(fpOT,";date time time_sec mac(x) BoilerStatus(x) BoilerT RetT dhw_t FlameModulation Pressure ExTstatus(x) t1 t2\n");
  }
  memcpy((void *)mac,(void *)&buf[0], 6);
  memcpy((void *)&BoilerStatus,(void *)&buf[6], 4);

//  memcpy((void *)&response,(void *)&buf[10], 4);
  memcpy((void *)&BoilerT,(void *)&buf[10], 4);
  memcpy((void *)&RetT,(void *)&buf[14], 4);
  memcpy((void *)&dhw_t,(void *)&buf[18], 4);
  memcpy((void *)&FlameModulation,(void *)&buf[22], 4);
  memcpy((void *)&Pressure,(void *)&buf[26], 4);
  memcpy((void *)&status,(void *)&buf[30], 4);
  memcpy((void *)&t1,(void *)&buf[34], 4);
  memcpy((void *)&t2,(void *)&buf[38], 4);
//  memcpy((void *)&Fault,(void *)&buf[50], 1);

    struct tm *newtime;
    time_t ltime;
	 char curtime[128];

	time(&ltime);
    newtime = localtime(&ltime);
    sprintf( curtime,"%02d.%02d.%d %d:%02d:%02d",
          newtime->tm_mday,newtime->tm_mon+1,newtime->tm_year+1900,
		  newtime->tm_hour, newtime->tm_min, newtime->tm_sec);

	fprintf(fpOT, "%s %ld ", curtime, ltime);
	fprintf(fpOT, "%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4]);
	fprintf(fpOT, " %x ", BoilerStatus);

		printf("BoilerT=%.3f\n", BoilerT);
		printf("RetT=%.3f\n",  RetT);
		printf("dhw_t=%.3f\n", dhw_t);
		printf("FlameModulation=%.3f\n", FlameModulation);
		printf("Pressure=%.3f\n", Pressure);

	fprintf(fpOT, "%.3f %.3f %.3f %.3f %.3f ", BoilerT, RetT, dhw_t, FlameModulation, Pressure);
	fprintf(fpOT, "%x %.3f %.3f\n", status, t1, t2);
	fflush(fpOT);
	return 0;
}

#ifdef _WIN32
void ClientUDP_Start_Thread( void * lpParam );
#else
void *ClientUDP_Start_Thread( void * lpParam );
#endif

#ifdef _WIN32
void ClientUDP_Start_Thread( void * lpParam )
#else
void *ClientUDP_Start_Thread( void * lpParam )
#endif
{	SmartServerClientTCPUDP * scu;
	scu = (SmartServerClientTCPUDP *)lpParam;
	scu->Start();
}

int SmartServerClientTCPUDP::createConnection(int _type, int _timeout, int _timeoutAnswer, int verboze)
{	int rc=-1;
	timeout = _timeout;
	timeoutAnswer = _timeoutAnswer;

	if(_type == 1)
	{	rc = udp.createUDPconnection(ipaddr, port, timeout, timeoutAnswer, verboze);
		Ctype = 1;
	} else if(_type == 2) {
		rc = tcp.createTCPconnection(ipaddr, port, timeout, timeoutAnswer, verboze);
		Ctype = 2;
	}

	return rc;
}

int SmartServerClientTCPUDP::closeConnection(void)
{	int rc;
	if(Ctype == 1)		rc = udp.closeConnection();
	else if(Ctype == 2) rc = tcp.closeConnection();
	return rc;
}

int SmartServerClientTCPUDP::InitClientConnection(void)
{	int rc;
	if(Ctype == 1)		rc = udp.InitClientConnection();
	else if(Ctype == 2) rc = tcp.InitClientConnection(verbose);
	return rc;
}

int SmartServerClientTCPUDP::Start_Thread(void)
{	int id;
#ifdef _WIN32
       id =_beginthread(ClientUDP_Start_Thread, 0, (void *) this);
#else
		{	
			pthread_t thread;
            pthread_create(&thread, NULL, ClientUDP_Start_Thread, (void *) this);
		}

#endif

	return id;
}

int SmartServerClientTCPUDP::Start(void)
{   int rc;
   time_t tdev, thost;
/*********************************/
/* N/A рожаем мютексные семафоры */
/*********************************/

   printf("ip %s:\n",ipaddr);
				
   	rc = udp.createUDPconnection(ipaddr, port, 2500, 2300,0);
	if(rc == 0)
	{	do
		{	rc = udp.InitClientConnection();
			if(rc == 0)
				break;
			Sleep(1000+ udp.timeout);
		} while(1);
		if(rc == 0)
		{  double dt;
			online = 1;

			rc = Identify();
			if(rc)
			{    printf("ip %s, ОШИБКА идентификации %i \n",ipaddr, rc);
					online = 0;
					return 3;
			}
			rc = GetTime(&tdev);
			if(rc)
			{	rc = GetTime(&tdev);
				if(rc)
				{    printf("ip %s, ОШИБКА при запросе времени RTC %i\n",ipaddr, rc);
					online = 0;
					return 2;
				}
			}
			thost = time(NULL);
			tlastc = thost;
			online = 1;
			printf("Host time: %s",ctime(&thost));
			printf("Device time: %s",ctime(&tdev));
			dt = difftime(thost, tdev);
			if(dt > 1)
			{	struct _timeb tstruct;
/*			    
struct timeb {
        time_t time;
        unsigned short millitm;
        short timezone;
        short dstflag;
        };
*/
			
 // Print additional time information.
			   _ftime( &tstruct ); // C4996
/*
				wprintf(L"Set device time to host time\n");

				wprintf(L"Sizeof tstruct %i\n", sizeof(tstruct));
				wprintf(L"Sizeof __timeb32 %i\n", sizeof(__timeb32));
				wprintf(L"Sizeof __timeb64 t%i\n", sizeof(__timeb64));
*/
				rc = SetTime(&tstruct);
				rc = GetTime(&tdev);
				printf("Device time: %s\n",ctime(&tdev));
			}
			
/*
	l=sizeof(HAND_SHAKE_INP);
    ucmd.cmd = CMD_GETTIME;
    ucmd.cmd0 =  0xfe;
    indcmd = (indcmd+1)&0xffff;
    ucmd.ind =  indcmd;
	memcpy(ucmd.Buf, HAND_SHAKE_INP,l);
	l += sizeof(short int)*3+1; //17

    rc = SendAndConfirm((char *)&ucmd, l, (char *)&outcmd, l,0);

    if(rc == 0)
*/
//	 		wprintf(L"OK todo %S\n", __FUNCTION__);
			rc= SetUDPServerInfo(1, pserv->udp_port, 10000);

		} else {
	 		printf("Err todo %s\n", __FUNCTION__);
		}

	}
    udp.closeConnection();


	return 0;
}

//MCMD_SETTIME
int SmartServerClientTCPUDP::SetTime(struct _timeb *t0)
{  int  rc,  cmd, l;
   struct Msg1  MsgIn, MsgOut;


   l = sizeof(struct _timeb);
   cmd = MCMD_SETTIME;
   memcpy((void *)&MsgIn.Buf[0],(void *)t0, l);
   if(l == 12)
	   l = 16;
   rc = GetSts0(cmd, &MsgIn, 6+l, &MsgOut, 6);

   return rc;
}

//MCMD_GETTIME
int SmartServerClientTCPUDP::GetTime(time_t *t0)
{  int  rc, cmd, l;
   struct Msg1  MsgIn, MsgOut;
#ifdef _WIN32
   __time32_t tt32;
   __time64_t tt64;
#else
    unsigned int tt32;
	unsigned long int tt64;
#endif
    time_t ltime;

   l = sizeof(tt64) + 2;

    cmd = MCMD_GETTIME;
    rc = GetSts0(cmd, &MsgIn, 6, &MsgOut, 6+l);

	if(rc == 0)
   {  memcpy((void *)&l,(void *)&MsgOut.Buf[0], 2);
      if(l != 4 && l != 8)
	  {		printf("Warning: controller sizeof(time_t) = %i\n",l);
	       return 2;
	  }
      if(l == 4)
	  {	  memcpy((void *)&tt32,(void *)&MsgOut.Buf[2], l);
		  ltime = tt32;
	  } else {
		  memcpy((void *)&tt64,(void *)&MsgOut.Buf[2], l);
		  ltime = tt64;
	  }
{
    struct tm *newtime;

    newtime = localtime(&ltime);
    printf("%02d.%02d.%d %d:%02d:%02d\n",
          newtime->tm_mday,newtime->tm_mon+1,newtime->tm_year+1900,
		  newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
}


////////////////////
	  *t0 = ltime;
//      mktime(t0); //потому как может оказаться 30 февраля
   }
  return rc;
}

//CMD_IDENTIFY
int SmartServerClientTCPUDP::Identify(void)
{  int  rc,  cmd, l;
   struct Msg1  MsgIn, MsgOut;

   l = sizeof(struct _timeb);
   cmd = MCMD_IDENTIFY;

   rc = GetSts0v(cmd, (struct Msg2 *)&MsgIn, 6,  (struct Msg2 *)&MsgOut, sizeof(MsgOut), 6, 1);
	if(rc == 0)
	{  l = *((short int *)&MsgOut.Buf[0]);
	   udp.IdCode = *((int *)&MsgOut.Buf[2]);
	   udp.IdNum  = *((int *)&MsgOut.Buf[6]);
       memcpy((void *)udp.IdName,(void *)&MsgOut.Buf[10], l-sizeof(int)*2);
	}

  return rc;
}


//MCMD_SET_UDPSERVER
int SmartServerClientTCPUDP::SetUDPServerInfo(int _sts, int hport, int infotime)
{  int  rc,  cmd, l;
   struct Msg1  MsgIn, MsgOut;

   l = sizeof(int)*3;
   cmd = MCMD_SET_UDPSERVER;
   memcpy((void *)&MsgIn.Buf[0],(void *)&_sts, 4);
   memcpy((void *)&MsgIn.Buf[4],(void *)&infotime, 4);
   memcpy((void *)&MsgIn.Buf[8],(void *)&hport, 4);
   rc = GetSts0(cmd, &MsgIn, 6+l, &MsgOut, 6);

   return rc;
}


//MCMD_SET_TCPSERVER
int SmartServerClientTCPUDP::SetTCPServerInfo(int _sts, char ipaddr[32], int hport, int infotime)
{  int  rc, cmd, l;
   struct Msg1  MsgIn, MsgOut;

   l = sizeof(int)*3+20;
   cmd = MCMD_SET_TCPSERVER;
   memcpy((void *)&MsgIn.Buf[0],(void *)&_sts, 4);
   memcpy((void *)&MsgIn.Buf[4],(void *)ipaddr, 20);
   memcpy((void *)&MsgIn.Buf[24],(void *)&infotime, 4);
   memcpy((void *)&MsgIn.Buf[28],(void *)&hport, 4);
   rc = GetSts0(cmd, &MsgIn, 6+l, &MsgOut, 6);

   return rc;
}

    
//MCMD_GETDATA
int SmartServerClientTCPUDP::GetData(void)
{  int  rc,  cmd, l;
   struct Msg1  MsgIn, MsgOut;


   l =  6 + sizeof(int)*2 + sizeof(float)*7; 

    cmd = MCMD_GETDATA;
    rc = GetSts0(cmd, &MsgIn, 6, &MsgOut, 6+l);

	if(rc == 0)
   {	
	   memcpy((void *)Mac,(void *)&MsgOut.Buf[0], 6);
	   memcpy((void *)&BoilerStatus,(void *)&MsgOut.Buf[6], 4);
	   memcpy((void *)&BoilerT,(void *)&MsgOut.Buf[10], 4);
	   memcpy((void *)&RetT,(void *)&MsgOut.Buf[14], 4);
	   memcpy((void *)&dhw_t,(void *)&MsgOut.Buf[18], 4);
	   memcpy((void *)&FlameModulation,(void *)&MsgOut.Buf[22], 4);
	   memcpy((void *)&Pressure,(void *)&MsgOut.Buf[26], 4);
	   memcpy((void *)&ExtTstatus,(void *)&MsgOut.Buf[30], 4);
	   memcpy((void *)&ExtT1,(void *)&MsgOut.Buf[34], 4);
	   memcpy((void *)&ExtT2,(void *)&MsgOut.Buf[38], 4);
   }
  return rc;
}

//MCMD_TESTCMD
int SmartServerClientTCPUDP::TestCmd(int id, int par)
{  int l, cmd, rc;
   struct Msg1  MsgIn, MsgOut;
   l = 4*2;
   cmd = MCMD_TESTCMD;
   memcpy((void *)&MsgIn.Buf[0],(void *)&id, 4);
   memcpy((void *)&MsgIn.Buf[4],(void *)&par, 4);
   
   rc = GetSts0(cmd, &MsgIn, 6+l, &MsgOut, 6);
  return rc;
}
//MCMD_TESTCMDANSWER
int SmartServerClientTCPUDP::TestCmdAnswer(int &responce, int &status)
{  int l, cmd, rc;
   struct Msg1  MsgIn, MsgOut;
   l = 4*2;
   cmd = MCMD_TESTCMDANSWER;
   
   rc = GetSts0(cmd, &MsgIn, 6, &MsgOut, 6+l);
   if(rc == 0)
   {	
	   memcpy((void *)&responce,(void *)&MsgOut.Buf[0], 4);
	   memcpy((void *)&status,(void *)&MsgOut.Buf[4], 4);
   }
  return rc;
}

/* lIn - длина исходящего сообщения, lOut - длина ответа */

int SmartServerClientTCPUDP::GetSts0(int cmd, struct Msg1  *MsgIn, int lIn, struct Msg1  *MsgOut, int lOut)
{  int  l, rc;

    udp.indcmd = (udp.indcmd+1)&0xffff;

    MsgIn->cmd0 = 0x22;
    MsgIn->cmd = cmd;
    MsgIn->ind = udp.indcmd;
    l = sizeof(*MsgIn);
	if(Ctype == 1)
		rc = udp.SendAndConfirm((char *)MsgIn, lIn, (char *)MsgOut, lOut, verbose);
	else
		rc = tcp.SendAndConfirm((char *)MsgIn, lIn, (char *)MsgOut, lOut, verbose);
//if(rc)
//   printf("****$$$***GetSts0 rc=%i\n",rc);
    if(rc == 1 || rc == 2)
    {   online = 0;
        return rc;
     } else
          online = 1;
    if(MsgOut->cmd0 != MsgIn->cmd0)
    {
        if(MsgOut->cmd0 & 0x8000 && (MsgIn->cmd0 == (MsgOut->cmd0&0xff)) )
                        return 0x12;
         return 0x10;
    }
    if(MsgOut->cmd != MsgIn->cmd)
    {
		if(Ctype == 1)
			rc = udp.SendAndConfirm((char *)MsgIn, lIn, (char *)MsgOut, lOut, verbose);
		else
			rc = tcp.SendAndConfirm((char *)MsgIn, lIn, (char *)MsgOut, lOut, verbose);
        if(rc == 1)
        {   online = 0;
          return rc;
        } else
          online = 1;
    }

    if(MsgOut->cmd != MsgIn->cmd)
          return 0x11;
    return 0;
}

/* lIn - длина исходящего сообщения, lOut - макс длина ответа */
// общая длина ответа в байтах рассчитывается по формуле
//  la = (int) *((short int *) &buff_out[shiftLansw]);
//  la = la * size0 + shiftLansw + 2;
int SmartServerClientTCPUDP::GetSts0v(int cmd, struct Msg2  *MsgIn, int lIn, struct Msg2 *MsgOut, int lOut, int shiftLansw, int size0)
{  int l, rc;

    udp.indcmd = (udp.indcmd+1)&0xffff;

    MsgIn->cmd0 = 0x12;
    MsgIn->cmd = cmd;
    MsgIn->ind = udp.indcmd;
    l = sizeof(*MsgIn);

	rc =   udp.SendAndConfirm2((char *)MsgIn, lIn, (char *)MsgOut, lOut, shiftLansw, size0,0x12, 1);

    if(rc == 1)
    {   online = 0;
        return rc;
    }
    if(MsgOut->cmd0 != MsgIn->cmd0)
    {
        if(MsgIn->cmd0 & 0x8000 && (MsgOut->cmd0 == (MsgIn->cmd0&0xff)) )
                        return 0x12;
         return 0x10;
    }
    if(MsgOut->cmd != MsgIn->cmd)
    {
		rc =    udp.SendAndConfirm2((char *)MsgIn, lIn, (char *)MsgOut, lOut, shiftLansw, size0,0x12,1);

        if(rc == 1)
        {   online = 0;
           return rc;
        } else 
          online = 1;
    }
    if(MsgOut->cmd != MsgIn->cmd)
          return 0x11;
    return 0;
}



int SmartServerClientTCPUDP::InformHost_on_ServerState(char IPaddr[20], int hport, int state)
{	int rc = 0;
   class UDPconnection uclient;

	rc = uclient.createUDPconnection(IPaddr, hport, 2000, 1700,0);
	if(rc == 0)
	{	rc = uclient.InitClientConnection();
		if(rc == 0)
		{
	 		printf("OK todo %s\n", __FUNCTION__);
		}

	}
    uclient.closeConnection();

	return rc;
}
