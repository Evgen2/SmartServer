/* OpenThermTest.cpp */
#include "stdafx.h"
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include <math.h>

#include "UDPconnection.hpp"
#include "SmartServer.hpp"

#include "OpenThermTest.h"

char *OpenThermStatusTxt[9] =
{
	"NOT_INITIALIZED",
	"READY",
	"DELAY",
	"REQUEST_SENDING",
	"RESPONSE_WAITING",
	"RESPONSE_START_BIT",
	"RESPONSE_RECEIVING",
	"RESPONSE_READY",
	"RESPONSE_INVALID"
};

char *OpenThermMessageTypeTxt[8] =
{
	/*  Master to Slave */
	"READ_DATA",
	"WRITE_DATA",
	"INVALID_DATA",
	"RESERVED",
	/* Slave to Master */
	"READ_ACK",
	"WRITE_ACK",
	"DATA_INVALID",
	"UNKNOWN_DATA_ID"
};

char *OpenThermResponseStatusTxt[4] =
{
	"NONE",
	"SUCCESS",
	"INVALID",
	"TIMEOUT"
};

#if defined(ARDUINO_ARCH_ESP8266)
	#define OTD(x) 
	#define OTDN(x)
#elif defined(ARDUINO_ARCH_ESP32)
	#define OTD(x) x
	#define OTDN(x)
#else
	#define OTD(x) x
	#define OTDN(x) ,#x
#endif

OpenThermID OT_ids[N_OT_NIDS] =
{
 {	Status,					OtRW,	FLAG8,	FLAG8,	1,	0,	0,	OTD("Master and Slave Status flags") OTDN(Status) }, 
 { 	TSet,					OtW,	F88,	OTNONE,	0,	0,	0,	OTD("Control setpoint ie CH water temperature setpoint (°C)") OTDN(Tset)},
 {	MConfigMMemberIDcode,	OtW,	FLAG8,	OTU8,	0,	0,	0,	OTD("Master Configuration Flags / Master MemberID Code") OTDN(MConfigMMemberIDcode) }, 
 {	SConfigSMemberIDcode,	OtR,	FLAG8,	OTU8,	0,	0,	0,	OTD("Slave Configuration Flags / Slave MemberID Code") OTDN(SConfigSMemberIDcode) },
 {	Command,				OtW,	OTU8,	OTU8,	0,	0,	0,	OTD("Remote Command"  OTDN(Command) )} ,
 {	ASFflags,				OtR,	FLAG8,	OTU8,	0,	0,	0,	OTD("OEM-fault-code Application-specific fault flags and OEM fault code")  OTDN(ASFflags) },
 {	RBPflags,				OtR,	FLAG8,	FLAG8,	0,	0,	0,	OTD("Remote boiler parameter transfer-enable & read/write flags")  OTDN(RBPflags) },
 {	CoolingControl,			OtW,	F88,	OTNONE,	0,	0,	0,	OTD("Cooling control signal (%)") OTDN(CoolingControl) },
 { 	TsetCH2,				OtW,	F88,	OTNONE,	0,	0,	0,	OTD("Control setpoint for 2e CH circuit (°C)") OTDN(TsetCH2) },
 { 	TrOverride,				OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Remote override room setpoint") OTDN(TrOverride) },

 { 	TSP,					OtR,	OTU8,	OTU8,	0,	0,	0,	OTD("Number of Transparent-Slave-Parameters supported by slave") OTDN(TSP) },
 { 	TSPindexTSPvalue,		OtRW,	OTU8,	OTU8,	0,	0,	0,	OTD("Index number / Value of referred-to transparent slave parameter") OTDN(TSPindexTSPvalue) },
 { 	FHBsize,				OtR,	OTU8,	OTU8,	0,	0,	0,	OTD("Size of Fault-History-Buffer supported by slave") OTDN(FHBsize) },
 { 	FHBindexFHBvalue,		OtR,	OTU8,	OTU8,	0,	0,	0,	OTD("Index number / Value of referred-to fault-history buffer entry") OTDN(FHBindexFHBvalue) },
 { 	MaxRelModLevelSetting,	OtW,	F88,	OTNONE,	0,	0,	0,	OTD("Maximum relative modulation level setting (%)") OTDN(MaxRelModLevelSetting) },
 { 	MaxCapacityMinModLevel,	OtR,	OTU8,	OTU8,	0,	0,	0,	OTD("Maximum boiler capacity (kW) / Minimum boiler modulation level(%)") OTDN(MaxCapacityMinModLevel) },
 {	TrSet,					OtW,	F88,	OTNONE,	0,	0,	0,	OTD("Room Setpoint (°C)") OTDN(TrSet) },
 { 	RelModLevel,			OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Relative Modulation Level (%)") OTDN(RelModLevel) },
 { 	CHPressure,				OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Water pressure in CH circuit  (bar)") OTDN(CHPressure) },
 { 	DHWFlowRate,			OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Water flow rate in DHW circuit. (litres/minute)") OTDN(DHWFlowRate) },

 {	DayTime,				OtRW,	OTSP, 	OTU8, 	0,	0,	0,	OTD("Day of Week and Time of Day") OTDN(DayTime) },
 {	Date,					OtRW, 	OTU8,	OTU8,	0,	0,	0,	OTD("Calendar date") OTDN(Date) },
 {	Year, 					OtRW,	OTU16,	OTNONE,	0,	0,	0,	OTD("Calendar year") OTDN(Year) },
 {	TrSetCH2,				OtW,	F88,	OTNONE,	0,	0,	0,	OTD("Room Setpoint for 2nd CH circuit (°C)") OTDN(TrSetCH2) },
 {	Tr, 					OtW,	F88,	OTNONE,	0,	0,	0,	OTD("Room temperature (°C)") OTDN(Tr) },
 {	Tboiler, 				OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Boiler flow water temperature (°C)") OTDN(Tboiler) },
 {	Tdhw, 					OtR,	F88,	OTNONE,	0,	0,	0,	OTD("DHW temperature (°C)") OTDN(Tdhw) },
 {	Toutside, 				OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Outside temperature (°C)") OTDN(Toutside) },
 {	Tret, 					OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Return water temperature (°C)") OTDN(Tret) },
 {	Tstorage, 				OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Solar storage temperature (°C)") OTDN(Tstorage) },

 {	Tcollector,				OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Solar collector temperature (°C)") OTDN(Tcollector) }, //s16 (p26) or f8.8 (p32) ?? 
 {	TflowCH2,				OtR,	F88,	OTNONE,	0,	0,	0, 	OTD("Flow water temperature CH2 circuit (°C)") OTDN(TflowCH2) },
 {	Tdhw2,					OtR,	F88,	OTNONE,	0,	0,	0, 	OTD("Domestic hot water temperature 2 (°C)") OTDN(Tdhw2) },
 {	Texhaust,				OtR,	OTS16,	OTNONE,	0,	0,	0, 	OTD("Boiler exhaust temperature (°C)") OTDN(Texhaust) },
 {	TdhwSetUBTdhwSetLB,		OtRW,	OTS8,	OTS8,	0,	0,	0, 	OTD("DHW setpoint upper & lower bounds for adjustment (°C)") OTDN(TdhwSetUBTdhwSetLB) }, //48 
 {	MaxTSetUBMaxTSetLB, 	OtRW,	OTS8,	OTS8,	0,	0,	0, 	OTD("Max CH water setpoint upper & lower bounds for adjustment (°C)") OTDN(MaxTSetUBMaxTSetLB) },
 {	HcratioUBHcratioLB,  	OtRW,	OTS8,	OTS8,	0,	0,	0, 	OTD("OTC heat curve ratio upper & lower bounds for adjustment") OTDN(HcratioUBHcratioLB) },
 {	TdhwSet,				OtRW,	F88,	OTNONE,	0,	0,	0, 	OTD("DHW setpoint (°C) (Remote parameter 1)") OTDN(TdhwSet) }, // 56
 {	MaxTSet,				OtRW,	F88,	OTNONE,	0,	0,	0, 	OTD("Max CH water setpoint (°C) (Remote parameters 2)") OTDN(MaxTSet) },
 {	Hcratio,				OtRW,	F88,	OTNONE,	0,	0,	0, 	OTD("f8.8  OTC heat curve ratio (°C) (Remote parameter 3)") OTDN(Hcratio) },

 {	RemoteOverrideFunction,	OtR,	FLAG8,	OTNONE,	0,	0,	0, OTD("Function of manual and program changes in master and remote room setpoint") OTDN(RemoteOverrideFunction) }, // = 100
 {	OEMDiagnosticCode,		OtR,	OTU16,	OTNONE,	0,	0,	0, OTD("OEM-specific diagnostic/service code") OTDN(OEMDiagnosticCode) }, // = 115
 {	BurnerStarts,			OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of starts burner") OTDN(BurnerStarts) }, 
 {	CHPumpStarts,			OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of starts CH pump") OTDN(CHPumpStarts) },
 {	DHWPumpValveStarts, 	OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of starts DHW pump/valve") OTDN(DHWPumpValveStarts) },
 {	DHWBurnerStarts,		OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of starts burner during DHW mode") OTDN(DHWBurnerStarts) },
 {	BurnerOperationHours,	OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of hours that burner is in operation (i.e. flame on)") OTDN(BurnerOperationHours) },
 {	CHPumpOperationHours, 	OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of hours that CH pump has been running") OTDN(CHPumpOperationHours) },
 {	DHWPumpValveOperationHours,
						 	OtRW,	OTU16,	OTNONE,	0,	0,	0,	OTD("Number of hours that DHW pump has been running or DHW valve has been opened") OTDN(DHWPumpValveOperationHours) },
 {	DHWBurnerOperationHours, 
						 	OtRW,	OTU16,	OTNONE,	0,	0,	0,	OTD("Number of hours that burner is in operation during DHW mode") OTDN(DHWBurnerOperationHours) },

 {	OpenThermVersionMaster,	OtW,	F88,	OTNONE,	0,	0,	0,	OTD("The implemented version of the OpenTherm Protocol Specification in the master") OTDN(OpenThermVersionMaster) },
 {	OpenThermVersionSlave,	OtR,	F88,	OTNONE,	0,	0,	0,	OTD("The implemented version of the OpenTherm Protocol Specification in the slave") OTDN(OpenThermVersionSlave) },
 {	MasterVersion,			OtW, 	OTU8,	OTU8,	0,	0,	0,	OTD("Master product version number and type") OTDN(MasterVersion) },
 {	SlaveVersion,			OtR, 	OTU8,	OTU8,	0,	0,	0,	OTD("Slave product version number and type") OTDN(SlaveVersion) }
 
};

OpenTherm ot;

int SmartServer::TestOpenTherm(void)
{ int id,idd, rc, par = 0, response=0, respstatus=0, messagetype;
    uint16_t u88;
	float t;
	FILE *fp, *fp1;
	fp = fopen("OpenThermU.txt","w");
	fp1 = fopen("OpenThermN.txt","w");
	for(id=0; id<N_OT_NIDS; id++)
	{ // printf("%s %d\n", OT_ids[id].name, OT_ids[id].id);
		if(OT_ids[id].rw == OtR || OT_ids[id].rw == OtRW)
		{   printf("OTid %d %s:",  OT_ids[id].id, OT_ids[id].name);  fflush(stdout);
			par = 0;
			rc = pc_client.TestCmd(OT_ids[id].id, par);
			if(rc == 0)
			{  Sleep(1000);
			}
			rc = pc_client.TestCmdAnswer(response, respstatus);
			if(rc == 0)
			{  //printf("response=%x ", response);
				if(respstatus >= 0 && respstatus <= 3)
					printf("Sts %s (%d)", OpenThermResponseStatusTxt[respstatus], respstatus);
				else
					printf("Sts %d", respstatus);
				printf("\n");
				if(respstatus == OpenThermResponseStatus::INVALID)
				{	fprintf(fp1,"%d %s %d\n", OT_ids[id].id, OT_ids[id].name,  respstatus);
					continue;
				}
				 messagetype = ot.getMessageType(response);
				 idd = (response >> 16 & 0xFF);
				 if(messagetype != OpenThermMessageType::READ_ACK)
					printf("msgtype %s id %d\n",OpenThermMessageTypeTxt[messagetype], idd);
				if(idd != OT_ids[id].id)
				{	printf("Error response id %d not equal %d\n", idd, OT_ids[id].id);
					fprintf(fp,"\n");
				    continue;
				}
				//printf("%s\n",OT_ids[id].descript);
				fprintf(fp,"%d %s ", OT_ids[id].id, OT_ids[id].name);

			    u88 = (response & 0xffff);
			    t = (u88 & 0x8000) ? -(0x10000L - u88) / 256.0f : u88 / 256.0f;
		{  unsigned char b;
		   char c;
		   unsigned short int b2;
		   short int c2;

				if(OT_ids[id].ptype1 == F88)
				{	fprintf(fp," %f", t);
				} else if(OT_ids[id].ptype1 == FLAG8) {
					fprintf(fp," %04x", u88 & 0x0ff);
				} else if(OT_ids[id].ptype1 == OTU8) {
					b = u88 & 0x0ff;
					fprintf(fp," %d", b);
				} else if(OT_ids[id].ptype1 == OTS8) {
					c = u88 & 0x0ff;
					fprintf(fp," %d", c);
				} else if(OT_ids[id].ptype1 == OTU16) {
					b2 = u88;
					fprintf(fp," %d", b2);
				} else if(OT_ids[id].ptype1 == OTS16) {
					c2 = u88;
					fprintf(fp," %d", c2);
				} else if(OT_ids[id].ptype1 == OTSP) {
					fprintf(fp," %04x", u88);
				}

				if(OT_ids[id].ptype2 == FLAG8) {
					fprintf(fp,"  %04x", (u88>>8) & 0x0ff);
				} else if(OT_ids[id].ptype2 == OTU8) {
					b = (u88>>8) & 0x0ff;
					fprintf(fp," %d", b);
				} else if(OT_ids[id].ptype2 == OTS8) {
					c = (u88>>8) & 0x0ff;
					fprintf(fp," %d", c);
				}
				fprintf(fp,"\n");

		}


				switch(idd)
				{
					case OpenThermMessageID::Status:  //0
				printf("Status: %04x ", u88);
				if(u88 & 0x01)
						printf("Fault ");
				if(u88 & 0x02)
						printf("CH ");
				if(u88 & 0x04)
						printf("HW ");
				if(u88 & 0x08)
						printf("Flame ");
				if(u88 & 0x10)
						printf("Cooling ");
				if(u88 & 0x20)
						printf("CH2 ");
				if(u88 & 0x40)
						printf("Diagnostic ");
				if(u88 & 0x80)
						printf("Reserved ");
				if(u88 & 0xff00)
						printf("Master status != 0");

				printf("\n");

						break;
					case OpenThermMessageID::SConfigSMemberIDcode:  //3
				printf("flags: %04x ", u88); //d194)a 10 b 11 c 12 d 13   d = 8+4+1   d1 1101 0001  
				printf("\n");
						break;

					case OpenThermMessageID::ASFflags: //5
				printf("flags: %04x\n", u88);
						break;
					case OpenThermMessageID::TrOverride: //9
				printf("flags: %04x\n", u88);
						break;
					case OpenThermMessageID::TSP: //10
				printf("flags: %04x\n", u88);
						break;
					case OpenThermMessageID::TSPindexTSPvalue: //11
				printf("flags: %04x\n", u88);
						break;
						
					case OpenThermMessageID::FHBsize: //12
				printf("flags: %04x\n", u88);
						break;
					case OpenThermMessageID::FHBindexFHBvalue: //13
				printf("flags: %04x\n", u88);
						break;
						

						
					case OpenThermMessageID::DHWBurnerStarts: //119
				printf(": %d\n", u88);
						break;
					case OpenThermMessageID::BurnerOperationHours: //120
				printf("Hours: %d\n", u88);
						break;
					case OpenThermMessageID::CHPumpOperationHours: //121
				printf("Hours: %d\n", u88);
						break;
					case OpenThermMessageID::DHWPumpValveOperationHours: //122
				printf("Hours: %d\n", u88);
						break;
					case OpenThermMessageID::DHWBurnerOperationHours: //123
				printf("Hours: %d\n", u88);
						break;
					case OpenThermMessageID::OpenThermVersionSlave:  //125
				printf("OT Version: %d %d\n", (u88>>8), (u88&0xff));
						break;
					case OpenThermMessageID::SlaveVersion:  //127
				printf("Version: %d %d\n", (u88>>8), (u88&0xff));
						break;
				}
			}

		}
	}

	fclose(fp);
	fclose(fp1);
	return 0;
}

/********************************************************/
OpenTherm::OpenTherm(int inPin, int outPin, bool isSlave):
	status(OpenThermStatus::NOT_INITIALIZED),
	inPin(inPin),
	outPin(outPin),
	isSlave(isSlave),
	response(0),
	responseStatus(OpenThermResponseStatus::NONE),
	responseTimestamp(0),
	handleInterruptCallback(NULL),
	processResponseCallback(NULL)
{
	status = NOT_INITIALIZED;
	Lastresponse = 0;

}

bool OpenTherm::parity(unsigned long frame) //odd parity
{
	byte p = 0;
	while (frame > 0)
	{
		if (frame & 1) p++;
		frame = frame >> 1;
	}
	return (p & 1);
}

OpenThermMessageType OpenTherm::getMessageType(unsigned long message)
{
	OpenThermMessageType msg_type = static_cast<OpenThermMessageType>((message >> 28) & 7);
	return msg_type;
}

OpenThermMessageID OpenTherm::getDataID(unsigned long frame)
{
	return (OpenThermMessageID)((frame >> 16) & 0xFF);
}
