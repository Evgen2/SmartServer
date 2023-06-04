/* UDPconnection.hpp */
//UTF-8
//����� UDPconnection
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

#endif //MSG1MSG2
class UDPconnection
{
public:
   int sock;
   struct sockaddr_in server;  //����� ���������� �������
   struct sockaddr_in client;  //��������� ���� �� �������
   int port;                   //����� ����� �� �������
   char IpTo[16];              //ip ����� ���������� ������� � char
   int timeout;                //������� �� ���������� � ��
   int timeoutAnswer;          //������� �� ���������� ������� � ��
   int timeLastRW;

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

   UDPconnection(void) //����������� �� ���������
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
      IdName[0] = 0; //�������� ����������
      IdCode = 0; // id ����������
      IdNum = 0;  // ����� ����������
	  IdType = 0;
   }
   ~UDPconnection(void) //����������
   { //if(sock > 0)
     //       soclose(sock);
   }
//������� ����������
   int createUDPconnection(char _IpTo[], int port, int timeout, int timeoutAnswer, int verboze);
   int createUDPserverconnection(int port, int timeout, int timeoutAnswer);
//������� ����������
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

//������� ����� � ��������������
   int SendAndConfirm(char bufin[], int len, char bufout[], int lenout);
   int SendAndConfirm(char bufin[], int len, char bufout[], int lenout, int verbose);
//������� ����� � �������������� � ���������� ������ ������
   int SendAndConfirm2(char bufin[], int len, char bufout[], int lenout, int shiftLansw, int size0, int FirstByte, int verbose);
//������ ����������
   int GetInfo(void);
   int InitClientConnection(void);
   //������
   int Read(char bufin[], int len);
//������
   int Read(char bufin[], int len, int timeout);
//�������
   int Send(char buf[], int len);

};

#endif //UDPCONNECTION_DEF
