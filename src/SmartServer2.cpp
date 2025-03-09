/* SmartServer2.cpp */

#include "stdafx.h"
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/stat.h>

#include "SmartServer.hpp"


#ifdef _WIN32
	void TCP_ClientThread2( void * lpParam );
#else
	void *TCP_ClientThread2( void * lpParam );
#endif

int num_therads = 0;
int num_therads_rt = 0;

void SmartServer::Server(void)
{	int rc, ierr, raz, id_client,  client_sock;
    socklen_t addr_len;
    struct sockaddr_in client;  //lockal client
	SmartClient *psc;

	clock_t t0;

	tcpserver.port = tcp_port;
	tcpserver.timeout = timeout;
	tcpserver.timeoutAnswer = timeoutAnswer;

	rc = tcpserver.createTCPserverconnection(verbose);
	if(rc)
	{	printf("createTCPserverconnection rc %d\n", rc);
		sts = 0x10;
		Sleep(100);
		tcpserver.closeConnection();
		printf("Server died\n");
		sts = 0x20;
		return;
	}
	t0 = GetClock();
	rc = listen(tcpserver.sock0,5);
    if (rc == -1)
    { 	sts = 0x10;
		Sleep(1000);
		sts = -1;
		Sleep(100);
		tcpserver.closeConnection();
		sts = 0x20;
		return;
	}
	sts = 1;
	printf("Server work\n");
/************************************/
	for(;;)
	{   addr_len = sizeof (struct sockaddr_in);
		client_sock = accept(tcpserver.sock0, (struct sockaddr *) &client, &addr_len);
        if (client_sock == -1)
		{	sts = 0x10;
#ifdef _WIN32
          ierr = WSAGetLastError();
          if(WSAEWOULDBLOCK == ierr)
		  {	 Sleep(1);
			continue;
		  }

		  printf("accept(), error = %d\n", WSAGetLastError() );
#else
 		  if(errno == EAGAIN)
		  {	 Sleep(1);
			continue;
		  }

		  printf(" error = %d %s\n", errno, strerror(errno) );
#endif
			Sleep(1000);
			continue;
		}
		
/* nonblocking mode */
#ifdef _WIN32
    const unsigned int nonblocking = TRUE;
    rc = ioctlsocket(client_sock, FIONBIO, (u_long *)&nonblocking);
    if (rc < 0)
    {  if(verbose)
			printf("Error call ioctl, error = %d\n", WSAGetLastError() );
       closesocket(client_sock);
		   client_sock = -1;
       return;
    }
#else
    rc = fcntl(client_sock, F_SETFL, O_NONBLOCK);
    if(rc == -1)
    {   perror("fcntl");
       closesocket(client_sock);
       return;
    }
#endif // WIN32


//		printf("Client from %s\n", inet_ntoa(client.sin_addr));

		id_client = GetFreeClient();
		if(id_client < 0)
		{	printf("(2) closesocket\n");
			closesocket(client_sock);
			Sleep(1000);
			continue;
		}
		psc = &sclient[id_client];
//		printf("Free Client id %d idclient %d\n",id_client, psc->idclient );
		psc->Init();
		psc->tcpClient.sock = client_sock;
		psc->tcpClient.client = client;
		strcpy(psc->ipaddr, inet_ntoa(client.sin_addr));
		sclient[id_client].sts_cl = 0;
#ifdef _WIN32
		uintptr_t hid;
		hid = _beginthread(TCP_ClientThread2,0,(void *) psc);
#else
		{	int status;
			pthread_t thread;
			pthread_attr_t attr;
			pthread_attr_init(&attr);
    			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			status = pthread_create(&thread, &attr, TCP_ClientThread2, (void *)  psc);
			if(status != 0)
			{	perror("pthread_create() error");
				exit(1);
			}
		}
#endif
		num_therads++;
		Sleep(10);
		raz = 0;
		while (sclient[id_client].sts_cl == 0)
		{  Sleep(20);
			if(raz > 1)
				printf("wait Client id %d sts %d (%d)\n", id_client,  sclient[id_client].sts_cl, raz);
			raz++;
		}
	printf("thr %d/%d Nsmt %d Nsma %d start Client id %d sts %d\r", num_therads,num_therads_rt, Nsmt, Nsma, id_client,  sclient[id_client].sts_cl);
	fflush(stdout);
		Sleep(10);
		if(need_save_smt)
			Write_Smt();
	} /* for(;;) */
/************************************/
}


#ifdef _WIN32
void TCP_ClientThread2( void * lpParam )
#else
void *TCP_ClientThread2( void * lpParam )
#endif
{	SmartClient *psc;
	int rc, lsend=0;
//    socklen_t addr_len;

	 if(lpParam == NULL)
	 {	printf("Error: lpParam = NULL in %s\n", __FUNCTION__);
	    exit(1);
	 }
	num_therads_rt++;

	psc =(SmartClient *)lpParam;
	psc->Log(8, "Thread Client start sts %x IP %s\n",  psc->sts_cl, psc->ipaddr);
	Sleep(10);
	psc->sts_cl = 1; //
	psc->Log(8, "Thread Client ready sts %x\n",  psc->sts_cl);

//work todo
	do
	{	rc = psc->loop();
	} while(rc == 0);
	
//	printf("Thread Client ending sts %x rc= %d\n", psc->sts_cl, rc); fflush(stdout);

	psc->sts_cl = 0x10; //
	psc->Log(8, "Thread Client ending sts %x rc= %d\n", psc->sts_cl, rc); fflush(stdout);
	if(psc->tcpClient.sock > 0)
	{	closesocket(psc->tcpClient.sock);
		psc->tcpClient.sock = -1;
	}
	Sleep(100);
	psc->sts_cl = 0x20; //
	psc->Log(8, "Thread Client End sts %x\n",  psc->sts_cl);
	num_therads_rt--;

#ifdef _WIN32

#else
   return NULL;
#endif
}


int SmartServer::GetFreeClient(void)
{	int rc = -1, i, isconn=0, isStop=-1;
//проверяем состояние ниток клиентов
	for(i=0; i<NclientsThreads; i++)
	{	if(sclient[i].sts_cl == 0x20)
			sclient[i].sts_cl = -1;
		if(sclient[i].sts_cl == -1)
		{	if( i == NclientsThreads-1)
				NclientsThreads--;
			else
				isStop = i;
			break;
		}
	}
	if(isStop == -1 && NclientsThreads < MAX_CLIENTS)
	{//	StartClientThread(NclientsThreads);
		rc = NclientsThreads;
		NclientsThreads++;
	} else {
		rc = isStop;
//		StartClientThread(isStop);
	}
	sclient[rc].sts_cl = 0x0;
	return rc;
}

