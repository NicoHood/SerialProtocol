/*
 Pi_Serial.cpp - SerialProtocol library - demo
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

#ifdef RaspberryPi //just that the Arduino IDE doesnt compile these files.

//include system librarys
#include <stdio.h> //for printf
#include <stdint.h> //uint8_t definitions
#include <stdlib.h> //for exit(int);
#include <string.h> //for errno
#include <errno.h> //error output

//wiring Pi
#include <wiringPi.h>
#include <wiringSerial.h>

//Protocol library
#include "SerialProtocol.h"
int fd; //filedescriptor
char device[]= FTDI_PROGRAMMER;
unsigned long baud = 9600;

//prototypes
int main(void);
void loop(void);
void setup(void);


void setup(){
  PRINTLN("Raspberry Startup!");

  //get filedescriptor
  if ((fd = serialOpen (device, baud)) < 0){
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    exit(1); //error
  }

  //setup GPIO in wiringPi mode 
  if (wiringPiSetup () == -1){
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    exit(1); //error
  }

  //pass the filedescriptor to the Protocol
  Protocol.setStream(fd);
}


void loop(){
  // if you want to read every input this time and clear the queue
  //while(Protocol.read()){ 
  // read one input if available
  if(Protocol.read()){
    PRINT_HEX(Protocol.getData());
    PRINT(" ");
    PRINT_HEX(Protocol.getAddress());
    PRINT("\n");    
  }
}


int main() {
  setup();
  while(1) loop();
  return 0;
}

#endif //#ifdef RaspberryPi

