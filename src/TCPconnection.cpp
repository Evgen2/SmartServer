/* TCPconnection.cpp */
//UTF-8
#include "stdafx.h"
#include "TCPconnection.hpp"

int ShowMyIp(void);
int InitTCPIP(void);
void CloseTCPIP(void);

int TCPconnection::InitClientConnection(int verboze)
{   int l, rc;
   struct  Msg1 ucmd;
   struct  Msg1 outcmd;

	l=sizeof(HAND_SHAKE_INP);
    ucmd.cmd = MCMD_HAND_SHAKE;
    ucmd.cmd0 =  0xfe;
    indcmd = (indcmd+1)&0xffff;
    ucmd.ind =  indcmd;
	memcpy(ucmd.Buf, HAND_SHAKE_INP,l);
	l += sizeof(short int)*3; //16

    rc = SendAndConfirm((char *)&ucmd, l, (char *)&outcmd, l, verboze);

    if(rc == 0)
    {  int isbad1 = 0, isbad2=0;
       static int razbad=0;

	   if(!strcmp((char *)outcmd.Buf,HAND_SHAKE_OUT))
       {
/*
           if(online == 0)
                need_init = 1;
          online = 1;
          t_last_work = GetClock();
*/
		 printf("Connection to %s OK\n", IpTo);
/*          */
    ucmd.cmd = MCMD_ECHO;
    ucmd.cmd0 =  0xfe;
    indcmd = (indcmd+1)&0xffff;
    ucmd.ind =  indcmd;
	memcpy(ucmd.Buf, HAND_SHAKE_INP,l);
	l += sizeof(short int)*3+1; //17
    rc = SendAndConfirm((char *)&ucmd, l, (char *)&outcmd, l,  verboze);
    if(rc == 0)
	{    return 0;
    }
/*         */
        } else
               rc = 0x10;
    } else if(rc == 1) {
//        online = 0;
 	   printf("Connection to %s failed\n", IpTo);
    }
    return -1;
}

int TCPconnection::createTCPserverconnection(int verboze)
{	return createTCPserverconnection(port, timeout, timeoutAnswer, verboze);
}

//создать сервернон соединение с ip адресом _IpToб портом _port, таймаутами _timeout, _timeoutAnswer
int TCPconnection::createTCPserverconnection(int _port, int _timeout, int _timeoutAnswer, int verboze)
{   int rc;
    socklen_t n;
    struct sockaddr_in client2;  //локальный порт на клиенте

    memset(&client,0,sizeof(sockaddr_in));
    client.sin_family = AF_INET;//PF_UNIX;
    client.sin_port = INADDR_ANY;

    memset(&server,0,sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(_port);
    server.sin_addr.s_addr =   INADDR_ANY; //inet_addr(_IpTo); //преобразовывает и записывает IP-адрес
    sock0 = socket( AF_INET, SOCK_STREAM,  0 /* IPPROTO_UDP */ ); //создаем сокет
    if(sock0 == -1)
    {
#ifdef _WIN32
        printf("socket function failed with error = %d\n", WSAGetLastError() );
#else
        perror("socket failed");
#endif // _WIN32
       printf("Ошибка создания сокета\n");
       return(1);
    }
    
	printf("sock0 %x\n", sock0);


	n = sizeof(sockaddr_in);
    rc = getsockname(sock0, (struct sockaddr *)&client2, &n);
	if(verboze)
		printf("Server port: %i (%x)\n", _port, _port);
#ifdef _WIN32
	{	int iResult; 
		int iOptVal = 0;
		int iOptLen = sizeof (int);
		BOOL bOptVal = FALSE;
		int bOptLen = sizeof (BOOL);
		bOptVal = TRUE;
#if 1		
		iResult = getsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (char *) &iOptVal, &iOptLen);
		if (iResult == SOCKET_ERROR) {
			printf("getsockopt for SO_REUSEADDR failed with error: %u\n", WSAGetLastError());
		} else {
//			printf("SO_REUSEADDR Value: %ld\n", iOptVal);
		}
		iOptVal = 1;
//		iResult = setsockopt(sock0, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &iOptVal, sizeof (iOptVal));
		iResult = setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (char *) &iOptVal, sizeof(iOptVal));
		if (iResult == SOCKET_ERROR) {
	        printf("setsockopt for SO_REUSEADDR failed with error: %u\n", WSAGetLastError());
	    } else {
//			printf("Set SO_REUSEADDR: ON\n");
		}

		iResult = getsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (char *) &iOptVal, &iOptLen);
		if (iResult == SOCKET_ERROR) {
			printf("getsockopt for SO_REUSEADDR failed with error: %u\n", WSAGetLastError());
		} else {
//			printf("SO_REUSEADDR Value: %ld\n", iOptVal);
		}
#endif
#if 0
//SO_EXCLUSIVEADDRUSE
		iResult = getsockopt(sock0, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &iOptVal, &iOptLen);
		if (iResult == SOCKET_ERROR) {
			printf("getsockopt for SO_EXCLUSIVEADDRUSE failed with error: %u\n", WSAGetLastError());
		} else {
//			printf("SO_EXCLUSIVEADDRUSE Value: %ld\n", iOptVal);
		}

	    iOptVal = 1;
		iResult = setsockopt(sock0, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &iOptVal, sizeof (iOptVal));
//		iOptVal = 1;
//		iResult = setsockopt(sock0, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &iOptVal, sizeof(iOptVal));
		if (iResult == SOCKET_ERROR) {
	        printf("setsockopt for SO_EXCLUSIVEADDRUSE failed with error: %u\n", WSAGetLastError());
	    } else {
//			printf("Set SO_EXCLUSIVEADDRUSE: ON\n");
		}

		iResult = getsockopt(sock0, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &iOptVal, &iOptLen);
		if (iResult == SOCKET_ERROR) {
			printf("getsockopt for SO_EXCLUSIVEADDRUSE failed with error: %u\n", WSAGetLastError());
		} else {
//			printf("SO_EXCLUSIVEADDRUSE Value: %ld\n", iOptVal);
		}

#ifdef SO_REUSEPORT
		iResult = getsockopt(sock0, SOL_SOCKET, SO_REUSEPORT, (char *) &iOptVal, &iOptLen);
		if (iResult == SOCKET_ERROR) {
		        printf("setsockopt for SO_REUSEADDR failed with error: %u\n", WSAGetLastError());
//		else
//			printf("SO_REUSEPORT: %ld\n", iOptVal);

		iResult = setsockopt(sock0, SOL_SOCKET, SO_REUSEPORT, (char *) &bOptVal, bOptLen);
		if (iResult == SOCKET_ERROR) {
	        printf("setsockopt for SO_REUSEPORT failed with error: %u\n", WSAGetLastError());
	    } else {
//			printf("Set SO_REUSEPORT: ON\n");
		}

		iResult = getsockopt(sock0, SOL_SOCKET, SO_REUSEPORT, (char *) &iOptVal, &iOptLen);
		if (iResult == SOCKET_ERROR) {
		        printf("setsockopt for SO_REUSEADDR failed with error: %u\n", WSAGetLastError());
//		else
//			printf("SO_REUSEPORT: %ld\n", iOptVal);
#endif
#endif 

	}
#else
	{	int iResult; 
		int iOptVal = 0;
		socklen_t iOptLen = sizeof (int);
		int reuse = 1;
		int bOptLen = sizeof (int);
		
		iResult = getsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (char *) &iOptVal, &iOptLen);
		if (iResult == SOCKET_ERROR) {
			 perror("getsockopt for SO_REUSEADDR failed with error");
		} else {
			printf("SO_REUSEADDR Value: %d\n", iOptVal);
		}
		iResult = setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, bOptLen);
		if (iResult == SOCKET_ERROR) {
	        	 perror("setsockopt for SO_REUSEADDR failed with error:");
		} else {
//			printf("Set SO_REUSEADDR: ON\n");
		}
		iResult = getsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (char *) &iOptVal, &iOptLen);
		if (iResult == SOCKET_ERROR) {
			 perror("getsockopt for SO_REUSEADDR failed with error");
//		} else {
//			printf("SO_REUSEADDR Value: %d\n", iOptVal);
		}

#ifdef SO_REUSEPORT
    	if (setsockopt(sock0, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
       			perror("setsockopt(SO_REUSEPORT) failed");
//		else
//			printf("Set SO_REUSEPORT: ON\n");
		iResult = getsockopt(sock0, SOL_SOCKET, SO_REUSEPORT, (char *) &iOptVal, &iOptLen);
		if (iResult == SOCKET_ERROR) {
			 perror("getsockopt for SO_REUSEPORT failed with error:");
		} else {
//			printf("SO_REUSEPORT Value: %d\n", iOptVal);
		}
	}
#endif
#endif // WIN32

/* nonblocking mode */
#ifdef _WIN32
    const unsigned int nonblocking = TRUE;
    rc = ioctlsocket(sock0, FIONBIO, (u_long *)&nonblocking);
    if (rc < 0)
    {  if(verboze)
			printf("Ошибка вызова ioctl error = %d\n", WSAGetLastError() );
       closesocket(sock0);
		   sock0 = -1;
       return(3);
    }
#else
    rc = fcntl(sock0, F_SETFL, O_NONBLOCK);
    if(rc == -1)
    {   perror("fcntl");
       closesocket(sock0);
		   sock0 = -1;
       return(3);
    }
#endif // WIN32

    rc = bind(sock0, (struct sockaddr *)&server, sizeof(sockaddr_in));
    if(rc == -1)
    {
#ifdef _WIN32
		if(verboze)
			printf("Ошибка вызова bind error = %d\n", WSAGetLastError() );
#else
        perror("bind");
#endif //_WIN32
       closesocket(sock0);
       return(2);
    }

    port = _port;
    timeout = _timeout;
    timeoutAnswer = _timeoutAnswer;
	return 0;
}

int TCPconnection::closeConnection(void)
{ 
#ifdef _WIN32
#endif 
	if(sock > 0)
    { 	printf("closeConnection sock %x\n", sock);
		closesocket(sock); 
    }
   sock = -1;
   if(sock0 > 0)
   { 	printf("closeConnection sock0 %x\n", sock0);
       closesocket(sock0);
   }
   sock0 = -1;
   return 0;
}

int TCPconnection::TCPconnect(int verboze)
{ int rc;
    timeval Timeout_us;
    Timeout_us.tv_sec = timeout/1000;
    Timeout_us.tv_usec = (timeout - timeout/1000*1000)*1000 ;

    sock = socket( AF_INET, SOCK_STREAM,  IPPROTO_TCP /* IPPROTO_UDP */ ); //создаем сокет
    if(sock == -1)
    {
#ifdef _WIN32
      if(verboze)
        printf("socket function failed with error = %d\n", WSAGetLastError() );
#else
        perror("socket failed");
#endif // _WIN32
       return(1);
    }
	    
/* nonblocking mode */
#ifdef _WIN32
    const unsigned int nonblocking = TRUE;
    rc = ioctlsocket(sock, FIONBIO, (u_long *)&nonblocking);
    if (rc < 0)
    {  if(verboze)
			printf("Error call ioctl error = %d\n", WSAGetLastError() );
       closesocket(sock);
		   sock = -1;
       return(3);
    }
#else
    rc = fcntl(sock, F_SETFL, O_NONBLOCK);
    if(rc == -1)
    {   perror("fcntl");
       closesocket(sock);
       return(3);
    }
#endif // WIN32

  if(verboze)
	printf("Try connect to %s port %i\n", IpTo, port);
   rc = connect(sock, (struct sockaddr *) &server, sizeof (struct sockaddr_in));
  if (rc==(-1))
  {  
#ifdef _WIN32
	  int errcode;
	  errcode = WSAGetLastError();
	  if(errcode != WSAEWOULDBLOCK)
	  {
		if(verboze)
		{ if(errcode ==  WSAETIMEDOUT)
			printf("connect  to %s port %i timeout\n", IpTo,  port );
		else
			printf("Connect to %s port %i failed with error = %d\n", IpTo, port, errcode);
		}
		if(verboze)
			printf("Cann't establish TCP connection\r\n");
		closesocket(sock);
		   sock = -1;
		return 2;
     }
#else
	 perror("connect");
	 printf("(Try connect to %s port %i)\n", IpTo, port);
	 if(verboze)
       printf("Cann't establish TCP connection\r\n");
       closesocket(sock);
		   sock = -1;
       return 2;
#endif
   };

  /*******************************/
 {
   fd_set Write, Err;
    FD_ZERO(&Write);
    FD_ZERO(&Err);
    FD_SET(sock, &Write);
    FD_SET(sock, &Err);
 
    // check if the socket is ready
    rc = select(0,NULL,&Write,&Err,&Timeout_us);			
  if (rc==(-1))
  {  
#ifdef _WIN32
	  int errcode;
	  errcode = WSAGetLastError();
	if(verboze)
	{ if(errcode ==  WSAETIMEDOUT)
        printf("connect  to %s port %i timeout\n", IpTo, port );
	  else
	    printf("Connect to %s port %i failed with error = %d\n", IpTo, port, errcode);
	}
#else
	     perror("connect");
	    printf("(Try connect to %s port %i)\n", IpTo, port);
#endif
	if(verboze)
       printf("Cann't establish TCP connection\r\n");
       closesocket(sock);
		   sock = -1;
       return 2;
   };

    if(!(FD_ISSET(sock, &Write)) ) 
    {	
		if(verboze)
		   printf("Cann't establish TCP connection\r\n");
       closesocket(sock);
		   sock = -1;
       return 2;
    }
 
 }
  /*******************************/

  if(verboze)
  {	 printf("TCP connection established\r\n");
     printf("Controller addr: %s  port:%x\n", IpTo, port);
  }

	return 0;
}

//создать соединение с ip адресом _IpToб портом _port, таймаутами _timeout, _timeoutAnswer
//rc = 2 = Cann't establish TCP connection
//rc = 0 Ok
int TCPconnection::createTCPconnection(char _IpTo[], int _port, int _timeout, int _timeoutAnswer, int verboze)
{   int rc;
    socklen_t n;
    timeval Timeout_us;
    Timeout_us.tv_sec = _timeout/1000;
    Timeout_us.tv_usec = (_timeout - _timeout/1000*1000)*1000 ;

#ifdef _WIN32
    const unsigned int nonblocking = TRUE;
#endif
    struct sockaddr_in client2;  //локальный порт на клиенте

    memset(&client,0,sizeof(sockaddr_in));
    client.sin_family = AF_INET;//PF_UNIX;
    client.sin_port = INADDR_ANY;

    memset(&server,0,sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(_port);
    server.sin_addr.s_addr = inet_addr(_IpTo); //преобразовывает и записывает IP-адрес
    sock = socket( AF_INET, SOCK_STREAM,  IPPROTO_TCP /* IPPROTO_UDP */ ); //создаем сокет
    if(sock == -1)
    {
#ifdef _WIN32
      if(verboze)
        printf("socket function failed with error = %d\n", WSAGetLastError() );
        perror("socket failed");
#else
        perror("socket failed");
#endif // _WIN32
       return(1);
    }
	    
/* nonblocking mode */
#ifdef _WIN32
    rc = ioctlsocket(sock, FIONBIO, (u_long *)&nonblocking);
    if (rc < 0)
    { 
	  if(verboze)
		printf("Ошибка вызова ioctl error = %d\n", WSAGetLastError() );
       closesocket(sock);
		   sock = -1;
       return(3);
    }
#else
    rc = fcntl(sock, F_SETFL, O_NONBLOCK);
    if(rc == -1)
    {   perror("fcntl");
       closesocket(sock);
       return(3);
    }
#endif // WIN32


  if(verboze)
	printf("Try connect to %s port %i\n", _IpTo, _port);
   rc = connect(sock, (struct sockaddr *) &server, sizeof (struct sockaddr_in));
  if (rc==(-1))
  {  
#ifdef _WIN32
	  int errcode;
	  errcode = WSAGetLastError();
	  if(errcode != WSAEWOULDBLOCK) //WSAEWOULDBLOCK= A non-blocking socket operation could not be completed immediately.
	  {
	if(verboze)
	{ if(errcode ==  WSAETIMEDOUT)
        printf("connect  to %s port %i timeout\n",_IpTo, _port );
	  else
	    printf("Connect to %s port %i failed with error = %d\n", _IpTo, _port, errcode);
        perror("connect() failed");
	    printf("Errno=%d\n", errno);
	}
		if(verboze)
			printf("Cann't establish TCP connection\r\n");
		closesocket(sock);
		   sock = -1;
		return 2;
	  }
#else
	perror("connect");
	printf("(Try connect to %s port %i\n", _IpTo, _port);
	if(verboze)
		printf("Cann't establish TCP connection\r\n");
	closesocket(sock);
	   sock = -1;
	return 2;
#endif
   };

  /*******************************/
 {
   fd_set Write, Err;
    FD_ZERO(&Write);
    FD_ZERO(&Err);
    FD_SET(sock, &Write);
    FD_SET(sock, &Err);
 
    // check if the socket is ready
    rc = select(0,NULL,&Write,&Err,&Timeout_us);			
  if (rc==(-1))
  {  
#ifdef _WIN32
	  int errcode;
	  errcode = WSAGetLastError();
	if(verboze)
	{ if(errcode ==  WSAETIMEDOUT)
        printf("connect  to %s port %i timeout\n",_IpTo, _port );
	  else
	    printf("Connect to %s port %i failed with error = %d\n", _IpTo, _port, errcode);
	}
#else
	     perror("connect ()");
	     printf("(Try connect to %s port %i)\n", _IpTo, _port);
#endif
	if(verboze)
       printf("Cann't establish TCP connection\r\n");
       closesocket(sock);
		   sock = -1;
       return 2;
   };

    if(!(FD_ISSET(sock, &Write)) ) 
    {	
		if(verboze)
		   printf("Cann't establish TCP connection\r\n");
       closesocket(sock);
		   sock = -1;
       return 2;
    }
 
 }
  /*******************************/

  if(verboze)
		printf("TCP connection established\r\n");

    n = sizeof(sockaddr_in);
    rc = getsockname(sock, (struct sockaddr *)&client2, &n);
  if(verboze)
    printf("Controller addr: %s  port:%x\n", _IpTo, _port);

    port = _port;
    strcpy(IpTo, _IpTo);
    timeout = _timeout;
    timeoutAnswer = _timeoutAnswer;

	return 0;
}

int TCPconnection::reTCPconnect(void)
{  int rc;
   closeConnection();
   rc = TCPconnect(1);
#if 0
   rc = connect(sock, (struct sockaddr *) &server, sizeof (struct sockaddr_in));
  if (rc==(-1))
  {  
#ifdef _WIN32
	  int errcode;
	  errcode = WSAGetLastError();
	  if(errcode == WSAEISCONN)
	  {
        printf("already connected\n" );
		return 0;
	  }

	  if(errcode ==  WSAETIMEDOUT)
//        printf("connect  to %s port %i timeout\n",_IpTo, _port );
        printf("connect  timeout\n" );
	  else
	    printf("reConnect failed with error = %d\n",  errcode);
//	    printf("Connect to %s port %i failed with error = %d\n", _IpTo, _port, errcode);
#else
	     _wperror("connect");
	    printf("(Try connect to %s port %i)\n", _IpTo, _port);
#endif
       printf("Cann't establish TCP connection\r\n");
       closesocket(sock);
		   sock = -1;
       return 2;
   };
printf("TCP connection established !!!\r\n");
  return 0;
#endif 
  return rc;
}

int ShowMyIp(void)
{
    char ac[80];
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR)
    {
#ifdef _WIN32
		printf("Error when getting local host name= %d\n", WSAGetLastError() );
#else
		perror("Error when getting local host name\n" );
#endif
        return 1;
    }
    printf("Host name is %s\n", ac);

    struct hostent *phe = gethostbyname(ac);
    if (phe == 0)
    {
#ifdef _WIN32
		printf("Yow! Bad host lookup. error= %d\n", WSAGetLastError() );
#else
		herror("Yow! Bad host lookup.");
#endif
        return 1;
    }

    for (int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        printf("My address  %i : %s\n", i, inet_ntoa(addr));
    }

    return 0;
}

//int razSend1 = 0;

//послать буфер с подтверждением
//rc = 0 Ok
//rc = 1 Timeout
//rc = 2 прочие ошибки
//rc = 3 плохой сокет
//rc = 4 длина входного буфера неправильная
//rc = 5 получено не то количество байт, которое должно быть
//rc = 6 ответ: неизвестная команда
//rc = -1 CONNABORTED
int TCPconnection::SendAndConfirm(char bufin[], int len, char bufout[], int lenout, int verboze)
{  int i, rc, nb=0, nRep=0, rc0=0, nRepRoute=0, isTimeout, isTimeout2;
   int razSend = 0, raz;
   unsigned char buff_out[1500];
   int tout0Answer, tout1Answer;
   int tout0, tout1;
   int start = 0;
static int razSendold=0,tclockOld=0;

   if(sock <= 0)
        return 3;

   if(len > 1400 || len < 0)
        return 4;
//если что-то есть на - входе - прочитать
M0:
    rc = recv(sock, (char *)&buff_out,sizeof(buff_out),0);
    if(rc == SOCKET_ERROR)
    {
#ifdef _WIN32
          ierr = WSAGetLastError();
	      if(WSAEWOULDBLOCK != ierr)
           {
			   if(WSAECONNABORTED == ierr)
				   return -1;
			   else if(WSAECONNRESET == ierr)
			   {   rc = reTCPconnect();
			       if(rc)
					   return -2;
			   }
			   if(rc)
			   {  if(verboze)
						printf("recv error= %d\n", ierr );
                 rc0 = ierr;
                 stat[3]++;
				 return -3;
			   }
           }
#else
        perror("recv");
#endif // _WIN32          ierr = WSAGetLastError();
           stat[4]++;
    } else {
          nb = rc;
          if(rc > 0)
          { nresv ++;
            nBresv += nb;
            stat[5]++;
          }
	}

   if(razSendold > 1)
   {   if(nb == 0)
       {  if(GetClock()-tclockOld < 100)
          {
              Sleep(1);
              tclockOld = GetClock()-200;
              goto M0;
          }
       }
   }

   razSend = 0;
   isTimeout2 = 1;
      nb = 0;
   tout0 = GetClock();
   do
   {
      ierr = 0;
      razSend++;
	  if(razSend > 1)
	  {	    printf(".");
	  }
      rc = sendto(sock,bufin,len,0,(struct sockaddr *)&server,sizeof(sockaddr_in));
      if(rc == SOCKET_ERROR)
      {
#ifdef _WIN32
          ierr = WSAGetLastError();
           printf("sendto error= %d\n", ierr );
#else
         perror("sendto");
#endif // _WIN32
         rc0 = ierr;
         stat[1]++;
         break;
      } else {
         nBsend += rc;
         if(start)
         {  nResend++;
            nRep++;
         }
         start = 1;
         stat[0]++;
      }
     tout0Answer = GetClock();
      isTimeout = 1;

      for(raz=0; ;raz++)
      {
          ierr = 0;
{ int sockList[2], timeout_ms=5000, num=0;
  fd_set fds;
  struct timeval timeout_us;

   FD_ZERO(&fds);
   FD_SET(sock, &fds);
  sockList[0] = sock;
  timeout_us.tv_sec = timeoutAnswer/1000;
  timeout_ms = (timeoutAnswer - timeoutAnswer/1000*1000)*1000;
  if(lenout > 60)
  {  timeout_ms = 8000;
     if(lenout > 120)
         timeout_ms = 16000;
  }
  timeout_us.tv_usec = timeout_ms;
  num = select(sizeof(fds)*8, &fds, NULL, NULL, &timeout_us);
      if(num == SOCKET_ERROR)
      {
#ifdef _WIN32
          ierr = WSAGetLastError();
           printf("select( error= %d\n", ierr );
#else
           perror("select");
#endif // _WIN32
	  }
  rc = -2;
  if(num)
          rc = recv(sock, (char *)&buff_out,sizeof(buff_out),0);
}
          if(rc < 0)
          {   if(rc == SOCKET_ERROR)
               {
#ifdef _WIN32
                    ierr = WSAGetLastError();
                    printf("recv error= %d\n", ierr );
#else
                    perror("recv");
#endif // _WIN32

					break;
			   }
                 tout1Answer = GetClock();
                 if(abs(tout1Answer - tout0Answer) < timeoutAnswer)
                 {  //  if(raz > 256*64)  //26Kbytes
                                if(raz & 0x01)
                                           Sleep(1);
                 } else
                      break;

             stat[4]++;
          } else {
            if(rc > 0)
            {  nb += rc;
               nresv ++;
               nBresv += rc;
               stat[2]++;
               isTimeout = 0;
			   break;
             } else
                 break;
          }
      } //endof for(raz=0; ;raz++)

      if(isTimeout)
      {  stat[6]++;
	     if(nb == 0)
			   break;
      }

      if(ierr)
      {   //?? if(SOCEAGAIN != ierr)
           {  isTimeout2 = 0;
              break;
           }
      } else {
           if(nb != lenout)
           {	struct Msg1 *pm1, *pm2;
				pm1 = (struct Msg1 *) bufin;
		        pm2 = (struct Msg1 *) buff_out;
				if(nb >= 6)
				{	if(pm2->cmd0 &0x8000)
					{	i = nb;
						if(i > lenout)
							i = lenout;
						memcpy(bufout,buff_out,i);
						if(verboze) 
						{ printf("\a\nCMD %d (0x%x) unknown to controller\n", pm2->cmd, pm2->cmd);
						}
						return 6;
					}
				}
			   
			   if(nb != lenout) 
               {  printf("\a\nGet: %i bytes  != %i, cmdR=%i S=%i rS=%i<\n",
                    nb, lenout, pm2->cmd, pm1->cmd, razSendold);
                 stat[5]++;
                 rc0 = 5;
                 isTimeout2 = 0;
                 for(i=0; i < lenout; i++)  printf("%2x ",bufin[i]);
                 printf("\n>");
                  for(i=0; i < nb; i++) printf("%2x ",buff_out[i]);
                 printf("\n");
                 return 5;
             }
                break;
           } else {
                isTimeout2 = 0;
               break;
           }

      }

      tout1 = GetClock();
   } while(abs(tout1 - tout0) < timeout);

   if( isTimeout2) stat[7]++;
   if(razSend > 2) stat[8]++;

   memcpy(bufout,buff_out,lenout);
   if(rc0 == 0)
   { // if(ierr == SOCEAGAIN)
	   if(isTimeout2)
	   {   rc0 = 1;
//          printf("таймаут после %i попыток\n",nRep);
      }
   }
//   printf("%i %i %i %i raz=%i dt=%i\n", lenout, nb, stat[7],stat[8], raz, clock()-tout0);
   razSendold = razSend;
   if(rc0 == 0 && nb == 0)
	     printf("hren\n");
  tclockOld = GetClock();
   return rc0;
}


//послать буфер с подтверждением с переменной длиной ответа
//FirstByte - первый байт в данных ответа должен равняться FirstByte
//size0 - размер в байтах одного элемента ответа
//shiftLansw - смещение в байтах от начала ответа длины ответа в элементах (short int)
//             la = (int) *((short int *) &buff_out[shiftLansw]); - длина ответа в элеменатх
//             la * size0 + shiftLansw + 2; - общая длина ответа
// пример:	rc =  SendAndConfirm2((char *)&ucmd, 6, (char *)&outcmd, sizeof(outcmd), 6, 1,0x12);
//смещение 6, элементы по 1 байту, первый байт должен быть 0x12
//rc = 0 Ok
//rc = 1 Timeout
//rc = 2 прочие ошибки
//rc = 3 плохой сокет
//rc = 4 длина входного буфера неправильная
//rc = 5 длина выходного буфера не равна длине полученного сообщения
int TCPconnection::SendAndConfirm2(char bufin[], int len, char bufout[], int lenout, int shiftLansw, int size0, int FirstByte)
{  int i, rc, nb=0, nRep=0, rc0=0, nRepRoute=0, isTimeout, isTimeout2;
   int razSend = 0, raz;
   unsigned char buff_out[1500];
   int tout0Answer, tout1Answer,la=0;
   int tout0, tout1, num;
   int start = 0;
   if(sock <= 0)
        return 3;

   if(len > 1400 || len < 0)
        return 4;
//если что-то есть на - входе - прочитать
    rc = recv(sock, (char *)&buff_out,sizeof(buff_out),0);
    if(rc == SOCKET_ERROR)
    {
#ifdef _WIN32
          ierr = WSAGetLastError();
          if(WSAEWOULDBLOCK != ierr)
           {
                 printf("recv error= %d\n", ierr );
                 rc0 = ierr;
                 stat[3]++;
           }
#else
          perror("recv");
#endif // _WIN32

           stat[4]++;
    } else {
          if(rc > 0)
          {
            nb = rc;
            nresv ++;
            nBresv += nb;
            stat[5]++;
          }
   }

//   ncmd = (ncmd + 2) & 0xff;
 //   razSend1 = razSend;
   razSend = 0;
   isTimeout2 = 1;
   tout0 = GetClock();
      nb = 0;
   do
   {
//    buff_in[0] = ncmd;
//      memcpy(&buff_in[1],bufin, len);
//      l = len+1;
      ierr = 0;
      razSend++;
      rc = sendto(sock,bufin,len,0,(struct sockaddr *)&server,sizeof(sockaddr_in));
      if(rc == SOCKET_ERROR)
      {

#ifdef _WIN32
          ierr = WSAGetLastError();
           printf("sendto error= %d\n", ierr );
#else
         perror("sendto");
#endif // _WIN32

         rc0 = ierr;
         stat[1]++;
         break;
      } else {
         nBsend += rc;
         if(start)
         {  nResend++;
            nRep++;
         }
         start = 1;
         stat[0]++;
      }
      tout0Answer = GetClock();
      isTimeout = 1;
/*******************************/
      for(raz=0; ;raz++)
      {
          ierr = 0;
{ int  timeout_ms=5000, num=0;
  fd_set fds;
  struct timeval timeout_us;

   FD_ZERO(&fds);
   FD_SET(sock, &fds);
  timeout_us.tv_sec = 0;
  if(lenout > 60)
  {  timeout_ms = 8000;
     if(lenout > 120)
         timeout_ms = 16000;
  }
  timeout_us.tv_usec = timeout_ms;
  num = select(sizeof(fds)*8, &fds, NULL, NULL, &timeout_us);

      if(num == SOCKET_ERROR)
      {
#ifdef _WIN32
          ierr = WSAGetLastError();
           printf("select( error= %d\n", ierr );
#else
           perror("select");
#endif // _WIN32
	  }
  rc = -2;
  if(num)
          rc = recv(sock, (char *)&buff_out,sizeof(buff_out),0);

}

          if(rc < 0)
          {   if(rc == SOCKET_ERROR)
               {
#ifdef _WIN32
          ierr = WSAGetLastError();
          printf("recv error= %d\n", ierr );
#else
          perror("recv");
#endif // _WIN32

					break;
			   }
                 tout1Answer = GetClock();
                 if(abs(tout1Answer - tout0Answer) < timeoutAnswer)
                 {  //  if(raz > 256*64)  //26Kbytes
                                if(raz & 0x01)
                                           Sleep(1);
                 } else
                      break;

             stat[4]++;
          } else {
            if(rc > 0)
            {  nresv ++;
               nBresv += rc;
               stat[2]++;
               isTimeout = 0;
                if(nb == 0)
                {  if(FirstByte != buff_out[0]) /* чужой ответ ? */
                   {   isTimeout = 1;
                       continue;
                   }

                    la = (int) *((short int *) &buff_out[shiftLansw]);
                    la = la * size0 + shiftLansw + 2;
                    if(la < 0 || la > 1500) la = 0;
                    if(la >lenout)
                            la = lenout;
                }
                nb += rc;
                if(nb >= la)
                   break;

			   break;
             } else
                 break;
          }
          Sleep(1);
      } //endof for(raz=0; ;raz++)

/*******************************/

      if(isTimeout)
      {  stat[6]++;
	     if(nb == 0)
			   break;
      }

      if(ierr)
      {   // if(SOCEAGAIN != ierr)
           {  isTimeout2 = 0;
              break;
           }
      } else {
           if(nb != la)
           {
//               la = (int) *((short int *) &buff_out[shiftLansw]);
//               la = la * size0 + shiftLansw + 2;

//               if(la != nb)
//               {
                  printf("\a\n2Get: %i байт != %i\n",nb, la);
                 stat[5]++;
                 rc0 = 5;
                 isTimeout2 = 0;
                 for(i=0; i < len; i++)  printf("%2x ",bufin[i]&0xff);
                 printf("\n>");
                  for(i=0; i < nb; i++) printf("%2x ",buff_out[i]&0xff);
                 printf("\n");
                 return 5;
//             }
                isTimeout2 = 0;
                break;
//todo!!!
//           if(buff[0] != ncmd)
           } else {
//                for(i=0; i < rc; i++) printf("%2x ",buff_out[i]);
///                printf("\n");
                isTimeout2 = 0;
               break;
           }

      }
      tout1 = GetClock();
printf("raz=%i rc=%i num=%i razSend=%i nb=%i dt=%i %i %i\n", raz, rc, num, razSend, nb,
     tout1 - tout0Answer, tout1-tout0,  timeoutAnswer);

   } while(abs(tout1 - tout0) < timeout);

   if( isTimeout2) stat[7]++;
   if(razSend > 2) stat[8]++;

   if(nb > 0)
   {  if(nb > lenout) nb = lenout;
           memcpy(bufout,buff_out,nb);
   }
   if(rc0 == 0)
   { // if(ierr == SOCEAGAIN)
	  if(isTimeout2)
      {   rc0 = 1;
//          printf("таймаут после %i попыток\n",nRep);
      }
   }

   if(razSend>1)
      printf("!!2!razSend=%i\n",razSend);
  if(isTimeout2)
      printf("!!2!isTimeout2 nb=%i\n", nb);

   return rc0;
}

int TCPconnection::Read(char bufin[], int len)
{  int rc;
   rc = Read(bufin, len, 5000);
   return rc;
}

//return 
//rc >= 0  -number of bytes read from socket
//rc = -2 select   socker error
//rc = -3 recvfrom socker error
//rc = -4 recvfrom error
//
int TCPconnection::Read(char bufin[], int len, int timeout_ms)
{  int rc = 0, i;
   int sockList[2],  num=0;
   fd_set fds;
  struct timeval timeout_us;
   unsigned char buff_out[1500];
   socklen_t addr_len;

   FD_ZERO(&fds);
   FD_SET(sock, &fds);
   sockList[0] = sock;
   timeout_us.tv_sec = 0;
   timeout_us.tv_usec = timeout_ms*1000;
   num = select(FD_SETSIZE, &fds, NULL, NULL, &timeout_us);
   if(num == SOCKET_ERROR)
   {
#ifdef _WIN32
       ierr = WSAGetLastError();
       printf("select( error= %d\n", ierr );
#else
       perror("select");
#endif // _WIN32
       return -2;
   }
   if(num == 0)
	   return 0;

   addr_len = sizeof(struct sockaddr);

  //  rc = recv(sock, (char *)&buff_out,sizeof(buff_out),0);
    rc = recvfrom(sock, (char *)&buff_out, sizeof(buff_out), 0, (struct sockaddr *)&client, &addr_len);
    if(rc < 0)
    {   if(rc == SOCKET_ERROR)
         {
#ifdef _WIN32
              ierr = WSAGetLastError();
			  if(ierr != WSAECONNABORTED)
				printf("select( error= %d\n", ierr );
#else
             perror("recvfrom");
#endif //_WIN32
			  return -3;
	     }
//         printf("recv rc= %d\n", rc);
		 return -4;
	} else if(rc == 0) {
		;
/*         printf("select rc %d recvfrom rc %d\n", num, rc);

		{  char str[20];
inet_ntop(AF_INET, &(client.sin_addr), str, INET_ADDRSTRLEN);

printf("CLIENT  %s\n",  str);
		}
*/
	}  else {
//         printf("1select rc %d recvfrom rc %d\n", num, rc);
		 timeLastRW = time(NULL);
		if(rc < len)
			len = rc;
		for(i=0; i< len; i++)
		{   bufin[i] = buff_out[i];
		}

	}

	return rc;
}


int TCPconnection::Send(char buf[], int len)
{  int rc;
    rc = sendto(sock,buf,len,0,(struct sockaddr *)&client,sizeof(sockaddr_in));
    if(rc == SOCKET_ERROR)
    {
#ifdef _WIN32
         ierr = WSAGetLastError();
         printf("sendto error= %d\n", ierr );
#else
         perror("sendto");
#endif // _WIN32
         return -1;
    }

	if(rc > 0)
		timeLastRW = time(NULL);

	return rc;
}

int InitTCPIP(void)
{
#ifdef _WIN32
    WSADATA wsaData = {0};
	int rc;

  // Initialize Winsock
    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc != 0)
	{   printf("WSAStartup failed: %d\n", rc);
        return 1;
    }
#else
	;
#endif
	ShowMyIp();
    GetClockInit();
	return 0;
}

void CloseTCPIP(void)
{  int rc;
#ifdef _WIN32
	printf("call WSACleanup\n");
	rc = WSACleanup();
	printf("WSACleanup rc=%d\n", rc);
	if (rc == SOCKET_ERROR)
			printf("Closesocket error %d\n", WSAGetLastError() );
#endif //_WIN32
}


#ifdef _WIN32

static clock_t d_time;
static clock_t d_time_start;
//typedef long clock_t;

clock_t GetClockInit(void)
{ d_time_start = clock();
  return d_time_start; 
}

clock_t GetClock(void)
{  d_time = clock();
   return  d_time -  d_time_start; 
}

#else

static double d_time;
static double d_time_start;

clock_t GetClockInit(void)
{ struct timespec time;
   clock_gettime(CLOCK_MONOTONIC,&time);
   d_time_start = ((double)time.tv_sec + 1.0e-9 * time.tv_nsec);
   return (clock_t)0;
}

clock_t GetClock(void)
{   struct timespec time;
     clock_gettime(CLOCK_MONOTONIC,&time);
     d_time = ((double)time.tv_sec + 1.0e-9 * time.tv_nsec);
	 return (clock_t)((d_time - d_time_start) * 1000);
}

#endif
/*******************************************************************/

/*
    Function calculate checksum
*/
unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
    register long sum;
    u_short oddbyte;
    register u_short answer;
 
    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
 
    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *) & oddbyte) = *(u_char *) ptr;
        sum += oddbyte;
    }
 
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16); //??
    answer =(u_short)( ~sum);
 
    return (answer);
}

