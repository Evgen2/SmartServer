/* Connection.hpp */
//класс Connection 
//UTF-8
#ifndef CONNECTION_DEF
  #define CONNECTION_DEF

#ifdef _WIN32
#define  _CRT_SECURE_NO_WARNINGS 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
 #ifndef _AFXDLL
  #define _AFXDLL
  #include <afxsock.h>		// MFC socket extensions
  #include <WinSock.h>
  #include <process.h>    /* _beginthread, _endthread */
  #define socklen_t int
 #endif 
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

#define SOCKET_ERROR (-1)
#define closesocket close

inline int Sleep(int ms)
{   usleep(1000*ms);
    return 0;
}

#endif

#ifndef MSG1MSG2
#define MSG1MSG2

#include "Smart_commands.h"

struct Msg1
{   short int cmd0; //команда
    short int cmd; //команда
    short int ind; //параметр
    unsigned char Buf[120];
};

struct Msg2
{   short int cmd0; //команда
    short int cmd; //команда
    short int ind; //параметр
    unsigned char Buf[1500];
};

#endif // MSG1MSG2

class Connection1
{
public:
   int type;
   struct sockaddr_in server;  //адрес  сервера
   struct sockaddr_in client;  //адрес  клиента
   int timeout;                //таймаут на соединение в мс
   int timeoutAnswer;          //таймаут на повторение посылки в мс
   int ierr;  // код ошибки, если есть

   wchar_t IdName[128]; //Название устройства
   int IdCode; // id устройства
   int IdNum;  // номер устройства

   Connection1(void) //конструктор по умолчанию
   {  type = 0;
      ierr = 0;
      IdName[0] = 0; //Название устройства
      IdCode = 0; // id устройства
      IdNum = 0;  // номер устройства
   }
   ~Connection1(void) //деструктор
   { //if(sock > 0)
     //       soclose(sock);
   }
/*
//послать буфер с подтверждением
   int SendAndConfirm(char buf[], int len, char bufout[], int lenout);
//послать буфер с подтверждением с переменной длиной ответа
   int SendAndConfirm2(char buf[], int len, char bufout[], int lenout, int shiftLansw, int size0, int FirstByte);
//запрос информации
   int GetInfo(void);
//читать
   int Read(char bufin[], int len);
//читать
   int Read(char bufin[], int len, int timeout);
//послать
   int Send(char buf[], int len);
*/

};

#endif
//CONNECTION_DEF
