/* SmartDevice.cpp */

#include "stdafx.h"
#include "SmartServer.hpp"

int SmartTerm::loop1(struct Msg1 *out, int &outb)
{	int rc = 1; 
	SmartTermSTS ssts;
// 	printf("SmartTerm::loop sts %d\n", sts);
	tlastc = time(NULL);
	ssts = sts;
	switch(ssts)
	{
		case STS_INIT:
			ask_sts(out, outb);
			sts = ASK_STS;
			rc = 0;
			break;
		case  ASK_STS:
//			ask_sts(out, outb);
			sts = STS_REPORT;
//			sts = SET_STS;
			rc = 0;
			break;
		case  SET_STS:
			sts = STS_REPORT;
			break;
		case  STS_REPORT:
			sts = STS_REPORT_ANSWER;
			break;
		case STS_REPORT_ANSWER:
			sts = STS_REPORT;
			break;
		case  STS_ERROR:
			sts = STS_PAUSE;
			break;
		case  STS_PAUSE:
			sts = STS_PAUSE; //todo
			break;
	}
	return rc;
}

//MCMD_SET_TCPSERVER
int SmartTerm::set_sts(struct Msg1 *out, int &outb)
{   short int nonclose_connect;
	int srp;
	out->cmd = MCMD_SET_TCPSERVER;
	nonclose_connect = 0;
	srp = GetReportPeriod();
	memcpy(&out->Buf[0], &srp,4);
   Log(10, "Use 1 TCPserver_repot_period =%d\n", srp);

	memcpy(&out->Buf[4], &nonclose_connect,2);
	outb = 6+4+2;

	return 0;
}
//MCMD_SET_TCPSERVER

//SCMD_GET_STS
int SmartTerm::ask_sts(struct Msg1 *out, int &outb)
{	int rc = 0, srp;
    short int nonclose_connect;
	static int raz = 0;
	time_t tnow;
	tnow = time(NULL);

//printf("SmartTerm::ask_sts  at %s", ctime(&tnow));
	out->cmd = SCMD_GET_STS;
	nonclose_connect = 1;
	srp = GetReportPeriod();
	memcpy(&out->Buf[0], &srp,4);
   Log(10, "Use 2 TCPserver_repot_period =%d\n", srp);
//	memcpy(&out->Buf[4], &nonclose_connect,2);
	outb = 6+4;

	return rc;
}

//CCMD_SEND_STS
//SCMD_GET_STS	answer to get controller sts 
int SmartTerm::callback_Get_Sts(struct Msg1 *in, int inb, struct Msg1 *out, int &outb)
//int SmartTerm::callback_Get_Sts(struct Msg1 *in, int inb)
{	int rc = 0, _id;
#ifdef _WIN32
   __time32_t tt32;
//   __time64_t tt64;
#else
    unsigned int tt32;
//	unsigned long int tt64;
#endif
	time_t tnow;
	tnow = time(NULL);
//printf("get %d, scallback_Get_Sts answer at %s", inb, ctime(&tnow));
	memcpy((void *)&_id,(void *)&in->Buf[0], 4);
	if(_id != ClientId)
	{		printf("!!!!_id %d != ClientId %d\n", _id, ClientId);
		return -1;
	}
/*
	memcpy((void *)_mac,(void *)&in->Buf[0], 6);
	if(memcmp(_mac, mac, 6))
	{
		printf("My  MAC %02x:%02x:%02x:%02x:%02x:%02x\n",mac[0], mac[1], mac[2], mac[3], mac[4],mac[5] );
		printf("Get MAC %02x:%02x:%02x:%02x:%02x:%02x\n",_mac[0], _mac[1], _mac[2], _mac[3], _mac[4],_mac[5] );
		return -1;
	}
	*/

	memcpy((void *)&B_flags,(void *)&in->Buf[6], 2);
	memcpy((void *)&stsOT,  (void *)&in->Buf[8], 2);
	memcpy((void *)&tt32,   (void *)&in->Buf[10], 4);
	t_lastwork = tt32;
	memcpy((void *)&BoilerStatus,	(void *)&in->Buf[14], 4);
	memcpy((void *)&BoilerT,	(void *)&in->Buf[18], 4);
	memcpy((void *)&RetT,		(void *)&in->Buf[22], 4);
	memcpy((void *)&Tset,		(void *)&in->Buf[26], 4);
	memcpy((void *)&Tset_r,		(void *)&in->Buf[30], 4);
	memcpy((void *)&dhw_t,		(void *)&in->Buf[34], 4);
	memcpy((void *)&TdhwSet,	(void *)&in->Buf[38], 4);

	memcpy((void *)&FlameModulation,	(void *)&in->Buf[42], 4);
	memcpy((void *)&Pressure,	(void *)&in->Buf[46], 4);
	memcpy((void *)&stsDS,		(void *)&in->Buf[50], 4);
	memcpy((void *)&t1,			(void *)&in->Buf[54], 4);
	memcpy((void *)&t2,			(void *)&in->Buf[58], 4);
	memcpy((void *)&Toutside,	(void *)&in->Buf[62], 4);
	memcpy((void *)&tempindoor,	(void *)&in->Buf[66], 4);
	memcpy((void *)&tempoutdoor,(void *)&in->Buf[70], 4);
	memcpy((void *)&TroomTarget,(void *)&in->Buf[74], 4);
	if(inb >= 92)
	{	if(StartSts == 0)
		{
			memcpy((void *)&InT,		(void *)&in->Buf[78], 4);
			memcpy((void *)&U0,			(void *)&in->Buf[82], 4);
//			printf("==>InT %f, U0 %f", InT, U0);
		}
	}

//printf("Get t1 %f t2 %f\n", t1, t2);

	return rc;
}


//CCMD_SEND_STS_S answer
int SmartTerm::callback_Send_Sts(struct Msg1 *in, int inb, struct Msg1 *out, int &outb)
{	int rc = 0, tmp;
    short int nonclose_connect;
	short int remote_cmd;
	time_t tnow;
	static int raz = 0;
	tnow = time(NULL);
	callback_Get_Sts(in, inb, NULL, tmp);
	out->cmd = CCMD_SEND_STS_S;
//printf("get %d, send CCMD_SEND_STS_S answer at %s", inb, ctime(&tnow));

    nonclose_connect = 1;

	tmp = GetReportPeriod();
	
	memcpy(&out->Buf[0], &tmp,4);
   Log(10, "Use 3 TCPserver_repot_period =%d\n", tmp);

	remote_cmd = 0;
	if(isCmd == 1)
			remote_cmd = 1;
	else if(isCmd == 0x10)
			remote_cmd = 0x10;
	memcpy(&out->Buf[4], &remote_cmd,2);
	switch(remote_cmd)
	{	case 0:
			outb = 6+4+2;
			break;
		case 1:
			memcpy(&out->Buf[6], &B_flags_toSet,2);
			memcpy(&out->Buf[8], &Tset_toSet,4);
			memcpy(&out->Buf[12], &TroomTarget_toSet,4);
			memcpy(&out->Buf[16], &TdhwSet_toSet,4);
			isCmd = 2;
			outb = 6+4*4+2*2;
			break;
		case 0x10:
			memcpy(&out->Buf[6], &InT,4);
			memcpy(&out->Buf[10], &U0,4);
			StartSts = 0;
//printf("<= InT %f, U0 %f", InT, U0);

			outb = 6+4*3 +2;
			isCmd = 0;
			break;
	}

	return rc;
}

// MCMD_GET_OT_INFO
int SmartTerm::callback_Get_OT_Info(struct Msg1 *in, int inb, struct Msg1 *out, int &outb)
{
//pucmd->Buf
 unsigned int tt32;

  memcpy((void *)mac,(void *)&in->Buf[0], 6);
//printf("callback_Get_OT_Info need redo !!!\n" );
//printf("MAC %02x:%02x:%02x:%02x:%02x:%02x\n",mac[0], mac[1], mac[2], mac[3], mac[4],mac[5] );

  memcpy((void *)&B_flags,(void *)&in->Buf[6], 2);
  memcpy((void *)&stsOT,(void *)&in->Buf[8], 2);
//2
  memcpy((void *)&tt32,(void *)&in->Buf[12], 4);
  t_lastwork = tt32;
#ifdef _WIN32
//printf("t_lastwork %x\n",t_lastwork );
#else
//printf("t_lastwork (lx) %lx\n",t_lastwork );
#endif
  memcpy((void *)&BoilerStatus,(void *)&in->Buf[20], 4); //20 = 12+8
//printf("BoilerStatus %x\n",BoilerStatus );
  memcpy((void *)&BoilerT,(void *)&in->Buf[24], 4); 
//printf("BoilerT %f\n",BoilerT );
  memcpy((void *)&RetT,   (void *)&in->Buf[28], 4);
//printf("RetT %f\n",RetT);
  memcpy((void *)&Tset,   (void *)&in->Buf[32], 4);
//printf("Tset %f\n",Tset);
  memcpy((void *)&Tset_r, (void *)&in->Buf[36], 4);
//printf("Tset_r %f\n",Tset_r);
  memcpy((void *)&dhw_t,  (void *)&in->Buf[40], 4);
//printf("dhw_t %f\n",dhw_t);
  memcpy((void *)&FlameModulation,  (void *)&in->Buf[44], 4);
//printf("FlameModulation %f\n",FlameModulation);
  memcpy((void *)&Pressure,  (void *)&in->Buf[48], 4);
//printf("Pressure %f\n",Pressure);
  memcpy((void *)&stsDS,  (void *)&in->Buf[52], 4);
  memcpy((void *)&t1,  (void *)&in->Buf[56], 4);
  memcpy((void *)&t2,  (void *)&in->Buf[60], 4);

	//todo
	return 0;
}

int ReadStr(FILE *fp, char *str, char *nameClass, char *name, char *par )
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
   strcpy(par,pstr+1);      // читаем параметры
   pstr = strstr(str,".");  // ищем точку для определения имени класса
   if(pstr == NULL)
   {
      nameClass[0] = 0;
      sscanf(str,"%s",name);
   } else {
      *pstr = 0;
      sscanf(str,"%s",nameClass);
      sscanf(pstr+1,"%s",name); // читаем имя параметра
   }
   return 0;
}

int SmartTerm::write(FILE *fp)
{
		fprintf(fp,"smt.MAC=%02x:%02x:%02x:%02x:%02x:%02x\n",mac[0], mac[1], mac[2], mac[3], mac[4],mac[5] );
		fprintf(fp,"smt.type=%d\n",type);
		fprintf(fp,"smt.ipaddr=%s\n",ipaddr);
		fprintf(fp,"smt.ClientId=%x\n",ClientId);
		fprintf(fp,"smt.ClientId_k=%x\n", ClientId_k);
		
		
		return 0;
}

int SmartTerm::AnalizeRecodrRead(char *name, char *par)
{  int i,is=0,npar=0, rc;

static const char *lprgsNames[]=
{ "MAC",  "type", "ipaddr", "ClientId", "ClientId_k",
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
  {  case 0:  /* MAC */
      { int imac[6];
        rc = sscanf(par,"%02x:%02x:%02x:%02x:%02x:%02x\n", &imac[0], &imac[1], &imac[2], &imac[3], &imac[4], &imac[5] );
		for(i=0; i<6; i++)
			mac[i] = imac[i];
	  }
       break;

     case 1:  /* type */
      { int tmp;
        rc = sscanf(par,"%d",&tmp);
		if(rc > 0)
			type = tmp;
      }  
       break;

     case 2:  /* ipaddr */
      { // int tmp;
		strcpy(ipaddr, par);
      }
        break;

	 case 3:  /* ClientId */
      { int tmp;
        rc = sscanf(par,"%x",&tmp);
		if(rc > 0)
			ClientId = tmp;
      }  
       break;
	 case 4:  /* ClientId_k */
      { int tmp;
        rc = sscanf(par,"%x",&tmp);
		if(rc > 0)
			ClientId_k = tmp;
      }  
       break;
  }

  return 0;
}


int SmartTerm::read(FILE *fp)
{	int rc;
	return 0;
}

int SmartApp::loop1( struct Msg1 *out, int &outb)
{	
	tlastc = time(NULL);
	return 0;
}

//ACMD_GET_STS_S
//
//pair to SmartTerm::callback_Get_Sts
int SmartApp::callback_get_sts(struct Msg1 *in, int inb, struct Msg1 *out, int &outb)
{	int rc = 0;
#ifdef _WIN32
   __time32_t tt32;
//   __time64_t tt64;
#else
    unsigned int tt32;
//	unsigned long int tt64;
#endif
	if(pSmt == NULL)
		return -1;
	outb = 6 + 80;	

	memcpy((void *)out,(void *)in,6); // out->cmd = ACMD_GET_STS_S

	memcpy((void *)&out->Buf[0], (void *)&pSmt->B_flags, 2);

//printf("** B_flags %x ", ((unsigned int)pSmt->B_flags));
//if(pSmt->B_flags & 0x0800)
//			printf(" ** PID USED\n");
//else
//			printf(" ** PID not USED\n");

	memcpy((void *)&out->Buf[2], (void *)&pSmt->stsOT, 2);
#ifdef _WIN32
   tt32 = (__time32_t )pSmt->t_lastwork; //время последней коммуникации контроллера c OT
#else
    tt32 = (unsigned int)pSmt->t_lastwork;
#endif

	memcpy((void *)&out->Buf[4], (void *)&tt32, 4);

	memcpy((void *)&out->Buf[8], (void *)&pSmt->BoilerStatus, 4);
	memcpy((void *)&out->Buf[12], (void *)&pSmt->BoilerT, 4);
	memcpy((void *)&out->Buf[16], (void *)&pSmt->RetT,	4);
	memcpy((void *)&out->Buf[20], (void *)&pSmt->Tset,	4);
	memcpy((void *)&out->Buf[24], (void *)&pSmt->Tset_r,4);
	memcpy((void *)&out->Buf[28], (void *)&pSmt->dhw_t, 4);
	memcpy((void *)&out->Buf[32], (void *)&pSmt->TdhwSet, 4);
	memcpy((void *)&out->Buf[36], (void *)&pSmt->FlameModulation, 4);
	memcpy((void *)&out->Buf[40], (void *)&pSmt->Pressure,	4);
	memcpy((void *)&out->Buf[44], (void *)&pSmt->stsDS,	4);
	memcpy((void *)&out->Buf[48], (void *)&pSmt->t1,	4);
	memcpy((void *)&out->Buf[52], (void *)&pSmt->t2,	4);
	memcpy((void *)&out->Buf[56], (void *)&pSmt->Toutside,	4);
	memcpy((void *)&out->Buf[60], (void *)&pSmt->tempindoor, 4);
	memcpy((void *)&out->Buf[64], (void *)&pSmt->tempoutdoor,4);
	memcpy((void *)&out->Buf[68], (void *)&pSmt->TroomTarget,4);
#ifdef _WIN32
    tt32 = (__time32_t)pSmt->tlastc; //время последней коммуникации с контроллером
#else

    tt32 = (unsigned int) pSmt->tlastc;
#endif
	memcpy((void *)&out->Buf[72], (void *)&tt32,	4);
#ifdef _WIN32
    tt32 = (__time32_t) pSmt->parent->t_start;  //время запуска сервера
#else
    tt32 = (unsigned int) pSmt->parent->t_start;
#endif
	memcpy((void *)&out->Buf[76], (void *)&tt32,	4);

/* два раз проверяем, установилась ли уставка, потом сбрасываем признак наличия команды. Чтобы из веб интрефейса и HA можно было задать новую уставку */
	if(pSmt->isCmd == 2)
	{	if(pSmt->TroomTarget != pSmt->TroomTarget_toSet || pSmt->TdhwSet != pSmt->TdhwSet_toSet) 
			pSmt->isCmd  = 1;
		else 		
			pSmt->isCmd  = 3;
	} else 	if(pSmt->isCmd == 3) {
		if(pSmt->TroomTarget != pSmt->TroomTarget_toSet || pSmt->TdhwSet != pSmt->TdhwSet_toSet) 
			pSmt->isCmd  = 1;
		else 		
			pSmt->isCmd  = 0;
	}
//todo
//	B_flags_toSet  
//	Tset_toSet
//	TroomTarget_toSet
//	TdhwSet_toSet

	//80+6
//printf("Send t1 %f t2 %f\n", pSmt->t1, pSmt->t2);
	
	return 0;
}

//ACMD_SET_STATE_S
int SmartApp::callback_set_state(struct Msg1 *in, int inb, struct Msg1 *out, int &outb)
{
	if(pSmt == NULL)
		return -1;
	outb = 6;	
	memcpy((void *)out,(void *)in,6); // out->cmd = ACMD_SET_STATE_S
printf("** ACMD_SET_STATE_S\n");

	memcpy((void *)&pSmt->B_flags_toSet,(void *)&in->Buf[4], 2);
printf("pSmt->B_flags_toSet = %d\n", ((unsigned int)pSmt->B_flags_toSet));
	memcpy((void *)&pSmt->Tset_toSet,(void *)&in->Buf[6], 4);
printf("pSmt->Tset_toSet = %f\n", pSmt->Tset_toSet);
	memcpy((void *)&pSmt->TroomTarget_toSet,(void *)&in->Buf[10], 4);
printf("pSmt->TroomTarget_toSet = %f\n", pSmt->TroomTarget_toSet);
	memcpy((void *)&pSmt->TdhwSet_toSet,(void *)&in->Buf[14], 4);
printf("pSmt->TdhwSet_toSet = %f\n", pSmt->TdhwSet_toSet);
	pSmt->isCmd = 1;

	return 0;
}

void SmartApp::OnIdentify(void)
{
	if(pSmt == NULL)
			return;
//	pSmt->TCPserver_repot_period = parent->controller_user_online_report_period;
	pSmt->SetReportPeriod(parent->controller_user_online_report_period,0);

//   Log(1, "set TCPserver_repot_period =%d\n", pSmt->TCPserver_repot_period);
	
}

void SmartApp::OnClose(void)
{
	if(pSmt == NULL)
			return;
//todo задержка 
//	pSmt->TCPserver_repot_period = parent->controller_report_period;
	pSmt->SetReportPeriod(parent->controller_report_period,20);
//   Log(1, "set TCPserver_repot_period =%d\n", pSmt->TCPserver_repot_period);

}
//int SmartClient::callback_get_sts(struct Msg1 *in, int inb, struct Msg1 *out, int &outb)
void SmartDevice_stub::SetReportPeriod(int rp_toset, int wait_sec)
{	if(wait_sec > 0)
	{	flag_need_to_set_rp = 1;
		TCPserver_repot_period_toset = rp_toset;
		t_need_toset_rp  = time(NULL) + wait_sec;
	} else {
		TCPserver_repot_period = rp_toset;
		flag_need_to_set_rp = 0;
	}
}

int SmartDevice_stub::GetReportPeriod(void)
{	double dt;
	if(flag_need_to_set_rp)
	{	dt = difftime(time(NULL), t_need_toset_rp);
		if(dt >= 0)
		{	TCPserver_repot_period = TCPserver_repot_period_toset;
			flag_need_to_set_rp = 0;
		}
	}
	return TCPserver_repot_period;
}

int SmartDevice_stub::Log(int level, const char *_format, ...)
{
	static int t0 = 0;
	clock_t  t, dt;
	if(level > DEBUG_LEVEL)
		return 1;
	t = GetClock();
	dt = t-t0;
	if(t0 == 0 || dt > 3600*1000)
			t0 = t;
	printf("D[%d][%ld][%x]",level, dt,ClientId);
	va_list args;
    va_start(args, _format);
    vprintf(_format, args);
    va_end(args);
	return 0;

}
