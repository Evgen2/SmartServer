// SmartUdp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "UDPconnection.hpp"
#include "SmartServer.hpp"
#include "SmartHosts.hpp"

char * SmartServer::Appname = (char *)APPLICATION_NAME;
char * SmartServer::Version = (char *)VERSION;
char * SmartServer::SubVersion = (char *)SUBVERSION;

SmartServer server;   
class my_hosts myhosts;

char ConfigFile[]="SmartServer.cfg";

	int ReadCFG(void);
	int WriteCFG(void);

int main(int npar, char* argv[])
{	int rc;
	int testmode = 0;
	
#ifdef _WIN32
rc = SetConsoleCP(1251);
rc = SetConsoleOutputCP(1251);
#endif
    if(npar > 1)
    { if( !_stricmp(argv[1],"help") || (*argv[1] == '-' && (*(argv[1]+1) == '?')||(*(argv[1]+1) == 'h') ) )
      {  printf("Usage: SmartServer [test]\n");   
         exit(0);
      }
	  if( !_stricmp(argv[1],"test") )
		  testmode = 1;
	}

    server.init(ConfigFile);
	if(testmode) server.mode = 1;

	if(server.mode & 0x01)
	{	rc = server.StartClient(testmode);
		if(rc == 0 && server.mode & 0x02)
			rc = server.Start();
	} else if(server.mode & 0x02) {
		rc = server.Start();
	}

	return 0;
}

