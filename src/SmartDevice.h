/* SmartDevice.h */
#ifndef SMARTDEVICE_H
#define SMARTDEVICE_H

#define APPLICATION_NAME5 L"Сервер умного дома"

#include "UDPconnection.hpp"
#include "TCPconnection.hpp"
#include "DeviceType.h"

class SmartDevice_stub
{
 public:
	int type;  // тип-класс устройства
    unsigned char mac[6]; 
  
	int connectionType;  // тип связи  1,2,3 = COM, UDP, TCP
	int sleeptype;   // 0 - не спит, 1 - может спать
    int online;

    int BiosCode;
    int Version;
    int SubVersion;
	int Variant; //Вариант исполнения
    char BiosDate[16]; //12 байт
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
	unsigned int ns;  // сколько раз передал информацию (для расчета потребленной энергии)
	SmartTerm(void)
	{  ns = 0;
	}
    int	WriteSts(FILE *fp);
    int	ReadSts(FILE *fp);
};

class SmartDevice:public SmartDevice_stub
{
 public:

    UDPconnection lpc;   //UDPконнекшен
    TCPconnection client;   //TCP коннекшен 

    char ipaddr[32];
    int base_port;
    int isInited; /* инициализирован =1 */
    int num_lost; //количество потерянных команд контроллера
    int num_send; //количество переданых команд контроллера
    int sts;   /* состояние */

/****************************************/
/* мутексный семафор для блокировки одновременного доступа из разных тредов не используем, т.к. один клиент - один тред */

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
    int Test(int mode); /* тест обмена  */
	int GetSts0(int cmd, struct Msg1  *MsgIn, int lIn, struct Msg1  *MsgOut, int lOut);

	int Print(void);
};



#endif // SMARTDEVICE_H