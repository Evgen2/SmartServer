/* Smaert_OT.cpp */

#include "stdafx.h"
#include "SmartServer.hpp"
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#include "OpenTherm.h"


bool OpenTherm::parity(unsigned long frame) //odd parity
{	byte p = 0;
	while (frame > 0)
	{
		if (frame & 1) p++;
		frame = frame >> 1;
	}
	return (p & 1);
}

OpenThermMessageType OpenTherm::getMessageType(unsigned long message)
{	OpenThermMessageType msg_type = static_cast<OpenThermMessageType>((message >> 28) & 7);
	return msg_type;
}

OpenTherm ot;

/************************************/
int Smart_OTlog::InitLog(int nl)
{
	if(indLog == -1) indLog = 0;
	else 
	{	indLog++;
		if(indLog >= OTlog_NMAX)
			indLog = 0;
	}

	lOTlog = nl;
	indOTlog = 0;
	tOTlog = 0;
	nbuf = 0;
	lbuf = OTlogMAX;
	if(pbuf == NULL)
		pbuf = (unsigned char *)calloc(OTlogMAX,OTlog_EL);

	t_start = time(NULL);
	return 0;
}

//в логе nl элементов. 
// Принимаем решение - сколько запросить
int Smart_OTlog::cmdLog(int nl)
{	int rc = 0;
//printf("cmdLog %d\n", nl);
	if(nl > 0)
	{	lOTlog = nl;
		rc = nl;
		if(rc > 15) rc = 15; //todo
		if(nbuf + rc >= OTlogMAX)
			rc = OTlogMAX - nbuf;
	} else if(nl == 0) {
		rc = 0;
	}
	return rc;
}

int Smart_OTlog::FinishLog(void)
{
//	printf("FinishLog\n");
	if(pbuf)
		free(pbuf);
	pbuf = NULL;
    lOTlog = 0;
	return 0;
}

int Smart_OTlog::Add_OT_log(void *_p, int n)
{	unsigned char *pb;
	int rc = 0;
	if(pbuf)
	{	pb = pbuf + nbuf * OTlog_EL;
		if(nbuf + n >= OTlogMAX)
		{	n = OTlogMAX-nbuf;
			rc = 1;
		}
		memcpy(pb, _p, n * OTlog_EL);
		nbuf += n;
	}
	return 0;
}

void CreatePath_log(char *pathname, char *dirname, char *filename);

int Smart_OTlog::writeLog(char *dirname, char *_info)
{	char filename[40], pathname[80];
	char str[512], str0[312], str1[128];
	FILE *fp;
    struct tm *newtime;
    uint16_t u88;
	unsigned char  lb, hb, sts;
	char sb;
	int i, id_OT_ids, idd;
//	OpenThermMessageID idd;
	unsigned int *p, tmp, tms0,tms00, tms, tmsold, t8, count=0, ic, ic8, icold, ind;
	unsigned long response;
    int parity, messagetype;
	float ft;

	sprintf(filename,"ot_%d",indLog);

	CreatePath_log(pathname, dirname, filename);

//	printf("write OT_log %d to %s\n",  nbuf, pathname);

	fp = fopen(pathname, "w");
	if(fp == NULL)
	{	printf("can't write to  file %s\n", pathname);
		return 1;
	}

    newtime = localtime(&t_start);
    sprintf( str,"%02d.%02d.%d %d:%02d:%02d",
          newtime->tm_mday,newtime->tm_mon+1,newtime->tm_year+1900,
		  newtime->tm_hour, newtime->tm_min, newtime->tm_sec);

	fprintf(fp,";ot Log created: %s\n", str);
	if(_info)
	fprintf(fp,";%s\n", _info);
		
	p = (unsigned int *)pbuf;
	ind = icold = 0;
	ic8 = 0;
	tms0  = tmsold = t8 = 0;

	for(i=0; i<nbuf;i++)
	{
		tmp = *((unsigned int *)p);
		ic = (tmp>>24);
		sts = ic>>6;
		ic = ic & 0x3f;
		if(ic < icold)
		{ ic8++;
		}
		ind = ic8*0x40 + ic;

		tms = tmp & 0xffffff;
		if(tms < tmsold)
		{ t8++;
		}
		tmsold = tms;
		tms0 = t8*0x1000000 + tms;
		if(i==0)
			tms00 = tms0;
		if((tms0 - tms00) > 10000000)
				fprintf(fp,"==>\n");
/*****/
	 icold = ic;

		p++;
		response = *p;
	    parity = ot.parity(response);
	    messagetype = ot.getMessageType(response);
		idd = (response >> 16 & 0xFF);
	    u88 = (response & 0xffff);
	    ft = (u88 & 0x8000) ? -(0x10000L - u88) / 256.0f : u88 / 256.0f;

	    sprintf(str, "%d %3d %2d %s %d %04x ", ind, tms0 - tms00, idd, OpenThermMessageTypeTxt[messagetype], sts, u88) ;
//	  	sprintf(str, "[%d %d] %d %3d %2d %s %d %04x ", ic, tms, ind, tms0 - tms00, idd, OpenThermMessageTypeTxt[messagetype], sts, u88) ;

		if(parity) 
			strcat(str, "Parity ");
		tms00 = tms0;

		id_OT_ids = ot.id_to_index[idd];
		if(idd == Status)
		{	ot.verboseStatus(str0, u88);
			strcat(str, str0);
		} else if((idd == SConfigSMemberIDcode) && (messagetype == READ_ACK)) {
			ot.verboseSConfigSMemberIDcode(str0, u88);
			memberCode = (u88 & 0x0ff);
			strcat(str, str0);
		} else if(id_OT_ids >= 0)	{  
		   hb = (u88>>8) & 0x0ff;
		   lb = u88 & 0x0ff;
		   str0[0] = str1[0] = 0;

				if(OT_ids[id_OT_ids].ptype1 == F88)
				{	sprintf(str0,"%g", ft);
				} else if(OT_ids[id_OT_ids].ptype1 == FLAG8) {
					sprintf(str0,"%04x", hb);
				} else if(OT_ids[id_OT_ids].ptype1 == OTU8) {
					sprintf(str0,"%d",hb);
				} else if(OT_ids[id_OT_ids].ptype1 == OTS8) {
					sb = (char)hb;
					sprintf(str0,"%d", sb);
				} else if(OT_ids[id_OT_ids].ptype1 == OTU16) {
					sprintf(str0,"%d", u88);
				} else if(OT_ids[id_OT_ids].ptype1 == OTS16) {
					sprintf(str0,"%d", u88);
				} else if(OT_ids[id_OT_ids].ptype1 == OTSP) {
					sprintf(str0,"%04x", u88);
				}

				if(OT_ids[id_OT_ids].ptype2 == FLAG8) {
					sprintf(str1,"  %04x", lb);
				} else if(OT_ids[id_OT_ids].ptype2 == OTU8) {
					sprintf(str1," %d", lb);
				} else if(OT_ids[id_OT_ids].ptype2 == OTS8) {
					sb = (char)lb;
					sprintf(str1," %d", sb);
				}
				strcat(str,  OT_ids[id_OT_ids].name);
				strcat(str, "\t");
				strcat(str, str0);
				strcat(str, str1);
		} else {
			strcat(str, "id unknown");
		}


//		printf("%s\n", str);

		fprintf(fp,"%s\n", str);
		p++;

	}

	fclose(fp);
	return 0;
}

#ifdef _WIN32
#include <direct.h>
#else 
#include <sys/stat.h> // stat
#endif
#include <stdlib.h>

void CreatePath_log(char *pathname, char *dirname, char *filename )
{	int rc;
#ifdef _WIN32
//   _splitpath(det->fname,drive,dir,fname,ext);
   _makepath(pathname,NULL,dirname,filename,".log");

   printf("pathname %s\n",  pathname);
//	printf("check existance dir %s\n",  dirname);
//	printf("Widows:\n");
	rc = _mkdir(dirname);
	if(rc == EEXIST)
		printf("dir exist\n");

#else // WIN32
//	printf("Linux:\n");

	strcpy(pathname,dirname);
	strcat(pathname,"/");
	strcat(pathname,filename);
	strcat(pathname,".log");

struct stat st = {0};

if (stat(dirname, &st) == -1) {
    mkdir(dirname, 0700);
//	printf("mkdir %s\n", dirname);
} else {
//	printf("Dir %s stat -1\n", dirname);
}

//	printf("file %s\n", pathname);

#endif // WIN32
}

//OpenThermVendor OTvendorList[] =
char * GetOTVendorName(int id)
{	int i, n;
	n = sizeof(OTvendorList)/sizeof(OpenThermVendor);
	for(i=0;i<n;i++)
	{	if(id == OTvendorList[i].id)
		{	return (char *) OTvendorList[i].name;
		}
	}
	return NULL;
}

void OpenTherm::verboseStatus(char *str, uint16_t u88)
{	sprintf(str, "Status:%04x ", u88);
	strcat(str,"Slave:");
	if(u88 & 0x01)
			strcat(str," Fault");
	if(u88 & 0x02)
			strcat(str," CH");
	if(u88 & 0x04)
			strcat(str," DHW");
	if(u88 & 0x08)
			strcat(str," Flame");
	if(u88 & 0x10)
			strcat(str," Cool");
	if(u88 & 0x20)
			strcat(str," CH2");
	if(u88 & 0x40)
			strcat(str," Diag");
	if(u88 & 0x80)
			strcat(str," Elprod");
	strcat(str," Master:");
	if(u88 & 0x100)
			strcat(str," CH_en");
	if(u88 & 0x200)
			strcat(str," DHW_en");
	if(u88 & 0x400)
			strcat(str," Cool_en");
	if(u88 & 0x800)
			strcat(str," OTC_act");
	if(u88 & 0x1000)
			strcat(str," CH2_en");
	if(u88 & 0x2000)
			strcat(str," Summer");
	if(u88 & 0x4000)
			strcat(str," DHW_block");
	if(u88 & 0x8000)
			strcat(str," 8000");
}

// OpenThermMessageID::SConfigSMemberIDcode:  //3
/*
ID3:HB0	Slave configuration: DHW present
ID3:HB1	Slave configuration: Control type
ID3:HB2	Slave configuration: Cooling configuration
ID3:HB3	Slave configuration: DHW configuration
ID3:HB4	Slave configuration: Master low-off&pump control
ID3:HB5	Slave configuration: CH2 present
ID3:HB6	Slave configuration: Remote water filling function
ID3:HB7	Heat/cool mode control
ID3:LB Slave MemberID Code
*/
void OpenTherm::verboseSConfigSMemberIDcode(char *str, uint16_t u88)
{	unsigned char lb;
	char *pstr, str0[80];
	lb = u88 & 0x0ff;

	sprintf(str, "SConfMemberID:%04x ", u88);
	if(u88 & 0x100)
			strcat(str," DHW present");
	if(u88 & 0x200)
			strcat(str," Ctrl: on/off");
//	else
//			strcat(str," Ctrl: modulation");

	if(u88 & 0x400)
			strcat(str," Cooling supported");

	if(u88 & 0x800)
			strcat(str," DHW: storage tank");
//	else
//			strcat(str," DHW configuration: instantaneous or not-specified");

	if(u88 & 0x1000)
			strcat(str," Master low-off&pump control: not allowed");
//	else
//			strcat(str," Master low-off&pump control: allowed");

	if(u88 & 0x2000)
			strcat(str," CH2 present");

//Unknown for applications with protocol version 2.2 or older. 
	if(u88 & 0x4000)
			strcat(str," Remote water filling function: not available");
//	else
//			strcat(str," Remote water filling function: available or unknown");

	if(u88 & 0x8000)
			strcat(str," Heat/cool mode control: is done by slave");
//	else
//			strcat(str," Heat/cool mode control: Heat/cool mode switching can be done by master");
	pstr = GetOTVendorName(lb);
	if(pstr)
	{	sprintf(str0,"MemberID %d %s", lb,pstr);
	} else {
		if(lb == 0)
			sprintf(str0,"MemberID 0 (customer)");
		else
			sprintf(str0,"MemberID %d (?)", lb);
	}
	strcat(str," ");
	strcat(str,str0);
}


