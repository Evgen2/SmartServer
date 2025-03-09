echo 'make smartserver'
g++ Server2.cpp SmartServer2.cpp SmartServer.cpp SmartClient.cpp AutoConfig.cpp SmartDevice.cpp TCPconnection.cpp Smart_OT.cpp -pthread -o Smartserver
exit