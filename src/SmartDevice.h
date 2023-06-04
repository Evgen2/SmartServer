/* SmartDevice.h */
#ifndef SMARTDEVICE_H
#define SMARTDEVICE_H

#define APPLICATION_NAME5 L"������ ������ ����"

#include "UDPconnection.hpp"
#include "TCPconnection.hpp"
#include "DeviceType.h"

class SmartDevice_stub
{
 public:
	int type;  // ���-����� ����������
    unsigned char mac[6]; 
  
	int connectionType;  // ��� �����  1,2,3 = COM, UDP, TCP
	int sleeptype;   // 0 - �� ����, 1 - ����� �����
    int online;

    int BiosCode;
    int Version;
    int SubVersion;
	int Variant; //������� ����������
    char BiosDate[16]; //12 ����
    int need_init;

	SmartDevice_stub(void)
	{
	  type = -1;
      online = 0;
	  connectionType = 0;
      need_init = 1;
	  sleeptype = 0;
      BiosCode = Version = SubVersion = 0;
	  Variant = 0;
      BiosDate[0]=0;
      BiosDate[12]=0;
	  mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = mac[5] = 0;
	}
/****************************************/
};

class SmartTerm:public SmartDevice_stub
{
	public:  
	unsigned int ns;  // ������� ��� ������� ���������� (��� ������� ������������ �������)
	SmartTerm(void)
	{  ns = 0;
	}
    int	WriteSts(FILE *fp);
    int	ReadSts(FILE *fp);
};

class SmartDevice:public SmartDevice_stub
{
 public:

    UDPconnection lpc;   //UDP���������
    TCPconnection client;   //TCP ��������� 

    char ipaddr[32];
    int base_port;
    int isInited; /* ��������������� =1 */
    int num_lost; //���������� ���������� ������ �����������
    int num_send; //���������� ��������� ������ �����������
    int sts;   /* ��������� */

/****************************************/
/* ��������� ������� ��� ���������� �������������� ������� �� ������ ������ �� ����������, �.�. ���� ������ - ���� ���� */

	SmartDevice(void)
	{
      isInited = 0;
      strcpy(ipaddr,"192.168.10.100");
      base_port = 0x1a70;
	  sts = 0;
      num_lost = 0;
      num_send = 0;
	}
	int InitUDP(int debug_mode);
	int InitTCP(int debug_mode);
    int Test(int mode); /* ���� ������  */
	int GetSts0(int cmd, struct Msg1  *MsgIn, int lIn, struct Msg1  *MsgOut, int lOut);

	int Print(void);
};



#endif // SMARTDEVICE_H