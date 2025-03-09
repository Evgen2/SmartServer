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
  #include <io.h>
  #include <fcntl.h> 

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

//cmd0 0xfe - запрос сервера, 0x22 - автоинформирование контроллера, 0x12 - переменная длина ответа - примерно

struct Msg1
{   short int cmd0; //команда
    short int cmd; //команда
    short int ind; //параметр
    unsigned char Buf[128];
};

struct Msg2
{   short int cmd0; //команда
    short int cmd; //команда
    short int ind; //параметр
    unsigned char Buf[1500];
};

#endif // MSG1MSG2


#endif
//CONNECTION_DEF
