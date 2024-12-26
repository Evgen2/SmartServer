echo 'make smartserver'
# g++ SmartServer.cpp SmartClient.cpp SmartServerMain.cpp SmartServerClient.cpp SmartDevice.cpp OpenThermTest.cpp AutoConfig.cpp UDPconnection.cpp TCPconnection.cpp -std=c++0x -pthread -o smartserver
g++ Server2.cpp SmartServer2.cpp SmartServer.cpp SmartClient.cpp AutoConfig.cpp SmartDevice.cpp TCPconnection.cpp -pthread -o Smartserver
exit