#include "./libraries/ArduinoRS485/src/ArduinoRS485.h" // ArduinoModbus depends on the ArduinoRS485 library
#include "./libraries/ArduinoModbus/src/ModbusTCPServer.h" 
#include "./libraries/Ethernet/src/Ethernet.h"

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 100);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);


EthernetServer server(502);
ModbusTCPServer modbusTCPServer;

/*
  // TODO move to README.MD
  Tasks: 
    - Create "Connected" LED to show if ethernet is working correctly
    - Create "Processing" LED to show when the 

*/

void doNothing() {
    while (true) {
      // TODO: Add status LED for error with blink codes
      delay(1); 
    }
}

void logWithHint( char* message , char* hint ) {
  Serial.println(message);
  Serial.println(strcat("\tHINT: ", hint));
} 

void setup() {
  Ethernet.init(10);

  Ethernet.begin(mac, ip, myDns, gateway, subnet);

  Serial.begin(9600);
   while (!Serial) {
    ;
  }

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    logWithHint("Could not detect ethernet sheild", "Ensure ethernet shield is attatched");
    doNothing();
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    logWithHint( "No Ethernet Connection Found", "Ensure ethernet cable is connected to arduino and communicating device" );
  }

  server.begin();

  Serial.print("Arduino Slave Address:");
  Serial.println(Ethernet.localIP());
  if (!modbusTCPServer.begin()) {
    Serial.println("Failed to initalize Modbus TCP");
    doNothing();
  }

  // TODO determine if more than 5 coild needed
  modbusTCPServer.configureCoils(0x00, 1);
}

void loop() {

  EthernetClient client = server.available();

  if (client) {
    Serial.println("New client connected");

    modbusTCPServer.accept(client);

    while (client.connected()) {
      // TODO set/read all apropriate pins here
    }

    Serial.println("Client Disconected");
  }
}
