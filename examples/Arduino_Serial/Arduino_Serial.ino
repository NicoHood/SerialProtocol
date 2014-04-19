/*
Arduino_Serial.ino - SerialProtocol library - demo
 Copyright (c) 2014 NicoHood.  All right reserved.
 Daniel Garcia from the FASTLED library helped me with this code
 
 Some additional Information:
 *The library provides different definitions for printing with the same syntax
 Use: PRINTLN() PRINT() WRITE() PRINT_HEX() PRINT_DEC()
 *Enable debug output for errors/infos in the .h file!
 *You can switch between two Serials with just recalling setSerial()
 This will cancel any pending reading
 *Find Serial device on Raspberry with ~ls /dev/tty*
 ARDUINO_UNO "/dev/ttyACM0"
 FTDI_PROGRAMMER "/dev/ttyUSB0"
 HARDWARE_UART "/dev/ttyAMA0"
 *Compile it like this
 cd /home/pi/Desktop/Arduino/libraries/SerialProtocol/examples/Arduino_Serial
 sudo make && sudo ./Pi_Serial
 */

//Hardware Setup
int buttonPin=8;
unsigned long lastButton=0; //debounce
int ledPin = 9;
int sensorPin=18;
long previousMillis = 0; 
long previousMillis2 = 0; 

//Protocol
#include "SerialProtocol.h"


void setup()
{
  // button setup with pullup
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);

  delay(1000);
  while(!Serial && !Serial1); //for Leonardo wait for Pi or PC

  //still need to setup the Serial for this
  Serial1.begin(9600); //Protocol communication
  Serial.begin(9600);  //debug output

  Serial.println("Arduino Startup!");

  // Pass the Serial we want to communicate
  // you can also call the function in you loop
  // to change the Serial at any time
  Protocol.setSerial(Serial1);

}


void loop()
{
  if(digitalRead(buttonPin)==LOW && millis()-lastButton>=200){
    Serial.println("Button");

    // Write some testvalues
    // Syntax: Address 1-64, 32bit Data (unsigned long)
    Protocol.sendAddress(1, random(1000)); 

    // Write a direct 4 bit command
    // Syntax: Command: 1-16
    Protocol.sendCommand(2);

    // for debounce
    lastButton=millis(); 
  }

  // send analogread every 3 sec
  if(millis() - previousMillis > 3000) {
    Protocol.sendAddress(2, analogRead(sensorPin)); 
    previousMillis = millis();   
  }

  // send Ping every 10 sec
  if(millis() - previousMillis2 > 10000) {
    Protocol.sendCommand(1);
    previousMillis2 = millis();   
  }

  if(Protocol.read()){
    /*
    Serial.print(Protocol.getData());
     Serial.print(" ");
     Serial.print(Protocol.getAddress());
     Serial.print(" ");
     Serial.print(Protocol.getCommand());
     Serial.print("\n");   
     */

    switch(Protocol.getAddress()){
    case 0:
      // No Address -> Command
      break;
    case 1:
      analogWrite(ledPin, Protocol.getData());
      break;
    default:
      //not used
      break;
    }//end switch

    switch(Protocol.getCommand()){
    case 0:
      // No Command -> Address
      break;
    case 1:
      Serial.println("Ping!");
      break;
    default:
      //not used
      break;
    }//end switch

  }

}









