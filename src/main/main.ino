// TODO ensure all github repos and versions are refenced in the README.md

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h> // v1.0.8
#include <SPI.h> // Ethernet depends on the API library
#include <Ethernet.h> // v2.0.1
#include <Servo.h>

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

int port = 502;
int slaveId = 2;

// Use port 502
EthernetServer server(port);
ModbusTCPServer modbusTCPServer;

int potPin = A0;
int ledPin = 9;
int pbPin = 2;
int servoPin = 3;

Servo servo;

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
  servo.attach(servoPin);
  pinMode(ledPin, OUTPUT);
  pinMode(pbPin, INPUT);
  pinMode(potPin, INPUT);
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
  Serial.print(":");
  Serial.println(port);
  if (!modbusTCPServer.begin(slaveId)) {
    Serial.println("Failed to initalize Modbus TCP");
    doNothing();
  }

  Serial.print("Slave ID: ");
  Serial.println(slaveId);

  modbusTCPServer.configureCoils(0x00, 2);
  modbusTCPServer.configureInputRegisters(0x00, 1);
  modbusTCPServer.configureHoldingRegisters(0x00, 1);
}

void loop() {

  EthernetClient client = server.available();

  if (client) {
    Serial.println("New client connected");

    Serial.println("Using Values");
    logCoilsSerial();

    modbusTCPServer.accept(client);

    while (client.connected()) {
      modbusTCPServer.poll();
    }

    Serial.println("Client Disconected");
  }
  updateValues();
}

void logCoilsSerial() {
  long coilValues[] = {
    modbusTCPServer.coilRead(0x00),
    modbusTCPServer.coilRead(0x01),
    modbusTCPServer.inputRegisterRead(0x00),
    modbusTCPServer.holdingRegisterRead(0x00),
  };

  for ( int i = 0; i < 4; i++ ) {
    Serial.println("Device " + String(i) + " value: " + String(coilValues[i]));
  }
}

void updateValues() {
  bool pbValue = digitalRead(pbPin);
  modbusTCPServer.coilWrite(0x00, pbValue);

  int potValue = analogRead(potPin);
  modbusTCPServer.inputRegisterWrite(0x00, potValue);

  bool ledValue = modbusTCPServer.coilRead(0x01);
  digitalWrite(ledPin, ledValue);

  int servoValue = modbusTCPServer.holdingRegisterRead(0x00);
  servo.write(servoValue);

  // bool newButtonValue = !(modbusTCPServer.coilRead(0x00));
  // modbusTCPServer.coilWrite(0x00, newButtonValue);

  //long newPotValue = modbusTCPServer.inputRegisterRead(0x00);

  //newPotValue += 1;

  //if ( newPotValue > 255 ) {
    //newPotValue = 0;
  //}

  //modbusTCPServer.inputRegisterWrite(0x00, newPotValue);
}
