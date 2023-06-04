/* SmartServer.hpp */
#ifndef SMARTSERVER_H
#define SMARTSERVER_H

#include "Smart_ServerConfig.h"
#include "mybuffer.hpp"
#include "SmartDevice.h"

#define CONFIG_VERSION 0
#include "AutoConfig.h"

#define MAX_CLIENTS 32

class SmartServer;

class SmartServerClient
{
 public:
	int Ctype;       /* -1 - не инициализирован, 0 - COM, 1 - UDP, 2 - TCP*/
	int port;       /* порт/номер компорта */
	int sts;        /* -1 - не инициализирован 0 - стартовал 1 - онлайн 2 закончился -2 критическая ошибка */
	char ipaddr[32];
	int Debuglog;
	int verbose;
	time_t tlastc; /* время последней коммуникации */
	
	SmartServer *pserv; //parent pointer

	SmartServerClient(void)
	{	Ctype = -1;
		port = 0x0;
		sts = -1;
		ipaddr[0] = 0;
		Debuglog = 0;
		pserv = NULL;
		tlastc = 0;
		verbose = 1;
	}
};

class SmartServerClientTCPUDP:public SmartServerClient
{
 public:
    char ReadBuff[1500];
    char WriteBuff[1500];
    int online;
/*    HANDLE hmtx_LPC_io; мутексный семафор для блокировки одновременного доступа из разных тредов */
	int timeout;                //таймаут на соединение в мс
	int timeoutAnswer;          //таймаут на повторение посылки в мс

	unsigned char Mac[6]; //MAC address
	int BoilerStatus;
	float BoilerT, RetT;
	float dhw_t;   // DHW temperature (°C)
	float FlameModulation; // Relative Modulation Level setting (%)
	float Pressure; // ater pressure in CH circuit  (bar)
	int ExtTstatus; //статус внешних датчиков температуры - (не OT)
	float ExtT1;    // внешняя температура 1 - (не OT)
	float ExtT2;    // внешняя температура 2 - (не OT)


	UDPconnection udp;   //udp коннекшен
	TCPconnection tcp;

	 SmartServerClientTCPUDP(void)
	 {  Ctype = 0;
		online = 0;
		timeout = 2000;
		timeoutAnswer = 1000;
		memset(Mac,0,6);
		BoilerStatus = 0;
		BoilerT = RetT =0.;
		dhw_t = ExtT1 = ExtT1 = 0.;
		FlameModulation = 0.;
		Pressure = 0.; 
		ExtTstatus = 0; 
	 }
	~SmartServerClientTCPUDP(void)
	{//	if (hmtx_LPC_io) 
	 //		CloseHandle(hmtx_LPC_io);
	}

	int createConnection(int _type, int _timeout, int _timeoutAnswer, int verboze);
	int closeConnection(void);
	int InitClientConnection(void);

	int Start_Thread(void);
	 int Start(void);
	 int Identify(void);
    int GetSts0( int cmd, struct Msg1 *MsgIn, int lIn, struct Msg1 *MsgOut, int lOut);
    int GetSts0v(int cmd, struct Msg2 *MsgIn, int lIn, struct Msg2 *MsgOut, int lOut, int shiftLansw, int size0);
    int GetTime(time_t *t0);
    int SetTime(struct _timeb *t0);
	int SetUDPServerInfo(int _sts, int hport, int infotime);
	int SetTCPServerInfo(int _sts, char ipaddr[32], int hport, int infotime);
    int GetData(void);
    int TestCmd(int id, int par);
    int TestCmdAnswer(int &responce, int &status);

	 int InformHost_on_ServerState(char IPaddr[20], int hport, int state);
};


class SmartServerBase
{
public:
    char ReadBuff[1500];
    char WriteBuff[1500];
	int nc; // число клиентов
	int sts;        /* -1 - не инициализирован 0 - стартовал,  >1 - онлайн  2- stoped */
	int errsts;      /* -1 - не инициализирован 0 - Ok,  1 - error, 2 - critical error */
	int port;       /* порт/номер компорта */

	SmartServerBase(void)
	{
		port = 0;
	    nc = 0;
		sts = -1;
		errsts = -1;
	}
/*
   int ReadConfig(char *fname);
   int WriteConfig(char *fname);
   int AnalizeRecodrRead(char *name, char *par);
   int ReadStr(FILE *fp, char *str, char *nameClass, char *name, char *par);
*/
};

class SmartServerTCPUDP:public SmartServerBase
{
 public:
	SmartServer *pserv; //parent pointer
	int type; //2 = udp, 4 = tcp
	 UDPconnection udpserver;   //udp коннекшен
	 TCPconnection tcpserver;   //udp коннекшен
     SmartServerClientTCPUDP client [MAX_CLIENTS];

	 SmartServerTCPUDP(void)
	 { type = 2;
//		for(int i = 0; i<MAX_CLIENTS; i++) 
//					client[i].pserv = this;
	 }
	 int Start(void);
	 int InformHost_on_ServerState(char IPaddr[20], int hport, int state);
     int ParseFromUDP(char *buf, int nb);
     int ParseFromTCP(char *buf, int nb, char IPaddr[20], int &Lsend);
     int SendCmdToTCP(char *buf, int nb, char IPaddr[20]);
	 int OpenThermInfo(unsigned char *buf, int nb);
};



class SmartServer
{
 public:
static char *Appname;
static char *Version;
static char *SubVersion;
	int configversion;
	int type;       /* -1 - не инициализирован, 1 - COM, 2 - UDP, 4 - TCP; 6 = UDP+TCP*/
	int sts;        /* -1 - не инициализирован 0 - стартовал,  >1 - онлайн */
	int mode;		/*  0 - сервер, 1 - клиент */
	int errsts;      /* -1 - не инициализирован 0 - Ok,  1 - error, 2 - critical error */
	int tcp_port;
	int udp_port;

	int nd; // число устройств
	int ComPort;
	int baudrate;
	char CfgName[256];

    char ServerName[128];
    char ServerVersion[128];
	char Server_ipaddr[32]; //Server ip addr

//    SmartServerClientUDP client [MAX_CLIENTS];
	SmartServerClientTCPUDP pc_client; //PC - client. esp - server
    SmartServerTCPUDP server;
//	UDPconnection remoteudp_server;
//	TCPconnection remotetcp_server;
	
	SmartDevice_stub  *pdevice[MAX_DEVICES];
	AutoConfig cfg;

	SmartServer(void)
	{  
		configversion =-1;
		type = 2;
		mode = 1;
		sts = errsts = -1;
	     nd = 0;
		udp_port = 0x1a70; //6768
		tcp_port = 8876;

		ComPort = -1;
		baudrate = 115200;
		strcpy(CfgName, "SmartServer.cfg");
		strcpy(pc_client.ipaddr,"127.0.0.1");
		strcpy(Server_ipaddr,"127.0.0.1");	
		pc_client.port = 6769;
		server.pserv = this;
		for(int i = 0; i<MAX_DEVICES; i++) 
			pdevice[i] = NULL;

	}
	int Start(void);
	int Stop(void);
	int StartClient(int testmode);
	int TestOpenTherm(void);

	
	int init(char *ConfigFile);
	int ReadCFG(void);
	int WriteCFG(void);
	int RenameToBak(char *fname);

	int ReadDevSts(void);
	int WriteDevSts(void);
	int GetDevId(unsigned char mac[6]);
    int AddDev(unsigned char mac[6], int _type);

};


struct ClientPar
{  SmartServer *p_server;  //адрес SmartServer
   int ind;  // индекс клиента
};

#endif // SMARTSERVER_H
