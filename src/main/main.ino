// TODO ensure all github repos and versions are refenced in the README.md

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h> // v1.0.8
#include <SPI.h> // Ethernet depends on the API library
#include <Ethernet.h> // v2.0.1

/* 
  Address Pin Device
  0x00        Push Button
  0x01        LED
  0x02        Light Sensor/Potentiometer
  0x03        Servo
*/

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xAB, 0x14 };
IPAddress ip(192, 168, 1, 100);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Use port 502
EthernetServer server(502);
ModbusTCPServer modbusTCPServer;

/*
  // TODO move to README.MD
  Tasks: 
    - Create "Connected" LED to show if ethernet is working correctly
    - Create "Processing" LED to show when the 
    - Detemine if async should be used
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

  Serial.print("Arduino Modbus Slave Address: ");
  Serial.print(Ethernet.localIP());
  Serial.print(":502");
  if (!modbusTCPServer.begin()) {
    Serial.println("Failed to initalize Modbus TCP");
    doNothing();
  }

  modbusTCPServer.configureCoils(0x00, 2);
  modbusTCPServer.configureHoldingRegisters(0x02, 1);
  modbusTCPServer.configureInputRegisters(0x03, 1);
}

void loop() {

  EthernetClient client = server.available();

  if (client) {
    Serial.println("New client connected");

    modbusTCPServer.accept(client);

    while (client.connected()) {
      modbusTCPServer.poll();

      // TODO set/read all apropriate pins here
    }

    Serial.println("Client Disconected");

    logCoilsSerial();
    generateNewValues();
  }


}

void logCoilsSerial() {
  int coilValues[] = {
    modbusTCPServer.coilRead(0x00),
    modbusTCPServer.coilRead(0x01),
    modbusTCPServer.holdingRegisterRead(0x02),
    modbusTCPServer.inputRegisterRead(0x03),
  };

  for ( int i = 0; i < 4; i++ ) {
    Serial.println("Coil " + String(i) + " value: " + String(coilValues[i]));
  }
}

void generateNewValues() {
  bool new1Value = !(modbusTCPServer.coilRead(0x00));
  modbusTCPServer.coilWrite(0x00, new1Value);

  int new2Value = modbusTCPServer.holdingRegisterRead(0x02);

  new2Value += 1;

  if ( new2Value > 255 ) {
    new2Value = 0;
  }

  modbusTCPServer.holdingRegisterWrite(0x02, new2Value);
}
