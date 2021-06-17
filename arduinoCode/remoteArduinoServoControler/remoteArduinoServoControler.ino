/*
  LED

  This example creates a BLE peripheral with service that contains a
  characteristic to control an LED.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain. */


#include <ArduinoBLE.h>

BLEService launcherService("ed24d43a-a623-11eb-bcbc-0242ac130002"); // BLE LED Service

// BLE Feed characteristic (servo on pin D2) - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic feedCharacteristic("febe13d2-a623-11eb-bcbc-0242ac130002", BLERead | BLEWrite);

// BLE Speed characteristic (servo on pin D3) - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic speedCharacteristic("0c60be4a-a624-11eb-bcbc-0242ac130002", BLERead | BLEWrite);

// BLE Spin characteristic (servo on pin D5) - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic spinCharacteristic("1216429c-a624-11eb-bcbc-0242ac130002", BLERead | BLEWrite);

// BLE Spin characteristic (servo on pin D5) - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic oscillateCharacteristic("ac9fbcb2-ac32-11eb-8529-0242ac130003", BLERead | BLEWrite);


#include <Servo.h>
Servo feedServo;
Servo speedServo;
Servo spinServo;


void setup() {
  Serial.begin(9600); 

 feedServo.write(0);
 feedServo.attach(2); 
 delay(500);

  
 speedServo.write(0);
 speedServo.attach(3);
 delay(500);

 spinServo.write(90);
 spinServo.attach(5);

 pinMode(6, OUTPUT);
 digitalWrite(6,HIGH);
 
 

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("BALLLAUNCHER");
  BLE.setAdvertisedService(launcherService);

  // add the characteristic to the service
  launcherService.addCharacteristic(feedCharacteristic);
  launcherService.addCharacteristic(speedCharacteristic);
  launcherService.addCharacteristic(spinCharacteristic);
  launcherService.addCharacteristic(oscillateCharacteristic);

  // add service
  BLE.addService(launcherService);

  // set the initial value for the characeristic:
  feedCharacteristic.writeValue(20);
  speedCharacteristic.writeValue(10);
  spinCharacteristic.writeValue(90);
  oscillateCharacteristic.writeValue(0);


  
  // start advertising
  BLE.advertise();

  Serial.println("BLE TennisBallLauncher Peripheral all set"); 
}

void loop() {
  
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,      
      
      if (feedCharacteristic.written()) {
        Serial.print("Change to feedCharacteristic");
        Serial.println(feedCharacteristic.value());
        feedServo.write(feedCharacteristic.value());
      }

       if (speedCharacteristic.written()) {
          Serial.print("Change to speedCharacteristic");
        Serial.println(speedCharacteristic.value());
        speedServo.write(speedCharacteristic.value());
      }

       if (spinCharacteristic.written()) {
        Serial.print("Change to spinCharacteristic");
        Serial.println(spinCharacteristic.value());
        spinServo.write(spinCharacteristic.value());
      } 

      if (oscillateCharacteristic.written()) {
        Serial.print("Change to oscillateCharacteristic");
        Serial.println(oscillateCharacteristic.value());
        
        if (oscillateCharacteristic.value() == 0 ) {
          digitalWrite(6,HIGH);
        }
          
        if (oscillateCharacteristic.value() == 1) {
          digitalWrite(6,LOW);
        }       
      }

    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
