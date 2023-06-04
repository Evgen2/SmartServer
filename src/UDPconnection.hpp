/* UDPconnection.hpp */
//UTF-8
//класс UDPconnection
#ifndef UDPCONNECTION_DEF
#define UDPCONNECTION_DEF

#include "Connection.hpp"

#ifndef MSG1MSG2
#define MSG1MSG2

#define HAND_SHAKE_INP           "TCPiptEsT"
#define HAND_SHAKE_OUT           "ipTCPTeSt"
#define CMD_HAND_SHAKE  0x2020
#define CMD_ECHO        1
#define CMD_IDENTIFY	0x80

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

#endif //MSG1MSG2
class UDPconnection
{
public:
   int sock;
   struct sockaddr_in server;  //адрес удаленного сервера
   struct sockaddr_in client;  //локальный порт на клиенте
   int port;                   //номер порта на сервере
   char IpTo[16];              //ip адрес удаленного сервера в char
   int timeout;                //таймаут на соединение в мс
   int timeoutAnswer;          //таймаут на повторение посылки в мс
   int timeLastRW;

   int nsend; // послано, раз
   int nresv; // получено, paз
   int nBsend; // байтов послано
   int nBresv; // байтов получено
   int ierr;  // код ошибки, если есть
   int ncmd;  // номер посланной команды 0-255
   short int indcmd;  //
   int nResend; // сколько раз делали повторную посылку
   int stat[16]; //статистика
   wchar_t IdName[128]; //Название устройства
   int IdType; // id типа устройства
   int IdCode; // id устройства
   int IdNum;  // номер устройства

   UDPconnection(void) //конструктор по умолчанию
   {  sock = 0;
//      memset(&server,0,sizeof(sockaddr_in));
//      memset(&client,0,sizeof(sockaddr_in));
      memset(&stat,0,sizeof(stat));
      port = 0;
      nsend = nresv = 0;
      nBsend = nBresv = 0;
      timeout = 1000;
      timeoutAnswer = 10;
	  timeLastRW = 0;
      IpTo[0] = 0;
      ierr = 0;
      ncmd = 0;
      indcmd = 0x1a;
      nResend = 0;
      IdName[0] = 0; //Название устройства
      IdCode = 0; // id устройства
      IdNum = 0;  // номер устройства
	  IdType = 0;
   }
   ~UDPconnection(void) //деструктор
   { //if(sock > 0)
     //       soclose(sock);
   }
//создать соединение
   int createUDPconnection(char _IpTo[], int port, int timeout, int timeoutAnswer, int verboze);
   int createUDPserverconnection(int port, int timeout, int timeoutAnswer);
//закрыть соединение
   int closeDPconnection(void)
   { //if(sock > 0)
     //       soclose(sock);
     sock = -1;
     return 0;
   }
	int closeConnection(void)
	{	if(sock > 0)
			closesocket(sock);
		sock = -1;
		return 0;
	}

	int AddRoute(void);

//послать буфер с подтверждением
   int SendAndConfirm(char bufin[], int len, char bufout[], int lenout);
   int SendAndConfirm(char bufin[], int len, char bufout[], int lenout, int verbose);
//послать буфер с подтверждением с переменной длиной ответа
   int SendAndConfirm2(char bufin[], int len, char bufout[], int lenout, int shiftLansw, int size0, int FirstByte, int verbose);
//запрос информации
   int GetInfo(void);
   int InitClientConnection(void);
   //читать
   int Read(char bufin[], int len);
//читать
   int Read(char bufin[], int len, int timeout);
//послать
   int Send(char buf[], int len);

};

#endif //UDPCONNECTION_DEF
