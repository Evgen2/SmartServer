﻿/* DeviceType.h */
#ifndef DEVICE_TYPE_H
#define DEVICE_TYPE_H


#define DS_PLUG	 0			// розетка
#define DS_TEMP	 1			// датчик температуры
#define DS_OPENTHERM	2	// контроллер OpenTherm
#define DS_BRIDE		3	// Мост ESP Now - UART/Wifi
#define DS_RELAY		4	// Реле Wifi
#define DS_OTHERM_EM	5	// контроллер эмулятора OpenTherm
#define DS_USER_APP		0x0100 // приложение пользователя 
#define DS_SMARTSERVER	0x1000 // сервер

//типы датчиков температуры
#define TEMP_DS18B20 1
#define TEMP_DHT11   2

#endif //DEVICE_TYPE_H
