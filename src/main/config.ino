#include <SD.h>

/*
  This file is designed to parse a JSON file to make setup more dynamic
*/

void parseFile(char *path)
{
    // TODO: read values from a JSON file on the SD or have defailt values
    byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xAB, 0x14 };
    IPAddress ip(192, 168, 1, 100);
    IPAddress myDns(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
}
