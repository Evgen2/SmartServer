/* OpenTherm.h */
/*
OpenTherm.h - OpenTherm Library for the ESP8266/Arduino platform
https://github.com/ihormelnyk/OpenTherm
http://ihormelnyk.com/pages/OpenTherm
Licensed under MIT license
Copyright 2018, Ihor Melnyk

Frame Structure:
P MGS-TYPE SPARE DATA-ID  DATA-VALUE
0 000      0000  00000000 00000000 00000000
*/

#ifndef OpenTherm_h
#define OpenTherm_h

#include <stdint.h>
//#include <Arduino.h>
#define B000 0
#define B001 1
#define B010 2
#define B011 3
#define B100 4
#define B101 5
#define B110 6
#define B111 7


enum OpenThermResponseStatus {
	NONE,
	SUCCESS,
	INVALID,
	TIMEOUT
};


enum OpenThermMessageType {
	/*  Master to Slave */
	READ_DATA       = B000,
	READ            = READ_DATA, // for backwared compatibility
	WRITE_DATA      = B001,
	WRITE           = WRITE_DATA, // for backwared compatibility
	INVALID_DATA    = B010,
	RESERVED        = B011,
	/* Slave to Master */
	READ_ACK        = B100,
	WRITE_ACK       = B101,
	DATA_INVALID    = B110,
	UNKNOWN_DATA_ID = B111
};

typedef OpenThermMessageType OpenThermRequestType; // for backwared compatibility

enum OpenThermMessageID {
	Status,					// 0 flag8 / flag8  Master and Slave Status flags.
	TSet,					// 1 f8.8  Control setpoint  ie CH  water temperature setpoint (°C)
	MConfigMMemberIDcode,	// 2 flag8 / u8  Master Configuration Flags /  Master MemberID Code
	SConfigSMemberIDcode,	// 3 flag8 / u8  Slave Configuration Flags /  Slave MemberID Code
	RemoteRequest,			// 4 u8 / u8  Remote Command
	ASFflags,				// 5 OEM-fault-code  flag8 / u8  Application-specific fault flags and OEM fault code
	RBPflags,				// 6 flag8 / flag8  Remote boiler parameter transfer-enable & read/write flags
	CoolingControl,			// 7 f8.8  Cooling control signal (%)
	TsetCH2,				// 8 f8.8  Control setpoint for 2e CH circuit (°C)
	TrOverride,				// 9 f8.8  Remote override room setpoint
	TSP,					// 10 u8 / u8  Number of Transparent-Slave-Parameters supported by slave
	TSPindexTSPvalue,		// 11 u8 / u8  Index number / Value of referred-to transparent slave parameter.
	FHBsize,				// 12 u8 / u8  Size of Fault-History-Buffer supported by slave
	FHBindexFHBvalue,		// 13 u8 / u8  Index number / Value of referred-to fault-history buffer entry.
	MaxRelModLevelSetting,	// 14 f8.8  Maximum relative modulation level setting (%)
	MaxCapacityMinModLevel, // 15 u8 / u8  Maximum boiler capacity (kW) / Minimum boiler modulation level(%)
	TrSet,					// 16 f8.8  Room Setpoint (°C)
	RelModLevel,			// 17 f8.8  Relative Modulation Level (%)
	CHPressure,				// 18 f8.8  Water pressure in CH circuit  (bar)
	DHWFlowRate,			// 19 f8.8  Water flow rate in DHW circuit. (litres/minute)
	DayTime,				// 20 special / u8  Day of Week and Time of Day
	Date,					// 21 u8 / u8  Calendar date
	Year,					// 22 u16  Calendar year
	TrSetCH2,				// 23 f8.8  Room Setpoint for 2nd CH circuit (°C)
	Tr,						// 24 f8.8  Room temperature (°C)
	Tboiler,				// 25 f8.8  Boiler flow water temperature (°C)
	Tdhw,					// 26 f8.8  DHW temperature (°C)
	Toutside,				// 27 f8.8  Outside temperature (°C)
	Tret,					// 28 f8.8  Return water temperature (°C)
	Tstorage,				// 29 f8.8  Solar storage temperature (°C)
	Tcollector,				// 30 f8.8  Solar collector temperature (°C)
	TflowCH2,				// 31 f8.8  Flow water temperature CH2 circuit (°C)
	Tdhw2					= 32, // 32 f8.8  Domestic hot water temperature 2 (°C)
	Texhaust				= 33, // 33 s16  Boiler exhaust temperature (°C)
    TboilerHeatExchanger				= 34, // f8.8    Boiler heat exchanger temperature(°C)
    BoilerFanSpeedSetpointAndActual		= 35, // u8/u8   Boiler fan speed Setpoint and actual value
    FlameCurrent						= 36, // f8.8    Electrical current through burner flame[μA]
    TrCH2								= 37, // f8.8    Room temperature for 2nd CH circuit(°C)
    RelativeHumidity					= 38, // f8.8    Actual relative humidity as a percentage
    TrOverride2                         = 39, // f8.8    Remote Override Room Setpoint 2

	TdhwSetUBTdhwSetLB					= 48, // s8 / s8  DHW setpoint upper & lower bounds for adjustment (°C)
	MaxTSetUBMaxTSetLB					= 49, // s8 / s8  Max CH water setpoint upper & lower bounds for adjustment (°C)
	HcratioUBHcratioLB					= 50, // (нет в матрице OT) s8 / s8  OTC heat curve ratio upper & lower bounds for adjustment
	TdhwSet								= 56, // f8.8  DHW setpoint (°C)    (Remote parameter 1)
	MaxTSet								= 57, // f8.8  Max CH water setpoint (°C)  (Remote parameters 2)
	Hcratio								= 58, //  (нет в матрице OT) f8.8  OTC heat curve ratio (°C)  (Remote parameter 3)
	
	StatusVH							= 70, // flag8 / flag8  Ventilation/HeatRecovery Master and Slave Status flags. HB/LB

    Vset                                        = 71, // -/u8  Relative ventilation position (0-100%). 0% is the minimum set ventilation and 100% is the maximum set ventilation. 
    ASFflagsOEMfaultCodeVentilationHeatRecovery = 72, // flag8/u8  Application-specific fault flags and OEM fault code ventilation / heat-recovery 
    OEMDiagnosticCodeVentilationHeatRecovery    = 73, // u16     An OEM-specific diagnostic/service code for ventilation / heat-recovery system 
    SConfigSMemberIDCodeVentilationHeatRecovery = 74, // flag8/u8  Slave Configuration Flags / Slave MemberID Code ventilation / heat-recovery 
    OpenThermVersionVentilationHeatRecovery     = 75, // f8.8    The implemented version of the OpenTherm Protocol Specification in the ventilation / heat-recovery system. 
    VentilationHeatRecoveryVersion              = 76, // u8/u8     Ventilation / heat-recovery product version number and type 
    RelVentLevel                                = 77, // -/u8  Relative ventilation (0-100%) 
    RHexhaust                                   = 78, // -/u8  Relative humidity exhaust air (0-100%) 
    CO2exhaust                                  = 79, // u16     CO2 level exhaust air (0-2000 ppm) 
    Tsi                                         = 80, // f8.8    Supply inlet temperature (°C) 
    Tso                                         = 81, // f8.8    Supply outlet temperature (°C) 
    Tei                                         = 82, // f8.8    Exhaust inlet temperature (°C) 
    Teo                                         = 83, // f8.8    Exhaust outlet temperature (°C) 
    RPMexhaust                                  = 84, // u16     Exhaust fan speed in rpm 
    RPMsupply                                   = 85, // u16     Supply fan speed in rpm 
    RBPflagsVentilationHeatRecovery             = 86, // flag8/flag8   Remote ventilation / heat-recovery parameter transfer-enable & read/write flags 
    NominalVentilationValue                     = 87, // u8/-  Nominal relative value for ventilation (0-100 %) 
    TSPventilationHeatRecovery                  = 88, // u8/u8     Number of Transparent-Slave-Parameters supported by TSP’s ventilation / heat-recovery 
    TSPindexTSPvalueVentilationHeatRecovery     = 89, // u8/u8     Index number / Value of referred-to transparent TSP’s ventilation / heat-recovery parameter. 
//  FHBsizeVentilationHeatRecovery              = 90, // u8/u8     Size of Fault-History-Buffer supported by ventilation / heat-recovery 
//  FHBindexFHBvalueVentilationHeatRecovery     = 91, // u8/u8     Index number / Value of referred-to fault-history buffer entry ventilation / heat-recovery 
    FHBsizeVHR									= 90, // u8/u8     Size of Fault-History-Buffer supported by ventilation / heat-recovery 
    FHBindexFHBvalueVHR							= 91, // u8/u8     Index number / Value of referred-to fault-history buffer entry ventilation / heat-recovery 

    Brand                                       = 93, // u8/u8     Index number of the character in the text string ASCII character referenced by the above index number 
    BrandVersion                                = 94, // u8/u8     Index number of the character in the text string ASCII character referenced by the above index number 

    BrandSerialNumber							= 95, // u8/u8     Index number of the character in the text string ASCII character referenced by the above index number 
	CoolingOperationHours						= 96, // u16     Number of hours that the slave is in Cooling Mode. Reset by zero is optional for slave 
    PowerCycles									= 97, // u16     Number of Power Cycles of a slave (wake-up after Reset), Reset by zero is optional for slave 
    RFsensorStatusInformation					= 98, // special/special   For a specific RF sensor the RF strength and battery level is written 
    RemoteOverrideOperatingModeHeatingDHW		= 99, // special/special   Operating Mode HC1, HC2/ Operating Mode DHW 

    RemoteOverrideFunction                     = 100, // flag8/-   Function of manual and program changes in master and remote room Setpoint 
    StatusSolarStorage                         = 101, // flag8/flag8   Master and Slave Status flags Solar Storage 
    ASFflagsOEMfaultCodeSolarStorage           = 102, // flag8/u8  Application-specific fault flags and OEM fault code Solar Storage 
    SConfigSMemberIDcodeSolarStorage           = 103, // flag8/u8  Slave Configuration Flags / Slave MemberID Code Solar Storage 
    SolarStorageVersion                        = 104, // u8/u8     Solar Storage product version number and type
    TSPSolarStorage                            = 105, // u8/u8     Number of Transparent - Slave - Parameters supported by TSP’s Solar Storage
    TSPindexTSPvalueSolarStorage               = 106, // u8/u8     Index number / Value of referred - to transparent TSP’s Solar Storage parameter.
    FHBsizeSolarStorage                        = 107, // u8/u8     Size of Fault - History - Buffer supported by Solar Storage
    FHBindexFHBvalueSolarStorage               = 108, // u8/u8     Index number / Value of referred - to fault - history buffer entry Solar Storage
    ElectricityProducerStarts                  = 109, // U16     Number of start of the electricity producer.
    ElectricityProducerHours                   = 110, // U16     Number of hours the electricity produces is in operation
    ElectricityProduction                      = 111, // U16     Current electricity production in Watt.
	CumulativElectricityProduction             = 112, // U16     Cumulative electricity production in KWh.
    UnsuccessfulBurnerStarts                   = 113, // u16     Number of un - successful burner starts
	FlameSignalTooLowNumber                    = 114, // u16     Number of times flame signal was too low
    OEMDiagnosticCode                          = 115, // u16     OEM - specific diagnostic / service code
	SuccessfulBurnerStarts                     = 116, // u16     Number of succesful starts burner
    CHPumpStarts                               = 117, // u16     Number of starts CH pump
    DHWPumpValveStarts                         = 118, // u16     Number of starts DHW pump / valve
    DHWBurnerStarts                            = 119, // u16     Number of starts burner during DHW mode
    BurnerOperationHours                       = 120, // u16     Number of hours that burner is in operation(i.e.flame on)
    CHPumpOperationHours                       = 121, // u16     Number of hours that CH pump has been running
	DHWPumpValveOperationHours                 = 122, // u16     Number of hours that DHW pump has been running or DHW valve has been opened
	DHWBurnerOperationHours                    = 123, // u16     Number of hours that burner is in operation during DHW mode
	OpenThermVersionMaster                     = 124, // f8.8    The implemented version of the OpenTherm Protocol Specification in the master.
    OpenThermVersionSlave                      = 125, // f8.8    The implemented version of the OpenTherm Protocol Specification in the slave.
    MasterVersion                              = 126, // u8/u8     Master product version number and type
    SlaveVersion                               = 127, // u8/u8     Slave product version number and type

	OT_ZontXZ_128							   = 128, // u8 / u8 ZontXZ_128
	OT_ZontXZ_129							   = 129, // u8 / u8 ZontXZ_129
	OT_RemehadFdUcodes							= 131, // u8 / u8 Remeha dF-/dU-codes
	OT_RemehaServicemessage						= 132, // u8 / u8 Remeha Servicemessage
	OT_RemehaDetectionConnectedSCU				= 133, // u8 / u8 Remeha detection connected SCU’s
	OT_ZontXZ_136							   = 136 // u8 / u8 ZontXZ_136

};

enum OpenThermStatus {
	NOT_INITIALIZED,
	READY,
	DELAY,
	REQUEST_SENDING,
	RESPONSE_WAITING,
	RESPONSE_START_BIT,
	RESPONSE_RECEIVING,
	RESPONSE_READY,
	RESPONSE_INVALID
};

struct OpenThermVendor
{	int id;
	const char *name;
};

// https://github.com/Jeroen88/EasyOpenTherm/blob/main/src/EasyOpenTherm.h
OpenThermVendor OTvendorList[] =
{	1,  "Baxi Fourtech/Luna 3",
    2,  "AWB/Brink/Viessmann",
	4,  "Baxi Slim",
    5,  "Itho Daalderop",
    6,  "IDEAL",
    8,  "Buderus/Bosch/Hoval",
	9,  "Ferrolli",
	11, "Remeha",
	16, "Unical",
	24, "Vaillant/Bulex",
	27, "Baxi Luna Duo-Tec P67=0",
	29, "Itho Daalderop",
	33, "Viessmann",
	41, "Italtherm/Radiant",
	56,	"Baxi Luna Duo-Tec P67=2",
	131, "Nefit",
    148, "Navien",
    173, "Intergas",
    247, "Baxi Ampera",
    248, "Zota Lux-X"
};


const char *OpenThermMessageTypeTxt[8] =
{
	/*  Master to Slave */
	"RD", //"READ_DATA ",
	"WD", //"WRITE_DATA",
	"INVALID_DATA",
	"RESERVED",
	/* Slave to Master */
	"RA", // "READ_ACK  ",
	"WA", // "WRITE_ACK ",
	"DATA_INVALID",
	"UNKNOWN_DATA_ID"
};

enum OpenThermRWtype {
	ONONE,
	OtR,
	OtW,
	OtRW
};

enum OpenThermMSGpartype {
	OTNONE,
	FLAG8, //flag8
	OTU8,  // u8
	OTS8,  // s8
	OTU16, // u16
	OTS16, // s16
	F88,   // f8.8
	OTSP   // special (DAY TIME only)
};


#define OTD(x) x
#define OTDN(x) ,#x

#define N_OT_NIDS 81

#pragma pack(1)
class OpenThermID
{   
public:
	byte /* OpenThermMessageID */ id;
	byte rw:2; //read 0x01, write 0x02
	byte ptype1:3; //parameter 1 type HB or HB+LB
	byte ptype2:3; //parameter 2 type LB
	byte used:2; //0 not used, 1 used, 2 not tested
	byte count:7; //
	byte countOk:7; //
#if defined(ARDUINO_ARCH_ESP8266)
//none
#elif defined(ARDUINO_ARCH_ESP32)
	const char *descript; //description
#else
	const char *descript; //description
	const char *name; //name
#endif
};
#pragma pack()


OpenThermID OT_ids[N_OT_NIDS] =
{
 {	Status,					OtRW,	FLAG8,	FLAG8,	1,	0,	0,	OTD("Master and Slave Status flags") OTDN(Status) }, 
 { 	TSet,					OtW,	F88,	OTNONE,	0,	0,	0,	OTD("Control setpoint ie CH water temperature setpoint (°C)") OTDN(Tset)},
 {	MConfigMMemberIDcode,	OtW,	FLAG8,	OTU8,	0,	0,	0,	OTD("Master Configuration Flags / Master MemberID Code") OTDN(MConfigMMemberIDcode) }, 
 {	SConfigSMemberIDcode,	OtR,	FLAG8,	OTU8,	0,	0,	0,	OTD("Slave Configuration Flags / Slave MemberID Code") OTDN(SConfigSMemberIDcode) },
 {	RemoteRequest,			OtW,	OTU8,	OTU8,	0,	0,	0,	OTD("Remote Command"  OTDN(Command) )} ,
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
 {	TboilerHeatExchanger,	OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Boiler heat exchanger temperature(°C)") OTDN(TboilerHeatExchanger) },
 { 	BoilerFanSpeedSetpointAndActual, 
							OtRW,	OTU8,	OTU8,	0,	0,	0,	OTD("Boiler fan speed Setpoint and actual value") OTDN(BoilerFanSpeedSetpointAndActual) },
 {	FlameCurrent,			OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Electrical current through burner flame[mkA]") OTDN(FlameCurrent) },
 {	TrCH2,					OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Room temperature for 2nd CH circuit(°C)") OTDN(TrCH2) },
 {	RelativeHumidity,		OtR,	F88,	OTNONE,	0,	0,	0,	OTD("RelativeHumidity") OTDN(RelativeHumidity) }, //38
 { 	TrOverride2,			OtR,	F88,	OTNONE,	0,	0,	0,	OTD("Remote Override Room Setpoint 2") OTDN(TrOverride) }, //39

 
 {	TdhwSetUBTdhwSetLB,		OtRW,	OTS8,	OTS8,	0,	0,	0, 	OTD("DHW setpoint upper & lower bounds for adjustment (°C)") OTDN(TdhwSetUBTdhwSetLB) }, //48 

 {	MaxTSetUBMaxTSetLB, 	OtRW,	OTS8,	OTS8,	0,	0,	0, 	OTD("Max CH water setpoint upper & lower bounds for adjustment (°C)") OTDN(MaxTSetUBMaxTSetLB) },
 {	HcratioUBHcratioLB,  	OtRW,	OTS8,	OTS8,	0,	0,	0, 	OTD("OTC heat curve ratio upper & lower bounds for adjustment") OTDN(HcratioUBHcratioLB) },
 {	TdhwSet,				OtRW,	F88,	OTNONE,	0,	0,	0, 	OTD("DHW setpoint (°C) (Remote parameter 1)") OTDN(TdhwSet) }, // 56
 {	MaxTSet,				OtRW,	F88,	OTNONE,	0,	0,	0, 	OTD("Max CH water setpoint (°C) (Remote parameters 2)") OTDN(MaxTSet) },
 {	Hcratio,				OtRW,	F88,	OTNONE,	0,	0,	0, 	OTD("f8.8  OTC heat curve ratio (°C) (Remote parameter 3)") OTDN(Hcratio) },

 {	StatusVH,				OtRW,	FLAG8,	FLAG8,	1,	0,	0,	OTD("Ventilation/HeatRecovery Master and Slave Status flags") OTDN(StatusVH) }, 

 { 	FHBsizeVHR,				OtR,	OTU8,	OTU8,	0,	0,	0,	OTD("Size of Fault-History-Buffer supported by ventilation / heat-recovery") OTDN(FHBsizeVHR) }, //90
 { 	FHBindexFHBvalueVHR,	OtR,	OTU8,	OTU8,	0,	0,	0,	OTD("Index number / Value of referred-to fault-history buffer entry ventilation / heat-recovery") OTDN(FHBsizeVHR) }, //91

 { 	Brand,					OtR,	OTU8,	OTU8,	0,	0,	0,	OTD(" Index number of the character in the text string ASCII character referenced by the above index number") OTDN(Brand) }, //93
 { 	BrandVersion,			OtR,	OTU8,	OTU8,	0,	0,	0,	OTD(" Index number of the character in the text string ASCII character referenced by the above index number") OTDN(BrandVersion) }, //94
 { 	BrandSerialNumber,		OtR,	OTU8,	OTU8,	0,	0,	0,	OTD(" Index number of the character in the text string ASCII character referenced by the above index number") OTDN(BrandSerialNumber) }, //95
 {	CoolingOperationHours,	OtR,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of hours that the slave is in Cooling Mode. Reset by zero is optional for slave") OTDN(CoolingOperationHours) }, // 96
 {	PowerCycles,			OtR,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of Power Cycles of a slave (wake-up after Reset), Reset by zero is optional for slave") OTDN(PowerCycles) }, // 97

 {	RFsensorStatusInformation,
							OtR,	OTSP, 	OTSP, 	0,	0,	0,	OTD("For a specific RF sensor the RF strength and battery level is written") OTDN(RFsensorStatusInformation) }, //98
 {	RemoteOverrideOperatingModeHeatingDHW,
							OtR,	OTSP, 	OTSP, 	0,	0,	0,	OTD("Operating Mode HC1, HC2/ Operating Mode DHW ") OTDN(RemoteOverrideOperatingModeHeatingDHW) }, //99
 {	RemoteOverrideFunction,	OtR,	FLAG8,	OTNONE,	0,	0,	0, OTD("Function of manual and program changes in master and remote room setpoint") OTDN(RemoteOverrideFunction) }, // = 100


 {	StatusSolarStorage,		OtRW,	FLAG8,	FLAG8,	1,	0,	0,	OTD("Master and Slave Status flags Solar Storage ") OTDN(StatusSolarStorage) }, //101

 {	ASFflagsOEMfaultCodeSolarStorage,	OtR,	FLAG8,	OTU8,	0,	0,	0,	OTD("Application-specific fault flags and OEM fault code Solar Storage") OTDN(ASFflagsOEMfaultCodeSolarStorage) }, //102

 {	CumulativElectricityProduction,
							OtR,	OTU16,	OTNONE,	0,	0,	0, OTD("Cumulativ Electricity production") OTDN(CumulativElectricityProduction) }, // = 112

 {	UnsuccessfulBurnerStarts,
							OtR,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of un - successful burner starts") OTDN(UnsuccessfulBurnerStarts) }, // = 113
 {	FlameSignalTooLowNumber,
							OtR,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of times flame signal was too low") OTDN(FlameSignalTooLowNumber) }, // = 114
 {	OEMDiagnosticCode,		OtR,	OTU16,	OTNONE,	0,	0,	0, OTD("OEM-specific diagnostic/service code") OTDN(OEMDiagnosticCode) }, // = 115
 {	SuccessfulBurnerStarts,	OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of starts burner") OTDN(SuccessfulBurnerStarts) }, //116
 {	CHPumpStarts,			OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of starts CH pump") OTDN(CHPumpStarts) }, //117
 {	DHWPumpValveStarts, 	OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of starts DHW pump/valve") OTDN(DHWPumpValveStarts) }, //118
 {	DHWBurnerStarts,		OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of starts burner during DHW mode") OTDN(DHWBurnerStarts) },//119
 {	BurnerOperationHours,	OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of hours that burner is in operation (i.e. flame on)") OTDN(BurnerOperationHours) }, //120
 
 {	CHPumpOperationHours, 	OtRW,	OTU16,	OTNONE,	0,	0,	0, OTD("Number of hours that CH pump has been running") OTDN(CHPumpOperationHours) },
 {	DHWPumpValveOperationHours,
						 	OtRW,	OTU16,	OTNONE,	0,	0,	0,	OTD("Number of hours that DHW pump has been running or DHW valve has been opened") OTDN(DHWPumpValveOperationHours) },
 {	DHWBurnerOperationHours, 
						 	OtRW,	OTU16,	OTNONE,	0,	0,	0,	OTD("Number of hours that burner is in operation during DHW mode") OTDN(DHWBurnerOperationHours) },
 {	OpenThermVersionMaster,	OtW,	F88,	OTNONE,	0,	0,	0,	OTD("The implemented version of the OpenTherm Protocol Specification in the master") OTDN(OpenThermVersionMaster) },
 {	OpenThermVersionSlave,	OtR,	F88,	OTNONE,	0,	0,	0,	OTD("The implemented version of the OpenTherm Protocol Specification in the slave") OTDN(OpenThermVersionSlave) },
 {	MasterVersion,			OtW, 	OTU8,	OTU8,	0,	0,	0,	OTD("Master product version number and type") OTDN(MasterVersion) },
 {	SlaveVersion,			OtR, 	OTU8,	OTU8,	0,	0,	0,	OTD("Slave product version number and type") OTDN(SlaveVersion) }, //127
 {	OT_ZontXZ_128,			OtR, 	OTU8,	OTU8,	0,	0,	0,	OTD("ZontXZ_128") OTDN(OT_ZontXZ_128) }, //128
 {	OT_ZontXZ_129,			OtR, 	OTU8,	OTU8,	0,	0,	0,	OTD("ZontXZ_129") OTDN(OT_ZontXZ_129) }, //129
 {	OT_RemehadFdUcodes,		OtR, 	OTU8,	OTU8,	0,	0,	0,	OTD("Remeha dF-/dU-codes") OTDN(OT_RemehadFdUcodes) }, //131
 {	OT_RemehaServicemessage,OtR, 	OTU8,	OTU8,	0,	0,	0,	OTD("Remeha Servicemessage") OTDN(OT_RemehaServicemessage) }, //132
 {	OT_RemehaDetectionConnectedSCU,
							OtR, 	OTU8,	OTU8,	0,	0,	0,	OTD("Remeha Servicemessage") OTDN(OT_RemehaDetectionConnectedSCU) }, //133
 {	OT_ZontXZ_136,			OtR, 	OTU8,	OTU8,	0,	0,	0,	OTD("ZontXZ_129") OTDN(OT_ZontXZ_136) } //136

};



class OpenTherm
{
public:
	int id_to_index[256];

	OpenTherm(bool isSlave = false)
	{	int i, j;
		for(i=0; i<256; i++) id_to_index[i] = 0;
		for(i=0; i<N_OT_NIDS; i++)
		{	j = OT_ids[i].id;
			id_to_index[j] = i;
		}
	}
	volatile OpenThermStatus status;
	unsigned long Lastresponse;
	void begin(void(*handleInterruptCallback)(void));
	void begin(void(*handleInterruptCallback)(void), void(*processResponseCallback)(unsigned long, OpenThermResponseStatus));
	
	void init_array(void);
	int update_id_array(int id, int sts);

	bool isReady();
	unsigned long sendRequest(unsigned long request);
	bool sendResponse(unsigned long request);
	bool sendRequestAync(unsigned long request);
	unsigned long buildRequest(OpenThermMessageType type, OpenThermMessageID id, unsigned int data);
	unsigned long buildResponse(OpenThermMessageType type, OpenThermMessageID id, unsigned int data);
	unsigned long getLastResponse();
	OpenThermResponseStatus getLastResponseStatus();
	const char *statusToString(OpenThermResponseStatus status);
	void handleInterrupt();
	void process();
	void end();

	bool parity(unsigned long frame);
	OpenThermMessageType getMessageType(unsigned long message);
	OpenThermMessageID getDataID(unsigned long frame);
	const char *messageTypeToString(OpenThermMessageType message_type);
	bool isValidRequest(unsigned long request);
	bool isValidResponse(unsigned long response);

	//requests
	unsigned long buildSetBoilerStatusRequest(bool enableCentralHeating, bool enableHotWater = false, bool enableCooling = false, bool enableOutsideTemperatureCompensation = false, bool enableCentralHeating2 = false);
	unsigned long buildSetBoilerTemperatureRequest(float temperature);
	unsigned long buildGetBoilerTemperatureRequest();

	//responses
	bool isFault(unsigned long response);
	bool isCentralHeatingActive(unsigned long response);
	bool isHotWaterActive(unsigned long response);
	bool isFlameOn(unsigned long response);
	bool isCoolingActive(unsigned long response);
	bool isDiagnostic(unsigned long response);
	uint16_t getUInt(const unsigned long response) const;
	float getFloat(const unsigned long response) const;	
	unsigned int temperatureToData(float temperature);

	//basic requests
	unsigned long setBoilerStatus(bool enableCentralHeating, bool enableHotWater = false, bool enableCooling = false, bool enableOutsideTemperatureCompensation = false, bool enableCentralHeating2 = false);
	bool setBoilerTemperature(float temperature);
	float getBoilerTemperature();
    float getReturnTemperature();
    bool setDHWSetpoint(float temperature);
    float getDHWTemperature();
    float getModulation();
    float getPressure();
    unsigned char getFault();
/******************************************/
	void verboseStatus(char *str, uint16_t u88);
	void verboseSConfigSMemberIDcode(char *str, uint16_t u88);

/******************************************/

private:
	bool isSlave;

	volatile unsigned long response;
	volatile OpenThermResponseStatus responseStatus;
	volatile unsigned long responseTimestamp;
	volatile byte responseBitIndex;

	int readState();
	void setActiveState();
	void setIdleState();
	void activateBoiler();

	void sendBit(bool high);
	void(*handleInterruptCallback)();
	void(*processResponseCallback)(unsigned long, OpenThermResponseStatus);
};


#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif

#ifndef IRAM_ATTR
#define IRAM_ATTR ICACHE_RAM_ATTR
#endif

#endif // OpenTherm_h
