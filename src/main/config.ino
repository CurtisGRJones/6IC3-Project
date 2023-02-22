#include <SD.h>

/*
  This file is designed to parse a JSON file to make setup more dynamic
*/

struct NetworkConfig {
  byte *mac[6];
  IPAddress ip;
  IPAddress myDns;
  IPAddress gateway;
  IPAddress subnet;
}

networkConfig loadNetworkConfig(const char *filename) {
  File file = SD.open(filename)
  NetworkConfig networkConfig

  // TODO determine biffer size
  StaticJsonBuffer<512> jsonBuffer;

  JsonObject &root = jsonBuffer.parseObject(file);

  // TODO make this read from file
  networkConfig.mac = [ 0xA8, 0x61, 0x0A, 0xAE, 0xAB, 0x14 ]
  networkConfig.ip = new IPAdress(192, 168, 1, 100)
  networkConfig.myDns = new IPAdress(192, 168, 1, 100)
  networkConfig.gateway = new IPAdress(192, 168, 1, 1)
  networkConfig.subnet = new IPAddress(255, 255, 255, 0)

  
}
