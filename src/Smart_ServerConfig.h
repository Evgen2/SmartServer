/* Smart_ServerConfig.h */
#ifndef SMART_CONFIG
#define SMART_CONFIG

#define APPLICATION_NAME1 L"Сервер умного дома"
#define APPLICATION_NAME2  "Сервер умного дома"
#define APPLICATION_NAME "SmartServer"
#define VERSION "0.00.00"
#define SUBVERSION "1"
#define CONFIG_VERSION 0

#define MAX_SMART_TERM_DEV 16
#define MAX_DEVICES 64


#ifdef B_D1_MINI
  #define TERMOSENSOR  TEMP_DS18B20
#elif B_NODEMSU
  #define TERMOSENSOR  TEMP_DS18B20
#else  
  #define TERMOSENSOR  TEMP_DHT11 
#endif  // B_D1_MINI

//есть LPC804
#define USE_LPC804  0

//есть датчик температуры
#define USE_SENSOR_T 1

//есть управление розетками 220V
#define USE_SMART_SOCKET  0

//есть счетчик импульсов
#define USE_COUNTER 0


#if defined(ARDUINO_ARCH_ESP8266)
 #define PROSESSOR_CODE  1
 #define IDENTIFY_TEXT        		"Умный датчик температуры ESP8266"
#elif defined(ARDUINO_ARCH_ESP32)
 #define PROSESSOR_CODE  2
 #define IDENTIFY_TEXT        		"Умный датчик температуры ESP32"
#endif

#define IDENTIFY_CODE   (PROSESSOR_CODE<<24)|(USE_LPC804<<20)|(USE_SMART_SOCKET<<16)|(USE_SENSOR_T<<8)|(USE_COUNTER)

#endif //SMART_CONFIG

