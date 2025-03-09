/* SmartClient.cpp */
#include "stdafx.h"
#include "SmartServer.hpp"

int SmartClient::loop(void)
{	int rc = 0, rc0, rc1, lsend=0, needclose = 0;
	double dt;
	rc0 = TcpRead();
	if(rc0 > 0)
	{	sts_cl = 3;
//printf("TcpRead %d\n", rc0);
		rc1 = ParseFromTCP( rc0,  lsend, needclose);
		if(rc1 == 0 && lsend > 0)
		{	rc0 = tcpClient.Send(WriteBuff, lsend);
			if(rc0 > 0)
			{	Log(10,"Send %d bytes\n", rc0);
				if(needclose)
					Sleep(10);
			}
			else 
				rc = 2;
		}
		if(needclose)
		{	rc = 3;
			Log(9,"Close client on needclose, dt %.f sec\n");
		}
	} else if(rc0 == 0) {
		dt = difftime(time(NULL), tlastc);
		if(dt < 3)
		{	Sleep(20);
		} else {
			Log(9,"Close client on timeout, dt %.f sec\n", dt);
			rc = -10;
		}
	} else {
		Log(9, "Close client on TcpRead2 rc %d\n", rc0);
		rc = 1;
	}
//	if(rc != 0)
//		printf("loop rc =%d\n", rc);
	if(rc == 0)
		Sleep(20);
	else
		OnClose();
	return rc;
}


void SmartClient::Init(void)
{	sts_cl = -1;
	verbose = 0;
	tlastc = 0;
	ipaddr[0] = 0;
	smartd = NULL;
	devsts = 0;
}

//rc >= 0 - number of bytes read from socket
//rc <  0 - error 
int SmartClient::TcpRead()
{	int rc, l;
	l = 512; //todo 
	rc = tcpClient.Read(ReadBuff, l, tcpClient.timeout); /* wait for client */
	if(rc > 0)
			tlastc = time(NULL);
	return rc;
}

int SmartClient::ParseFromTCP(int nb, int &Lsend, int &needclose)
{   struct  Msg1 *pucmd;
	struct Msg1  MsgOut;
    int cmd, rc;
	if(nb > sizeof(struct  Msg1))
		return -1;
	if(nb <= 0)
		return -2;
    pucmd = (struct  Msg1 *)ReadBuff;
	if(pucmd->cmd0 != 0x22 && pucmd->cmd0 != 0x12)
		return 1;
	cmd = pucmd->cmd;
	MsgOut.cmd0  =  pucmd->cmd0;
	MsgOut.ind  =  pucmd->ind;
	Lsend = 0;

	Log(10, "ParseFromTCP cmd %i (%x) %d bytes\n", cmd, cmd, nb);

	if(smartd!= NULL)
		smartd->tlastc = time(NULL);

	switch(cmd)
	{
		case MCMD_HAND_SHAKE:
			rc = callback_HandShake(pucmd, nb, &MsgOut, Lsend);
			if(rc == 0)
			{	memcpy(WriteBuff, &MsgOut, Lsend);
				needclose = 0;
			} else {
				needclose = 1;
			}
			devsts = 0;
			break;

		case MCMD_IDENTIFY:
			rc = callback_Identify(pucmd, nb, &MsgOut, Lsend);
			if(rc == 0)
			{	memcpy(WriteBuff, &MsgOut, Lsend);
				needclose = 0;
			} else {
				needclose = 1;
			}
			devsts = 2;
			break;

		case MCMD_INTRODUCESELF:
			rc = callback_Introduce(pucmd, nb, &MsgOut, Lsend);
			if(rc == 1)
			{	devsts = 1;
				SmartTerm *st;
				st = (SmartTerm *)smartd; 
				if(st == NULL)
					break;
				OnIdentify();
				needclose = 0;
				st->sts = STS_INIT;
			} else if(rc == 2)
			{	devsts = 2;
				SmartApp *sta;
				sta = (SmartApp *)smartd; 
				if(sta == NULL)
					break;
				OnIdentify();			
				sta->sts = STS_INIT;
				needclose = 0;
			} else {
				needclose = 1;
			}

			break;

		case SCMD_GET_STS:
		{	SmartTerm *st;
			st = (SmartTerm *)smartd; 
			if(st == NULL)
			{	ask_HandShake( &MsgOut, Lsend);
				needclose = 0;
				break;
			}
	   Log(10, "ParseFromTCP SCMD_GET_STS\n");

			rc = st->callback_Get_Sts(pucmd, nb, &MsgOut, Lsend);
//			rc = st->callback_Get_Sts(&MsgOut, Lsend);
	   Log(9, "ParseFromTCP SCMD_GET_STS rc =%d nb = %d Lsend=%d\n", rc, nb, Lsend);
			if(rc)
			{	st->sts = STS_ERROR; //todo
			} else {
				st->sts = STS_REPORT;
				st->tlastc = time(NULL);
			}
			needclose = 1;
		}
			break;

		case  CCMD_SEND_STS_S:
		{	SmartTerm *st;
			st = (SmartTerm *)smartd; 
			if(st == NULL)
			{	if(callback_ClientID_from_Get_Sts(pucmd, nb))
				{	if(smartd->type == 1)
					{	devsts = 1;
						st = (SmartTerm *)smartd; 
					} else {
					   Log(2, "Error : smartd->type %d != 1\n", smartd->type);
					   break;
					}

					OnIdentify();
				} else {
					ask_HandShake( &MsgOut, Lsend);
					break;
				}
			}
			if(st != NULL) 
				st->tlastc = time(NULL);
	   Log(10, "ParseFromTCP CCMD_SEND_STS \n");

			rc = st->callback_Send_Sts(pucmd, nb, &MsgOut, Lsend);
	   Log(10, "ParseFromTCP CCMD_SEND_STS rc =%d nb = %d Lsend=%d\n", rc, nb, Lsend);
			if(rc)
			{	st->sts = STS_REPORT_ANSWER; //todo
			needclose = 0;
			} else {
				st->sts = STS_REPORT_ANSWER;
			needclose = 1;
			}
		}
			break;

		case  CCMD_SEND_OTLOG_S:
		{	SmartTerm *st;
			st = (SmartTerm *)smartd; 
			if(st == NULL)
			{	if(callback_ClientID_from_Get_Sts(pucmd, nb))
				{	if(smartd->type == 1)
					{	devsts = 1;
						st = (SmartTerm *)smartd; 
					} else {
					   Log(2, "Error : smartd->type %d != 1\n", smartd->type);
					   break;
					}

					OnIdentify();
				} else {
					ask_HandShake( &MsgOut, Lsend);
					break;
				}
			}
			if(st != NULL) 
				st->tlastc = time(NULL);
	   Log(10, "ParseFromTCP CCMD_SEND_OTLOG_S\n");

			rc = st->callback_Send_OTlog(pucmd, nb, &MsgOut, Lsend);
	   Log(10, "ParseFromTCP CCMD_SEND_OTLOG_S rc =%d nb = %d Lsend=%d\n", rc, nb, Lsend);
			if(rc)
			{	st->sts = STS_REPORT_ANSWER; //todo
				needclose = 0;
			} else {
				st->sts = STS_REPORT_ANSWER;
				needclose = 1;
			}
		}
			break;

		case MCMD_GET_OT_INFO:
		{	SmartTerm *st;
			st = (SmartTerm *)smartd; 
			if(st == NULL)
				break;
			if(st != NULL) 
				st->tlastc = time(NULL);
	
			rc = st->callback_Get_OT_Info(pucmd, nb, &MsgOut, Lsend);
			if(rc)
			{	st->sts = STS_ERROR; //todo
			}
		}
			break;

		case ACMD_ASK_STS_S:
		{	
			rc = callback_ask_sts(pucmd, nb, &MsgOut, Lsend);
			if(rc == 0)
			{	devsts = 2;
			}
		}
			break;

		case ACMD_GET_STS_S:
		{	SmartApp *sa;
			sa = (SmartApp*)smartd; 
			if(sa == NULL)  //todo use callback_ClientID_from_Get_Sts & get devsts from ClientID
			{	if(callback_ClientID_from_Get_Sts(pucmd, nb))
				{	if(smartd->type == 2)
					{	devsts = 2;
						sa = (SmartApp*)smartd; 
					} else {
					   Log(2, "Error : smartd->type %d != 2\n", smartd->type);
					   break;
					}
					OnIdentify();
				} else {
					ask_HandShake( &MsgOut, Lsend);
					needclose = 0;
					break;
				}
			}

			sa->tlastc = time(NULL);
			rc = sa->callback_get_sts(pucmd, nb, &MsgOut, Lsend);
			needclose = 1;

		}
			break;

		case ACMD_SET_STATE_S:
		{	SmartApp *sa;
			sa = (SmartApp*)smartd; 
			if(sa == NULL)  //todo use callback_ClientID_from_Get_Sts & get devsts from ClientID
			{	if(callback_ClientID_from_Get_Sts(pucmd, nb))
				{	if(smartd->type == 2)
					{	devsts = 2;
						sa = (SmartApp*)smartd; 
					} else {
					   Log(2, "Error : smartd->type %d != 2\n", smartd->type);
					   break;
					}
					OnIdentify();
				} else {
					ask_HandShake( &MsgOut, Lsend);
					needclose = 0;
					break;
				}
			}

			sa->tlastc = time(NULL);
			rc = sa->callback_set_state(pucmd, nb, &MsgOut, Lsend);
			needclose = 1;
		}
			break;


		default:
 		Log(1,"Unknown cmd %i (%x) in %s\n", cmd, cmd, __FUNCTION__);
		return 2;
	}

	
/*
	if(devsts == 1)
	{	SmartTerm *st;
		st = (SmartTerm *)smartd; 
		rc = st->loop1( &MsgOut, Lsend);
		if(Lsend > 0)
		{	memcpy(WriteBuff, &MsgOut, Lsend);
		}
	   Log("ParseFromTCP return Lsend=%d\n",  Lsend);
	} else if(devsts == 2) {
	 	SmartApp *sta;
		sta = (SmartApp *)smartd; 
		rc = sta->loop1( &MsgOut, Lsend);
		if(Lsend > 0)
		{	memcpy(WriteBuff, &MsgOut, Lsend);
		}
	   Log("ParseFromTCP return Lsend=%d\n");

	}  else if(Lsend > 0) {
		memcpy(WriteBuff, &MsgOut, Lsend);
	   Log("ParseFromTCP return Lsend=%d\n", Lsend);
	}
*/
	if(Lsend > 0)
		memcpy(WriteBuff, &MsgOut, Lsend);
	return 0;
}

//MCMD_HAND_SHAKE
int  SmartClient::callback_HandShake(struct Msg1 *in, int inb, struct Msg1 *out, int &outb) 
{	int l, l0, rc = 0;
	l = strnlen((char *)in->Buf, inb-6);
	l0 = strlen(HAND_SHAKE_INP);
	
	if(l >= l0)
	{	if(!strncmp((char *)in->Buf, HAND_SHAKE_INP, l0))
		{
	Log(10, "Get HandShake %s\n", HAND_SHAKE_INP);
			out->cmd = MCMD_HAND_SHAKE;
			out->cmd0 = 0xfe;
			l0 = strlen(HAND_SHAKE_OUT);
			l0++; //плюс конечный ноль
			out->ind = in->ind;
			memcpy(out->Buf, HAND_SHAKE_OUT,l0);
			outb =  6 + l0;
		} else {
			rc = 1;
		}
	}
	return rc;
}
//SCMD_GET_HAND_SHAKE
void SmartClient::ask_HandShake(struct Msg1 *out, int &outb)
{
	out->cmd = SCMD_GET_HAND_SHAKE;
	outb = 6;
}

//MCMD_IDENTIFY
int SmartClient::callback_Identify(struct Msg1 *in, int inb, struct Msg1 *out, int &outb)
{	int i,l,lp; 
	unsigned char mac[6];
	for(i=0; i<6; i++) mac[i] = 0;
	l = sizeof(IDENTIFY_TEXT);
	Log(10, "Get MCMD_IDENTIFY\n");
	memcpy((void *)out,(void *)in,6);
	lp =sizeof(int)*6 + 6 + 12 +l;
	outb = 6 + sizeof(short int) + lp;	
	*((short int *) (&out->Buf[0])) = (short int) lp;
	*((int *) (&out->Buf[2])) = IDENTIFY_TYPE; //=DS_SMARTSERVER
	*((int *) (&out->Buf[6])) =  IDENTIFY_CODE;
	*((int *) (&out->Buf[10])) =  parent->IdNumber;	
	*((int *) (&out->Buf[14])) =  parent->Version;	
	*((int *) (&out->Buf[18])) =  parent->SubVersion;	
	*((int *) (&out->Buf[22])) =  parent->SubVersion1;	
 	memcpy((void *)&out->Buf[26],(void *)parent->BuildDate,12);
 	memcpy((void *)&out->Buf[38],(void *)&mac,6);
 	memcpy((void *)&out->Buf[44],(void *)&IDENTIFY_TEXT,l);

	return 0;
}


extern int FindSmartTherm(unsigned char mac[6]);
extern int FindSmartThermId(int _id);
extern int FindSmartAppId(int _id);

extern SmartDevice_stub *AddSmartTherm(unsigned char mac[6], char *_ipaddr, int st_type);
extern SmartDevice_stub *GetSmartDev(int _ind, int idclient, char *_ipaddr, int mode);

extern int FindSmartApp(unsigned char mac[6]);
extern SmartDevice_stub *GetSmartAdd(int ind);
extern SmartDevice_stub *AddSmartApp(unsigned char mac[6], char *_ipaddr,  int ind);


//MCMD_INTRODUCESELF
//rc < 0 error
//     0 - dev exist
//     1 - new dev
//     2 - new app
int  SmartClient::callback_Introduce(struct Msg1 *in, int inb, struct Msg1 *out, int &outb) 
{	int l, rc = -1,  ind, id, oldv=0, ischange;
	int idcode, idNumber, vers, subvers, subvers1;
	unsigned short int idtype, idsubtype;
	short int ld;
	char _biosDate[12];     /* дата компил€ции биоса */
	unsigned char mac[6];    
	char str[256];    

	if(inb < 42)
		return -3;
	memcpy((void *)out,(void *)in,6);
	outb = 6 + 4*3;

	Log(10, "Get MCMD_INTRODUCESELF\n");
    ld = *((short int *)&in->Buf[0]);

	idtype    = *((unsigned short int *)&in->Buf[2]);
	idsubtype = *((unsigned short int *)&in->Buf[4]);

	idcode = *((int *)&in->Buf[6]);
	idNumber = *((int *)&in->Buf[10]);
	vers     = *((int *)&in->Buf[14]);
	subvers  = *((int *)&in->Buf[18]);
	subvers1 = *((int *)&in->Buf[22]);
    memcpy(_biosDate,((void *)&in->Buf[26]),12);
    memcpy(mac,((void *)&in->Buf[38]),6);
//printf("MAC %02x:%02x:%02x:%02x:%02x:%02x\n",mac[0], mac[1], mac[2], mac[3], mac[4],mac[5] );
	l = ld - (24+12+6);
	if(in->Buf[44 + l-1] > 32)
	{	oldv = 1;
	} else {
		oldv = 0;
		l = ld - (24+12+6+3);
	}
	
	if(l > 0)
	{
		memcpy(str,((void *)&in->Buf[44]),l);
		str[l]=0;
#if 0
#ifdef _WIN32
		{
	    wchar_t IdName[128]; //Ќазвание устройства
		rc0 = MultiByteToWideChar(CP_UTF8, 0,str,-1, IdName, 0);
		rc0 = MultiByteToWideChar(CP_UTF8, 0,str,-1, IdName, 128);
		
		   printf("IdTypt=%x IdCode=%x IdNum=%i\n", idtype, idcode, idNumber);
    _setmode(_fileno(stdout), _O_U8TEXT);  
		   wprintf(L"IdName=%s \n", IdName);
    _setmode(_fileno(stdout), _O_TEXT);  //	   printf("Ќормальный режим консоли\n");

		   printf("vers %d.%d.%d %s\n", vers, subvers, subvers1, _biosDate);
		}
#else
		   printf("IdTypt=%x IdCode=%x IdNum=%i\nIdName=%s \n",idtype, idcode, idNumber, str); //todo
#endif
#endif
		if(idtype == DS_OPENTHERM && idcode& (2<<24) )
		{	ind = FindSmartTherm(mac);
			if(ind < 0)
			{ // printf("Ќовый контроллер\n"); 
			   smartd = AddSmartTherm(mac, ipaddr, idsubtype);
			   if(smartd == NULL)
			   { 	printf("ѕредел по числу контроллеров \n");
					rc = -1;
			   } else {
				   smartd->Vers = vers;
				   smartd->SubVers = subvers;
				   smartd->SubVers1= subvers1;
				   memcpy(smartd->BiosDate, _biosDate, 12);
				   rc = 1;
				}
			} else {
				ischange = 0;
//			  printf("контроллер ind %d\n", ind);
				smartd = GetSmartDev(ind, idclient, ipaddr,1);
				if(vers != smartd->Vers) ischange++;
				smartd->Vers = vers;
				if(subvers != smartd->SubVers) ischange++;
				smartd->SubVers = subvers;
				if(subvers1 != smartd->SubVers1) ischange++;
				smartd->SubVers1 = subvers1;
				memcpy(smartd->BiosDate, _biosDate, 12);
				if(idsubtype != ((SmartTerm *)smartd)->st_type)
				{	((SmartTerm *)smartd)->st_type = idsubtype;
					ischange++;
				}
				if(ischange)
					parent->need_save_smt = 1;
			  rc = 1;
			}
			if(oldv == 0)
			{
				((SmartTerm *)smartd)->StartSts = in->Buf[44+l];
				((SmartTerm *)smartd)->reset_reason[0] = in->Buf[45+l];
				((SmartTerm *)smartd)->reset_reason[1] = in->Buf[46+l];
				if(((SmartTerm *)smartd)->StartSts == 1) 
				{	if(((SmartTerm *)smartd)->U0 > 0.)
							((SmartTerm *)smartd)->isCmd = 0x10;
					else 
						((SmartTerm *)smartd)->StartSts = 0;
		//					printf("-----==> StartSts=1\n");
				}
			}
			memcpy(&out->Buf[0], &smartd->ClientId,4);
			memcpy(&out->Buf[4], &smartd->ClientId_k,4);
			{	int tmp;
			//		tmp = ((SmartTerm *)smartd)->TCPserver_repot_period; 
				tmp = ((SmartTerm *)smartd)->GetReportPeriod(); 
			if(((SmartTerm *)smartd)->StartSts == 1)
				tmp = 0;

		   Log(10, "Use 10 TCPserver_repot_period =%d\n", tmp);

			    memcpy(&out->Buf[8], &tmp,4);
			}
			//TCPserver_repot_period
		} else if(idtype == DS_USER_APP && idcode == 0x200) {
			ind = FindSmartTherm(mac); // провер€ем наличие такого контроллера
			if(ind < 0)
			{  printf("нет такого контроллера\n");
				rc = -1;
			} else {
				id = FindSmartApp(mac);
				if(id < 0)
				{//	printf("Ќовое приложение\n"); 
					smartd = AddSmartApp(mac, ipaddr, ind);
					if(smartd == NULL)
					{ 	printf("ѕредел по числу приложений\n");
							rc = -1;
					} else {
					   smartd->Vers = vers;
					   smartd->SubVers = subvers;
					   smartd->SubVers1= subvers1;
					   memcpy(smartd->BiosDate, _biosDate, 12);
						rc = 2;
					}
				} else {
					smartd = GetSmartAdd(id);
						rc = 2;
				}

				memcpy(&out->Buf[0], &smartd->ClientId,4);
				memcpy(&out->Buf[4], &smartd->ClientId_k,4);
				memcpy(&out->Buf[8], &parent->application_report_period,4);
			}

		} else {
		   printf("idtype=%x not applicable\n",idtype); 
		   rc = -2;
		}
	}

//	 printf("-----==> rc %d\n", rc);

	return rc;
}


//ACMD_ASK_STS_S
//rc < 0 error
//     0 - dev exist
//     1 - new dev
int  SmartClient::callback_ask_sts(struct Msg1 *in, int inb, struct Msg1 *out, int &outb) 
{	int rc, ind, id, flags;
	unsigned char mac[6];    
	if(inb < 12)
		return -3;
	Log(10, "callback_ask_sts\n");
    memcpy(mac,((void *)&in->Buf[0]),6);
//printf("? MAC %02x:%02x:%02x:%02x:%02x:%02x\n",mac[0], mac[1], mac[2], mac[3], mac[4],mac[5] );
    flags = 0;
	ind = FindSmartTherm(mac);
	if(ind < 0)
	{  Log(2, "I don't know controller with");
	   Log(2, " MAC %02x:%02x:%02x:%02x:%02x:%02x\n",mac[0], mac[1], mac[2], mac[3], mac[4],mac[5] );
 	   rc = -1;
	} else {
	   flags = 1;
	    id = FindSmartApp(mac);
		if(id < 0)
		{//	printf("Ќовое приложение\n"); 
			smartd = AddSmartApp(mac, ipaddr, ind);
			if(smartd == NULL)
			{ 	printf("ѕредел по числу приложений\n");
					rc = -1;
				flags |= 0x2;  
			}
		} else {
			smartd = GetSmartAdd(id);
		}
	}

	memcpy((void *)out,(void *)in,6);
	out->cmd = ACMD_ASK_STS_S;
	memcpy(&out->Buf[0], &flags,4);
	outb = 6+4;

	return rc;
}

//get ClientID from first 4 bytes for messages 
//CCMD_SEND_STS_S	
//ACMD_GET_STS_S
//rc = 0 = not found
//rc = 1 = I know ID SmartThertm
//rc = 2 = I know ID SmartApp
int SmartClient::callback_ClientID_from_Get_Sts(struct Msg1 *in, int inb)
{	int rc = 0, _id;

	memcpy((void *)&_id,(void *)&in->Buf[0], 4);

	Log(10, "ClientID_from_Get_Sts _id %x\n", _id);

//_id должно совпадать с smartd->ClientId
	smartd = GetSmartDev(_id, idclient, ipaddr, 0);
	if(smartd)
	{  rc = smartd->type;
		Log(10, "ClientID_from_Get_Sts rc %x ClientId %x\n", rc, smartd->ClientId);
		if(rc == 1)
		{	SmartTerm *st;
			st =  (SmartTerm *)smartd;
			if(st->B_flags&0x800)
					Log(10, "ClientID_from_Get_Sts use PID %x\n", rc, (st->B_flags&0x800));
			else 
					Log(10, "ClientID_from_Get_Sts not use PID %x\n", rc, (st->B_flags&0x800));
		}
	}

	return rc;
}

/* св€зываем A и C, т.е. т.е. передаем классу контроллера знание о наличии своего приложени€  */
void SmartClient::OnIdentify(void)
{ //todo
	if(devsts == 1)
	{	SmartTerm *st;
		st = (SmartTerm *)smartd; 
		st->tlastc = time(NULL);
	} else if(devsts == 2) {
	 	SmartApp *sta;
		sta = (SmartApp *)smartd;
		sta->tlastc = time(NULL);
		sta->OnIdentify();
//	void OnClose(void);

	}
}

void SmartClient::OnClose(void)
{ //todo
	if(devsts == 1)
	{	SmartTerm *st;
		st = (SmartTerm *)smartd; 
	} else if(devsts == 2) {
	 	SmartApp *sta;
		sta = (SmartApp *)smartd;
		sta->OnClose();
	}
}

void SmartDevice_stub::MakeClientId(int ind)
{
//todo
	ClientId = ind;
	if(type == 2)
		ClientId |= 0x80000000;
	ClientId_k = (int) clock(); //todo
}

int SmartClient::Log(int level, const char *_format, ...)
{
	static int t0 = 0;
	clock_t  t, dt;
	if(level > DEBUG_LEVEL)
		return 1;
	t = GetClock();
	dt = t-t0;
	if(t0 == 0 || dt > 3600*1000)
			t0 = t;
	printf("C[%d][%ld][%d][%d]",level, dt, idclient,devsts);
	va_list args;
    va_start(args, _format);
    vprintf(_format, args);
    va_end(args);
	return 0;
}
