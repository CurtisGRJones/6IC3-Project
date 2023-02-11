#include <SD.h>

/*
  This file is designed to parse a JSON file to make setup more dynamic
*/

void parseFile(char *path)
{
    // TODO: read values from a JSON file on the SD or have defailt values
    byte mac[] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    IPAddress ip(192, 168, 1, 100);
    IPAddress myDns(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
}
