//
#define APPLICATION_NAME6 L"Сервер умного дома"
/* DeviceType.h */
#ifndef DEVICE_TYPE_H
#define DEVICE_TYPE_H

/* 0 - Розетка, 1 - датчик температуры
   2 - OpenTherm
   3 - Мост ESP Now - UART/Wifi
 */ 

#define DS_PLUG	 0  // розетка
#define DS_TEMP	 1  // датчик температуры
#define DS_OPENTHERM	 2 // контроллер OpenTherm
#define DS_BRIDE	 3 // Мост ESP Now - UART/Wifi
#define DS_RELAY	 4 // Реле Wifi

//типы датчиков температуры
#define TEMP_DS18B20 1
#define TEMP_DHT11   2

//	int connectionType;  // тип связи  1,2,3 = COM, UDP, TCP

#define CONNECT_NO	0
#define CONNECT_COM	1
#define CONNECT_UDP 2
#define CONNECT_TCP 3
#endif //DEVICE_TYPE_H
