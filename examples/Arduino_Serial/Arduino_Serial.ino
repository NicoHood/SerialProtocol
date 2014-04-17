/*
 Arduino_Serial.ino - SerialProtocol library - demo
 Copyright (c) 2014 NicoHood.  All right reserved.
 Daniel Garcia from the FASTLED library helped me with this code
 
 Some additional Information:
 *The library provides different definitions for printing with the same syntax
 Use: PRINTLN() PRINT() WRITE() PRINT_HEX() PRINT_DEC()
 *Enable debug output for errors/infos in the .h file!
 #define DEBUG_OUTPUT_ERRORS //outputs errors
 #define DEBUG_OUTPUT_INFOS //outputs infos
 If enabled you can use PRINT_INFO() PRINT_INFO_HEX() PRINT_ERR() PRINT_ERR_HEX()
 *You can switch between two Serials with just recalling setStream()
 This will cancel any pending reading
 *Find Serial device on Raspberry with ~ls /dev/tty*
 ARDUINO_UNO "/dev/ttyACM0"
 FTDI_PROGRAMMER "/dev/ttyUSB0"
 HARDWARE_UART "/dev/ttyAMA0"
 *Compile it like this
 cd /home/pi/Desktop/Arduino/Projects/Arduino_Raspberry/Arduino_Serial
 sudo make && sudo ./Pi_Serial
 */

//Button setup
int buttonPin=8;
unsigned long lastButton=0; //debounce

//Protocol
#include "SerialProtocol.h"


void setup()
{
  // button setup with pullup
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);

  delay(1000);
  while(!Serial && !Serial1); //for Leonardo wait for Pi or PC

  //still need to setup the Serial for this
  Serial1.begin(9600); //Protocol communication
  Serial.begin(9600);  //debug output

  PRINTLN("Arduino Startup!");

  Protocol.setStream(Serial1);
}


void loop()
{
  if(digitalRead(buttonPin)==LOW && millis()-lastButton>=200){
    PRINTLN("Button");

    // Write some testavalues
    // Syntax: Address 0-63, 32bit Data (unsigned long)
    Protocol.write(13, 0xABCDEF03); 
    Protocol.write(1, 0x01);
    Protocol.write(2, 0x00);
    Protocol.write(3, 0x3a);
    Protocol.write(42, 0x4224);
    Protocol.write(63, 0xffffffff);

    // stresstest with random bytes
    //#define DEBUG_OUTPUT_ERRORS //outputs errors
    //PRINTLN("+++Stresstest+++");
    //stresstest();

    // for debounce
    lastButton=millis(); 
  }

  if(Protocol.read()){
    PRINT_HEX(Protocol.getData());
    PRINT(" ");
    PRINT_HEX(Protocol.getAddress());
    PRINT("\n");    
  }

}

void stresstest (void){
  for(int i=0; i<256; i++){
    // write random raw bytes to check error detection
    Protocol.write(random(256));
  } 
}

