/* SmartDevice.h */
#ifndef SMARTDEVICE_H
#define SMARTDEVICE_H

#include "DeviceType.h"

#define OTlogMAX 1024 //максимальна€ длина лога в элементах
#define OTlog_NMAX 10 //максимальное число логов, при превышении идем по кругу
#define OTlog_EL	8 //длина элемента лога в байтах
//OT лог
class Smart_OTlog
{
 public:
	short int lOTlog; //текущий размер OT лога в ST в элементах от ST
	short int indOTlog; //восстановленный индекс запрошенного элемента лога
	unsigned int tOTlog;//восстановленное врем€ в мс запрошенного элемента лога
	unsigned char *pbuf; //буфер
	int nbuf; //элементов в буфере
	int lbuf; //длина буфера
	int indLog; //индекс лога
	time_t t_start; // time of log
	short int memberCode;
	
	 Smart_OTlog(void)
	 {	lOTlog = 0;
		indOTlog = 0;
		tOTlog = 0;
		pbuf = NULL;
		nbuf = 0;
		lbuf = 0;
		indLog = -1; 
		t_start = 0;
		memberCode = -1;
	 }
	 int InitLog(int nl);
	 int cmdLog(int nl);
	 int FinishLog(void);
	 int Add_OT_log(void *_p, int n);
	 int writeLog(char *fname);

};

enum SmartTermSTS
{
	STS_RESTART=0,
	STS_INIT=1,
	ASK_STS=2,
	SET_STS=3,
	ASK_TIME=4,
	SET_TIME=5,
	STS_REPORT,
	STS_REPORT_ANSWER,
	STS_ERROR,
	STS_PAUSE
};

class SmartApp;
class SmartServer;

class SmartDevice_stub
{

 public:
	int type;  // тип-класс устройства: 1 SmartTerm, 2  App
    SmartTermSTS sts;   /* состо€ние */
	int BiosCode;     /* код биоса */
	int Vers;         /* верси€ */
	int SubVers;      /* подверси€ */
	int SubVers1;     /* верси€ подверсии */
	char BiosDate[12];  /* дата компил€ции биоса */
	int ClientId;     /* »дентификатор клиента */
	int ClientId_k;   /* »дентификатор клиента key (todo future use)*/
    char ipaddr[32];  /* IP адрес последней коммуникации */
	unsigned char mac[6];    
	int TCPserver_repot_period;  /* периодичность отправки данных серверу, сек */
	int flag_need_to_set_rp; 
	int TCPserver_repot_period_toset;  /*надо установить в TCPserver_repot_period_toset периодичность отправки данных серверу, сек */
	time_t t_need_toset_rp; /* врем€ когда устанавливаем TCPserver_repot_period_toset  */

	time_t tlastc; /* врем€ последней коммуникации */
	SmartServer *parent;

	SmartDevice_stub(void)
	{
		type = -1;
		sts = STS_INIT;
		mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = mac[5] = 0;
		BiosCode = 0;
		Vers = SubVers = SubVers1 = 0;
		BiosDate[0] = 0;
		TCPserver_repot_period = 30; //600;
		flag_need_to_set_rp = 0;
		TCPserver_repot_period_toset = 30;
		t_need_toset_rp  = 0;
		tlastc = 0;
		parent = NULL;
		ClientId = ClientId_k = 0;
		ipaddr[0] = 0;
	}
	void MakeClientId(int ind);
	void SetReportPeriod(int rp_toset, int wait_sec);
	int GetReportPeriod(void);
	int Log(int level, const char *_format, ...);

};


/* контроллер */
class SmartTerm:public SmartDevice_stub
{
	public:
	int st_type; //controller type: 0 - st32, 1 - st, 2 - st2
	short int B_flags;
	short int OTmemberCode;
//-2 not defined  -1 not init, 0 - normal work, 2 - timeout
//-2 = информаци€ еще не получена (серевер перезапустилс€, контроллер не прислал данных)
	short int stsOT; 
	int BoilerStatus;
	time_t t_lastwork; // time of last stsOT = 0
	float Tset;    // Control setpoint  ie CH  water temperature setpoint (∞C)
	float Tset_r;  // Temp set from responce
	float BoilerT; // Boiler flow water temperature (∞C) CH
	float RetT;    // Return water temperature (∞C) CH
	float TdhwSet; // f8.8  DHW setpoint (∞C)    (Remote parameter 1)
	float dhw_t;   // DHW temperature (∞C)
	float FlameModulation; //Relative Modulation Level (%)
	float Pressure; // Water pressure in CH circuit  (bar)
	int	  stsDS;
	float t1;
	float t2;
	float Toutside; 
	float tempindoor;
	float tempoutdoor;
	float TroomTarget;
	float InT;
	float U0;

	int isCmd; /* 0 - no cmd, 1 - is cmd from app to controller */
	short int B_flags_toSet;
	float Tset_toSet;    
	float TroomTarget_toSet;
	float TdhwSet_toSet;
	short int StartSts;
	short int reset_reason[2];
	short int lOTlog;
	short int indOTlog;
	unsigned int tOTlog;

	SmartApp *pSmA;
	Smart_OTlog OTlog;

	unsigned int ns;  // сколько раз передал информацию (дл€ расчета потребленной энергии)

	SmartTerm(void)
	{	st_type = 0; //st32 or undefined
		ns = 0;
		B_flags = 0;
		stsOT = -2;
		BoilerStatus = 0;
		t_lastwork = 0;
		BoilerT = RetT = 0.f; 
		Tset = Tset_r = 0.f;
		TdhwSet = dhw_t = 0.f;
		FlameModulation = Pressure = 0.f;
		t1 = t2 = 0.f;
		Toutside = 0.f;
		TroomTarget = tempindoor = tempoutdoor = 0.f;
		stsDS = 0;
		TCPserver_repot_period = 20; //600;
		isCmd = 0;
		pSmA = NULL;
		StartSts = 0;
		reset_reason[0] = reset_reason[1] = 0;
		InT = U0 = 0;
		lOTlog = 0;
		indOTlog = 0;
		tOTlog = 0;
		OTmemberCode = -1; //неизвестно
	}
//    int	WriteSts(FILE *fp);
//    int	ReadSts(FILE *fp);
	int loop1( struct Msg1 *out, int &outb);
	int ask_sts(struct Msg1 *out, int &outb);
	int set_sts(struct Msg1 *out, int &outb);
	int callback_Get_OT_Info(struct Msg1 *in, int inb, struct Msg1 *out, int &outb); 
	int callback_Get_Sts(struct Msg1 *in, int inb, struct Msg1 *out, int &outb); 
//	int callback_Get_Sts(struct Msg1 *in, int inb); 
	int callback_Send_Sts(struct Msg1 *in, int inb, struct Msg1 *out, int &outb); 
	int callback_Send_OTlog(struct Msg1 *in, int inb, struct Msg1 *out, int &outb); 
	int write(FILE *fp);
	int AnalizeRecodrRead(char *name, char *par);
	int Write_OT_log(void *p, int n);
};

/* приложение */
class SmartApp:public SmartDevice_stub
{
 public:
	SmartTerm *pSmt;
	SmartApp(void)
	{	TCPserver_repot_period = 5; //600;
		pSmt = NULL;
	}
	int loop1( struct Msg1 *out, int &outb);
	int callback_get_sts(struct Msg1 *in, int inb, struct Msg1 *out, int &outb);
	int callback_set_state(struct Msg1 *in, int inb, struct Msg1 *out, int &outb);

	void OnIdentify(void);
	void OnClose(void);
};



#endif //SMARTDEVICE_H
