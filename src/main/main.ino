#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h> // v1.0.8
#include <SPI.h> // Ethernet depends on the API library
#include <Ethernet.h> // v2.0.1
#include <Servo.h>

// Setting values needed for ethernet
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xAB, 0x14 };
IPAddress ip(192, 168, 1, 100);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Setting port and slave ID for TCP client
int port = 502;
int slaveId = 0;

// Use port 502
EthernetServer server(port);
ModbusTCPServer modbusTCPServer;

// Device Pins
int potPin = A0;
int ledPin = 9;
int pbPin = 2;
int servoPin = 3;

Servo servo;

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
  // Setting Pins
  servo.attach(servoPin);
  pinMode(ledPin, OUTPUT);
  pinMode(pbPin, INPUT);
  pinMode(potPin, INPUT);
  
  //Initalizing MODBUS TCP
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
  Serial.println(slaveId + 1);
  
  // Creating coils
  modbusTCPServer.configureCoils(0x00, 1);
  modbusTCPServer.configureDiscreteInputs(0x00, 1);
  modbusTCPServer.configureInputRegisters(0x00, 1);
  modbusTCPServer.configureHoldingRegisters(0x00, 1);
}

void loop() {

  EthernetClient client = server.available();

  if (client) {
    Serial.println("New client connected");

    modbusTCPServer.accept(client);

    while (client.connected()) {
      readInputPins();
      modbusTCPServer.poll();
      updateOutputPins();
    }

    logCoilsSerial();

    Serial.println("Client Disconected");
  }
}

void logCoilsSerial() {
  long coilValues[] = {
    modbusTCPServer.discreteInputRead(0x00),
    modbusTCPServer.coilRead(0x00),
    modbusTCPServer.inputRegisterRead(0x00),
    modbusTCPServer.holdingRegisterRead(0x00),
  };

  for ( int i = 0; i < 4; i++ ) {
    Serial.println("Device " + String(i) + " value: " + String(coilValues[i]));
  }
}

void readInputPins() {
  bool pbValue = digitalRead(pbPin);
  modbusTCPServer.discreteInputWrite(0x00, pbValue);

  int potValue = analogRead(potPin);
  modbusTCPServer.inputRegisterWrite(0x00, potValue);
}

void updateOutputPins() {
  bool ledValue = modbusTCPServer.coilRead(0x00);
  digitalWrite(ledPin, ledValue);

  int servoValue = modbusTCPServer.holdingRegisterRead(0x00);
  servo.write(servoValue);
}
