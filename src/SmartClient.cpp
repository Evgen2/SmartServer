/* SmartClient.cpp */
#include "stdafx.h"
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include <math.h>

#include "UDPconnection.hpp"
#include "SmartServer.hpp"
#include "SmartHosts.hpp"

extern int InitWinsok(void);

extern class my_hosts myhosts;

int SmartServer::StartClient(int testmode)
{	int rc;
	time_t tdev, thost;
	double dt; 	


	{	rc = InitWinsok();
		if(rc)
			return 1;
	}

	if(type == 0x02)
	{	rc = pc_client.createConnection(1, 2500, 2300,1);
	} else {
		rc = pc_client.createConnection(2, 2500, 2300,1);
	}
	if(rc)
		return 1;

    printf("Wait client...\n");
	do
	{	rc = pc_client.InitClientConnection();
			//rc =  remoteudp_server.InitClientConnection();
		if(rc == 0)
			break;
		printf("."); fflush(stdout);
		Sleep(1000 +  pc_client.timeout);
	} while(1);

	rc = pc_client.GetTime(&tdev);
	if(rc)
	{	rc = pc_client.GetTime(&tdev);
		if(rc)
		{    printf("ip %s, error  GetTime %i\n", pc_client.ipaddr, rc);
			pc_client.online = 0;
			return 2;
		}
	}
	thost = time(NULL);
	pc_client.tlastc = thost;
	pc_client.online = 1;
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
		rc = pc_client.SetTime(&tstruct);
		rc = pc_client.GetTime(&tdev);
		printf("Device time: %s\n",ctime(&tdev));
	}
	rc  = pc_client.GetData();
	if(rc == 0)
	{
		printf("MAC %02x:%02x:%02x:%02x:%02x:%02x\n",pc_client.Mac[0],pc_client.Mac[1],pc_client.Mac[2],pc_client.Mac[3],pc_client.Mac[4],pc_client.Mac[5]);
		printf("BoilerStatus = %x\n", pc_client.BoilerStatus);
		printf("BoilerT = %.2f\n", pc_client.BoilerT);
		printf("RetT = %.2f\n", pc_client.RetT);
		printf("dhw_t = %.2f\n", pc_client.dhw_t);
		printf("FlameModulation = %.2f\n", pc_client.FlameModulation);
		printf("Pressure = %.2f\n", pc_client.Pressure);
		printf("ExtTstatus = %x\n", pc_client.ExtTstatus);
		printf("ExtT1 = %.2f\n", pc_client.ExtT1);
		printf("ExtT2 = %.2f\n", pc_client.ExtT2);
	} else {
		if(rc == 1)
				printf("Timeout\n");
		else
				printf("Error code = %d\n", rc);
	}

	if(testmode)
	{	rc = TestOpenTherm();
		pc_client.closeConnection();
		exit(0);
	}
	if(type & 0x02)
		pc_client.SetUDPServerInfo(2, udp_port, 30000);
	else if(type & 0x04)
		pc_client.SetTCPServerInfo(2, Server_ipaddr, tcp_port, 30000);
	pc_client.closeConnection();

#if 0
	} else if (type == 0x04) {
		rc = pc_client.tcp.createTCPconnection(pc_client.ipaddr, pc_client.port, 2500, 2300,1);
		if(rc == 0)
		{   rc = pc_client.tcp.InitClientConnection();
			if(rc == 0)
			{	pc_client.Ctype = 2; //tcp

				printf("IdName = %s\n", pc_client.tcp.IdName);
				printf("IdCode = %x\n", pc_client.tcp.IdCode);
				printf("IdNum  = %x\n", pc_client.tcp.IdNum);
				return 0;
			}
			pc_client.tcp.closeConnection();
			return -1;
		} 
	}
#endif


 return 0;
}

