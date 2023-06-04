/* UDPconnection.cpp */
//UTF-8
#include "stdafx.h"
#include "UDPconnection.hpp"

int ShowMyIp(void);

void perror2(const char *msg)
{	
#ifdef _WIN32
	 printf("%s, errorcode = %d\n", msg, WSAGetLastError() );
#else
	perror(msg);
	printf("Errno=%d\n", errno);
#endif
}

//создать серверное соединение на порту _port, таймаутами _timeout, _timeoutAnswer
int UDPconnection::createUDPserverconnection(int _port, int _timeout, int _timeoutAnswer)
{   int rc;
#ifdef _WIN32
	int n;
#else
	unsigned int n;
#endif
    const unsigned int nonblocking = TRUE;
    struct sockaddr_in client2;  //локальный порт на клиенте

    memset(&client,0,sizeof(sockaddr_in));
    client.sin_family = AF_INET;//PF_UNIX;
    client.sin_port = INADDR_ANY;

    memset(&server,0,sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(_port);
    server.sin_addr.s_addr =   INADDR_ANY; //inet_addr(_IpTo); //преобразовывает и записывает IP-адрес
    sock = socket( AF_INET, SOCK_DGRAM,  0 /* IPPROTO_UDP */ ); //создаем сокет
    if(sock == -1)
    { // printf("socket function failed with error = %d\n", WSAGetLastError() );
       perror2("socket() failed");
       return(1);
    }

    rc = bind(sock, (struct sockaddr *)&server, sizeof(sockaddr_in));
    if(rc == -1)
    {  perror2("bind() failed");
       closesocket(sock);
       return(2);
    }

    n = sizeof(sockaddr_in);
    rc = getsockname(sock, (struct sockaddr *)&client2, &n);
    printf("Server port: %x\n",  _port);
/* */
/* nonblocking mode */
#ifdef _WIN32

    rc = ioctlsocket(sock, FIONBIO, (u_long *)&nonblocking);
    if (rc < 0)
    {  perror2("Ошибка вызова ioct");
       closesocket(sock);
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

    port = _port;
    timeout = _timeout;
    timeoutAnswer = _timeoutAnswer;
 
	return 0; 
}


//создать соединение с ip адресом _IpToб портом _port, таймаутами _timeout, _timeoutAnswer
int UDPconnection::createUDPconnection(char _IpTo[], int _port, int _timeout, int _timeoutAnswer, int verboze)
{   int rc;
#ifdef _WIN32
	int n;
#else
	unsigned int n;
#endif
    const unsigned int nonblocking = TRUE;
    struct sockaddr_in client2;  //локальный порт на клиенте

    memset(&client,0,sizeof(sockaddr_in));
    client.sin_family = AF_INET;//PF_UNIX;
    client.sin_port = INADDR_ANY;

    memset(&server,0,sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(_port);
    server.sin_addr.s_addr = inet_addr(_IpTo); //преобразовывает и записывает IP-адрес
    sock = socket( AF_INET, SOCK_DGRAM,  0 /* IPPROTO_UDP */ ); //создаем сокет
    if(sock == -1)
    {  perror2("socket function failed");
       return(1);
    }

    rc = bind(sock, (struct sockaddr *)&client, sizeof(sockaddr_in));
    if(rc == -1)
    { if(verboze)
		perror2("Ошибка вызова bind");
       closesocket(sock);
       return(2);
    }

    n = sizeof(sockaddr_in);
    rc = getsockname(sock, (struct sockaddr *)&client2, &n);
	if(verboze)
		printf("Controller addr: %s  port:%x\n", _IpTo, _port);
/* */
/* nonblocking mode */
#ifdef _WIN32
    rc = ioctlsocket(sock, FIONBIO, (u_long *)&nonblocking);
    if (rc < 0)
    {  if(verboze)  perror2("Ошибка вызова ioctl" );
       closesocket(sock);
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

    port = _port;
    strcpy(IpTo, _IpTo);
    timeout = _timeout;
    timeoutAnswer = _timeoutAnswer;

 
	return 0; 
}

int UDPconnection::InitClientConnection(void)
{   int i, l;
    int rc;
   struct  Msg1 ucmd;
   struct  Msg1 outcmd;

	l=sizeof(HAND_SHAKE_INP);
    ucmd.cmd = MCMD_HAND_SHAKE;
    ucmd.cmd0 =  0xfe;
    indcmd = (indcmd+1)&0xffff;
    ucmd.ind =  indcmd;
	memcpy(ucmd.Buf, HAND_SHAKE_INP,l);
	l += sizeof(short int)*3+1; //17

    rc = SendAndConfirm((char *)&ucmd, l, (char *)&outcmd, l,0);

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
//		 printf("Connection to %s OK\n", IpTo);
/*          */
    ucmd.cmd = MCMD_ECHO;
    ucmd.cmd0 =  0xfe;
    indcmd = (indcmd+1)&0xffff;
    ucmd.ind =  indcmd;
	memcpy(ucmd.Buf, HAND_SHAKE_INP,l);
	l += sizeof(short int)*3+1; //17
    rc = SendAndConfirm((char *)&ucmd, l, (char *)&outcmd, l, 1);
    if(rc == 0)
	{ // printf("Echo test Ok\n");
		ucmd.cmd = MCMD_IDENTIFY;
		ucmd.cmd0 = 0x12;
		rc =  SendAndConfirm2((char *)&ucmd, 6, (char *)&outcmd, sizeof(outcmd), 6, 1,0x12, 1);
		if(rc == 0)
		{  char str[512];
		printf("\n");
		   l = *((short int *)&outcmd.Buf[0]);
		   IdType = *((int *)&outcmd.Buf[2]);
		   IdCode = *((int *)&outcmd.Buf[6]);
		   IdNum  = *((int *)&outcmd.Buf[10]);
		   for(i=0; i<512; i++) str[i] = 1;
           memcpy(str,(void *)&outcmd.Buf[14], l-sizeof(int)*2);
//		   printf("test=%s\n",test);
#ifdef _WIN32
		rc = MultiByteToWideChar(CP_UTF8, 0,str,-1, IdName, 0);
		rc = MultiByteToWideChar(CP_UTF8, 0,str,-1, IdName, 128);
		   printf("IdTypt=%x IdCode=%x IdNum=%i\nIdName=%s \n",IdType,IdCode, IdNum, IdName);
#else
		   printf("IdTypt=%x IdCode=%x IdNum=%i\nIdName=%s \n",IdType,IdCode, IdNum, str); //todo
#endif
		   return 0;
		}
	    return 0x11;
    }
/*         */
        } else
               rc = 0x10;
    } else if(rc == 1) {
//        online = 0;
// 	   printf("Connection to %s failed\n", IpTo);
    }
    return -1;
}

//int razSend1 = 0;


//послать буфер с подтверждением
//rc = 0 Ok
//rc = 1 Timeout
//rc = 2 Timeout2
//rc = 3 плохой сокет
//rc = 4 длина входного буфера неправильная
//rc = 5 получено не то количество байт, которое должно быть
//rc = 6 ответ: неизвестная команда
int UDPconnection::SendAndConfirm(char bufin[], int len, char bufout[], int lenout)
{  return SendAndConfirm(bufin, len, bufout, lenout, 1);
}
int UDPconnection::SendAndConfirm(char bufin[], int len, char bufout[], int lenout, int verbose)
{  int i, rc, nb=0, nRep=0, rc0=0, nRepRoute=0, isTimeout, isTimeout2;
   int razSend = 0, raz;
   unsigned char buff_out[1500];
   int t0_Answer, t1_Answer;
   int tout0, tout1;
   int start = 0;
static int razSendold=0,tclockOld=0;
   unsigned int tk_0, tk_1, tk_2, tk_3;

   if(sock <= 0)
        return 3;

   if(len > 1400 || len < 0)
        return 4;
   tk_0 = clock();
   tk_1 = tk_2 = 0;
//если что-то есть на - входе - прочитать
M0:
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
           stat[4]++;
#else
        perror2("recv");
#endif
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
       {  if(clock()-tclockOld < 100)
          {
              Sleep(1);
              tclockOld = clock()-200;
              goto M0;
          }
       }
   }

   razSend = 0;
   isTimeout2 = 1;
      nb = 0;
   tout0 = clock();
   do
   {
      ierr = 0;
      razSend++;
      rc = sendto(sock,bufin,len,0,(struct sockaddr *)&server,sizeof(sockaddr_in));
      if(rc == SOCKET_ERROR)
      { 
          perror2("sendto error" );
          
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
   tk_1 = clock();
     t0_Answer = clock();
      isTimeout = 1;

      for(raz=0; ;raz++)
      {  int num = 0;
        ierr = 0;
		{ int timeout_us=5000;
		  fd_set fds;
			struct timeval timeout;

			FD_ZERO(&fds);
			FD_SET(sock, &fds);
			timeout.tv_sec = 0;
			if(lenout > 60) 
			{  timeout_us = 8000;
				if(lenout > 120) 
					timeout_us = 16000;
			}
			timeout.tv_usec = timeout_us; //microsecond

			num = select(sizeof(fds)*8, &fds, NULL, NULL, &timeout);
			if(num == SOCKET_ERROR)
			{  
#ifdef _WIN32
				ierr = WSAGetLastError();
				printf("udp select() error= %d\n", ierr );
#else
	       perror2("select()");

#endif // _WIN32

			}
			rc = -2;
			if(num)
			{	rc = recv(sock, (char *)&buff_out,sizeof(buff_out),0);
				tk_2 = clock();
			}
		}
        
		if(rc < 0)
        {   if(rc == SOCKET_ERROR)
            {  
#ifdef _WIN32
				ierr = WSAGetLastError();
					if(ierr != WSAECONNRESET)
						printf("udp recv() error= %d\n", ierr );
				if(ierr != WSAEWOULDBLOCK)
					break;
#else
	        perror2("recv()");
#endif // _WIN32

		    }
            
			t1_Answer = clock();
            if(abs(t1_Answer - t0_Answer) < timeoutAnswer)
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
						if(verbose) 
						{ printf("\a\nКоманда %d (0x%x) неизвестна контроллеру\n", pm2->cmd, pm2->cmd);
						}
						return 6;
					}
				}
				if(verbose) 
				{ printf("\a\nПолучено: %i байт != %i, cmdR=%i S=%i rS=%i<\n",
                    nb, lenout, pm2->cmd, pm1->cmd, razSendold);
                 stat[5]++;
                 rc0 = 5;
                 isTimeout2 = 0;
                 printf(" ");
                 for(i=0; i < lenout; i++)  printf("%2x ",(bufin[i]&0xff));
                 printf("\n>");
                  for(i=0; i < nb; i++) printf("%2x ",buff_out[i]&0xff);
                 printf("\n");
               }
               return 5;
           } else {
                isTimeout2 = 0;
               break;
           }
      }

      tout1 = clock();
   } while(abs(tout1 - tout0) < timeout);

   if( isTimeout2) stat[7]++;
   if(razSend > 2) stat[8]++;

   memcpy(bufout,buff_out,lenout);
   if(rc0 == 0)
   { // if(ierr == SOCEAGAIN)
	   if(isTimeout)
	   {	rc0 = 1;
	   } else if(isTimeout2) {
		   tk_3 = clock();
		   rc0 = 2;
		   printf("dt1=%i dt2=%i dt3=%i\n", tk_1-tk_0,tk_2-tk_0,tk_3-tk_0);
//          printf("таймаут после %i попыток\n",nRep);
      }
   }
//   printf("%i %i %i %i raz=%i dt=%i\n", lenout, nb, stat[7],stat[8], raz, clock()-tout0);
   razSendold = razSend;     
   if(rc0 == 0 && nb == 0)
	     printf("hren\n");
  tclockOld = clock();
   return rc0;
}


//послать буфер bufin длиной len с подтверждением с переменной длиной ответа
// lenout - максимальный размер буфера
// shiftLansw - сдвиг в байтах от начала буфера длины данных (short int)
// size0 - размер в байтах элемента данных
// общая длина ответа в байтах рассчитывается по формуле
//  la = (int) *((short int *) &buff_out[shiftLansw]);
//  la = la * size0 + shiftLansw + 2;
// FirstByte - первый байт ответа
//rc = 0 Ok
//rc = 1 Timeout
//rc = 2 прочие ошибки
//rc = 3 плохой сокет
//rc = 4 длина входного буфера неправильная
//rc = 5 длина выходного буфера не равна длине полученного сообщения

int UDPconnection::SendAndConfirm2(char bufin[], int len, char bufout[], int lenout, int shiftLansw, int size0, int FirstByte, int verbose)
{  int i, rc, nb=0, nRep=0, rc0=0, nRepRoute=0, isTimeout, isTimeout2;
   int razSend = 0, raz;
   unsigned char buff_out[1500];
   int tout0Answer, tout1Answer, la=0;
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
           {     printf("recv error= %d\n", ierr );
                 rc0 = ierr;
                 stat[3]++;
           }
           stat[4]++;
#else
        perror2("recv()");
#endif // _WIN32
		
    } else {
          if(rc > 0)
          { nb = rc;
            nresv ++;
            nBresv += nb;
            stat[5]++;
          }
   }

//   ncmd = (ncmd + 2) & 0xff;
 //   razSend1 = razSend;
   razSend = 0;
   isTimeout2 = 1;
   tout0 = clock();
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
        perror2("sendto()");
          
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
      tout0Answer = clock();
      isTimeout = 1;
/*******************************/
      for(raz=0; ;raz++)
      { int num=0;
          ierr = 0;
{ int sockList[2], timeout_ms=5000;
  fd_set fds;
  struct timeval timeout;

   FD_ZERO(&fds);
   FD_SET(sock, &fds);
  sockList[0] = sock;
  timeout.tv_sec = 0;
  if(lenout > 60) 
  {  timeout_ms = 8000;
     if(lenout > 120) 
         timeout_ms = 16000;
  }
  timeout.tv_usec = timeout_ms;
  num = select(sizeof(fds)*8, &fds, NULL, NULL, &timeout);

      if(num == SOCKET_ERROR)
      { 
        perror2("select() error");
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
					if(ierr)  // может быть 0
					{	printf("select( error= %d\n", ierr );
						if(ierr != WSAEWOULDBLOCK)
							break;
					}
#else
        perror2("recv");
#endif // _WIN32

			   }
                 tout1Answer = clock();
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

               if(verbose)
               {
                  printf("\a\n2Получено: %i байт != %i\n",nb, la);
                 stat[5]++;
                 rc0 = 5;
                 isTimeout2 = 0;
                 for(i=0; i < len; i++)  printf("%2x ",bufin[i]);
                 printf("\n>");
                  for(i=0; i < nb; i++) printf("%2x ",buff_out[i]);
                 printf("\n");
               }
                return 5;
           } else {
//                for(i=0; i < rc; i++) printf("%2x ",buff_out[i]);
///                printf("\n");
                isTimeout2 = 0;
               break;
           }

      }
      tout1 = clock();
     if(verbose)
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

   return rc0;
}

int UDPconnection::Read(char bufin[], int len)
{  int rc;
   rc = Read(bufin, len, 5000);
   return rc;
}

int UDPconnection::Read(char bufin[], int len, int timeout_ms)
{  int rc = 0, i;
   int sockList[2],  num=0;
   fd_set fds;
  struct timeval timeout;
   unsigned char buff_out[1500];
#ifdef _WIN32
   int addr_len;
#else
   unsigned int addr_len;
#endif

   FD_ZERO(&fds);
   FD_SET(sock, &fds);
   sockList[0] = sock;
   timeout.tv_sec = 0;
   timeout.tv_usec = timeout_ms;
   num = select(sizeof(fds)*8, &fds, NULL, NULL, &timeout);
   if(num == SOCKET_ERROR)
   {	perror2("select()");
       return -2;
   }
   if(num == 0)
	   return 0;

    addr_len = sizeof(struct sockaddr);

//    rc = recv(sock, (char *)&buff_out,sizeof(buff_out),0);
    rc = recvfrom(sock, (char *)&buff_out, sizeof(buff_out), 0, (struct sockaddr *)&client, &addr_len);
    if(rc < 0)
    {   if(rc == SOCKET_ERROR)
         {  
			 perror("recvfrom error");
			  return -3;
	     }
         printf("recv rc= %d\n", rc);
		 return -4;
	}

	for(i=0; i<rc; i++)
	{   bufin[i] = buff_out[i];
	}
    return rc;
}


int UDPconnection::Send(char buf[], int len)
{  int rc;
    rc = sendto(sock,buf,len,0,(struct sockaddr *)&client,sizeof(sockaddr_in));
    if(rc == SOCKET_ERROR)
    {   perror("sendto error");
         return -1;
    }
	return rc;
}

