/* TCPconnection.hpp */
//����� TCPconnection 
//UTF-8
#ifndef TCPCONNECTION_DEF
  #define TCPCONNECTION_DEF

#include "Connection.hpp"



#ifndef MSG1MSG2
#define MSG1MSG2

#define HAND_SHAKE_INP           "TCPiptEsT"
#define HAND_SHAKE_OUT           "ipTCPTeSt"
#define CMD_HAND_SHAKE  0x2020
#define CMD_ECHO        1
#define CMD_IDENTIFY	0x80

struct Msg1
{   short int cmd0; //�������
    short int cmd; //�������
    short int ind; //��������
    unsigned char Buf[120];
};

struct Msg2
{   short int cmd0; //�������
    short int cmd; //�������
    short int ind; //��������
    unsigned char Buf[1500];
};

#endif // MSG1MSG2

class TCPconnection
{
public:
   int sock0;
   int sock;
   struct sockaddr_in server;  //�����  �������
   struct sockaddr_in client;  //�����  �������
   int port;                   //����� ����� �� �������
   char IpTo[16];              //ip ����� ���������� ������� � char
   int timeout;                //������� �� ���������� � ��
   int timeoutAnswer;          //������� �� ���������� ������� � ��
   time_t timeLastRW;
   int nsend; // �������, ���
   int nresv; // ��������, pa�
   int nBsend; // ������ �������
   int nBresv; // ������ ��������
   int ierr;  // ��� ������, ���� ����
   int ncmd;  // ����� ��������� ������� 0-255
   short int indcmd;  //
   int nResend; // ������� ��� ������ ��������� �������
   int stat[16]; //����������
   wchar_t IdName[128]; //�������� ����������
   int IdType; // id ���� ����������
   int IdCode; // id ����������
   int IdNum;  // ����� ����������
   int Vers;
   int Subvers;
   int Subvers1;
   char Mac[6];
   char BiosDate[16]; //12 ����

   TCPconnection(void) //����������� �� ���������
   {  sock = sock0 = 0;
//      memset(&server,0,sizeof(sockaddr_in));
//      memset(&client,0,sizeof(sockaddr_in));
      memset(&stat,0,sizeof(stat));
      port = 0;
      nsend = nresv = 0;
      nBsend = nBresv = 0;
      timeout = 20000;
      timeoutAnswer = 2000;
	  timeLastRW = 0;
      IpTo[0] = 0;
      ierr = 0;
      ncmd = 0;
      indcmd = 0x1a;
      nResend = 0;
      IdName[0] = 0; //�������� ����������
      IdCode = 0; // id ����������
      IdNum = 0;  // ����� ����������
	  IdType = 0;
   }
   ~TCPconnection(void) //����������
   { //if(sock > 0)
     //       soclose(sock);
   }
//������� ����������
   int createTCPconnection(char _IpTo[], int port, int timeout, int timeoutAnswer, int verboze);

   int TCPconnect(int verboze);
   int reTCPconnect(void);
   int createTCPserverconnection(int verboze);
   int createTCPserverconnection(int port, int timeout, int timeoutAnswer, int verboze);
//������� ����������
   int closeConnection(void);
   int InitClientConnection(int verboze);

   int AddRoute(void);

//������� ����� � ��������������
   int SendAndConfirm(char buf[], int len, char bufout[], int lenout, int verboze);
//������� ����� � �������������� � ���������� ������ ������
   int SendAndConfirm2(char buf[], int len, char bufout[], int lenout, int shiftLansw, int size0, int FirstByte);
//������ ����������
   int GetInfo(void);
//������
   int Read(char bufin[], int len);
//������
   int Read(char bufin[], int len, int timeout);
//�������
   int Send(char buf[], int len);

};

class TCPclient
{
public:
	TCPconnection client;
	int id;
	int lifetime; // ����� �������� �������
	char buffer[1524];

	TCPclient(void)
	{  id = 0;
	   lifetime = 3600000;
	}
	int CommandHandle(int buflen);
};

clock_t GetClock(void);
clock_t GetClockInit(void);
int ShowMyIp(void);
int InitTCPIP(void);

#endif
//TCPCONNECTION_DEF
