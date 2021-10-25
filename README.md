# EasyMapper
Easy mapper for LoRaWAN networks allowing to discover new gateways, map their coverage and perform simple radio planning mesaurements.  
It is written in a way that HW the code is running on may be exchanged with minimum effort requiring addition of relevant wrappers/adapters to meet application interfaces.
## Coding framework
Project is created for PlatformIO framework but is built in an Arduino style where all the HW specific initialization is performed at startup and then setup() is being called once and next code is repetitively called in a loop().  
PlatformIO was selected due to its integration with Visual Studio Code providing better coding experience than when using an Arduino IDE.  
PlatformIO features also libraries management per project allowing to fix external code revision allowing to build the code properly each time.

