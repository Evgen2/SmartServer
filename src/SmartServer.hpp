/* SmartServer.hpp */
#ifndef SMARTSERVER_H
#define SMARTSERVER_H
#include "Smart_ServerConfig.h"
#include "TCPconnection.hpp"
#include "SmartDevice.h"
#include "AutoConfig.h"

class SmartServer;

class SmartClient
{
 public:
	int idclient;
/* -1 - не инициализирован 0 - стартовал,  1 - createTCPserverconnection & listen Ok, 2 accecpt Ok, 0x10 error 0x20 - ended */
/* -1 - не инициализирован/свободен 0 - стартует, 1 нитка запустилась
1 - createTCPserverconnection & listen Ok, 2 accecpt Ok, 0x10 error 0x20 - ended 
*/
	volatile int sts_cl;        
	int verbose;
    TCPconnection tcpClient; 
    TCPconnection tcpserver; /* Old */  //
    char ReadBuff[1500];
    char WriteBuff[1500];
	time_t tlastc; /* время последней коммуникации */
	char ipaddr[32];
	int devsts;
	SmartDevice_stub * smartd;
	SmartServer *parent;

	SmartClient(void)
	{	sts_cl = -1;
		idclient = -1;
		verbose = 0;
		tlastc = 0;
		ipaddr[0] = 0;
		smartd = NULL;
		devsts = 0;
	}
	void Init(void);
    int ParseFromTCP(int nb, int &Lsend, int &needclose);
	int callback_HandShake(struct Msg1 *in, int inb, struct Msg1 *out, int &outb); 
	int callback_Identify(struct Msg1 *in, int inb, struct Msg1 *out, int &outb); 
	int callback_Introduce(struct Msg1 *in, int inb, struct Msg1 *out, int &outb); 
	void ask_HandShake(struct Msg1 *out, int &outb); 
	int callback_ask_sts(struct Msg1 *in, int inb, struct Msg1 *out, int &outb);
	int callback_ClientID_from_Get_Sts(struct Msg1 *in, int inb);
	void OnIdentify(void);
	void OnClose(void);

	int loop(void);
	int TcpRead(void);
	int Log(int level, const char *_format, ...);

    
};

class SmartServer
{
 public:
static char *Appname;
static int Version;
static int SubVersion;
static int SubVersion1;
static const  char *BuildDate;
static int IdNumber;            /* номер устройства */

	int configversion;
	char *CfgName;
	AutoConfig cfg;
	int tcp_port;
	int timeout;                //таймаут на соединение в мс
	int timeoutAnswer;          //таймаут на повторение посылки в мс
	int controller_report_period; //default controller server report period, sec
	int controller_user_online_report_period; //default user online server report period, sec
	int application_report_period; //default application report period, sec
	time_t t_start; // time of server start

    TCPconnection tcpserver;   //

	int verbose;
	int sts;        /* -1 - не инициализирован 0 - стартовал,  >1 - онлайн */
	SmartClient sclient[MAX_CLIENTS];

	int NclientsThreads;   //число клиентских ниток

	
	SmartTerm smt[MAX_DEVICES];
	int Nsmt;
	SmartApp  sma[MAX_APPLICATIONS];
	int Nsma;
	int need_save_smt; 

	SmartServer(void)
	{  int i;
		configversion =-1;
		CfgName = (char *)"SmartServer.cfg";
		tcp_port = 8876;
		timeout = 20000;
		timeoutAnswer = 2000;
		sts = -1;
		verbose = 0;
		for(i=0; i<MAX_CLIENTS; i++) 
		{	sclient[i].idclient = i;
		}
		controller_report_period = 20; //600
		controller_user_online_report_period = 10;
		application_report_period = 4;
		 Nsmt = 0;
		 Nsma = 0;
		 need_save_smt = 0;
	}


	int init(char *ConfigFile);
//	int ReadCFG(void);
//	int WriteCFG(void);
	int Read_Smt(void);
	int AnalizeRecodrRead(char *name, char *par);
	int Write_Smt(void);

	int Start(void);
	void Server(void);
	int GetFreeClient(void);

};


#endif // SMARTSERVER_H
