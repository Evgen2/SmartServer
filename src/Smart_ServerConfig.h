﻿/* Smart_ServerConfig.h */
#ifndef SMART_CONFIG
#define SMART_CONFIG

#define APPLICATION_NAME1 L"SmartTherm server"
#define APPLICATION_NAME2  "SmartTherm server"
#define APPLICATION_NAME "ST server"
#define DEBUG_LEVEL 6

//#define VERSION "0.00.00"
//#define SUBVERSION "1"
#define CONFIG_VERSION 1

#define SERVER_CODE   	   25
#define SERVER_VERSION     0
#define SERVER_SUBVERSION  0
#define SERVER_SUBVERSION1 1

#define IDENTIFY_TYPE	DS_SMARTSERVER
#define IDENTIFY_TEXT   APPLICATION_NAME
#define IDENTIFY_CODE	0x10000

#define MAX_DEVICES 1024
#define MAX_APPLICATIONS 1024
#define MAX_CLIENTS (MAX_DEVICES + MAX_APPLICATIONS)



#endif //SMART_CONFIG

