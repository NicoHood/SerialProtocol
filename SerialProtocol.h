/*
 SerialProtocol.h - SerialProtocol library - description
 Copyright (c) 2014 NicoHood.  All right reserved.
 Daniel Garcia from the FASTLED library helped me with this code
 */

// ensure this library description is only included once
#ifndef SerialProtocol_h
#define SerialProtocol_h

//================================================================================
//Settings
//================================================================================

//Debugoutput
//#define DEBUG_OUTPUT_ERRORS //outputs errors
//#define DEBUG_OUTPUT_INFOS //outputs infos
#define DEVICE_PRINT Serial //Arduino only

//Set values
#define SERIAL_TIMEOUT 1000 //<-- not used

//================================================================================
//Raspberry only
//================================================================================

#ifdef RaspberryPi

//wiring Pi
#include <wiringPi.h>
#include <wiringSerial.h>

//include system librarys
#include <stdio.h> //for printf
#include <stdint.h> //uint8_t definitions

//Devices, check with ~ls /dev/tty*
#define ARDUINO_UNO "/dev/ttyACM0"
#define FTDI_PROGRAMMER "/dev/ttyUSB0"
#define HARDWARE_UART "/dev/ttyAMA0"

//Serial functions
#define SERIAL_AVAILABLE() serialDataAvail (mpStream-1) //-1 because its set +1 in setStream
#define SERIAL_WRITE(data) serialPutchar (mpStream-1, uint8_t(data))
#define SERIAL_READ() serialGetchar (mpStream-1)
#define SERIAL_FLUSH() serialFlush(mpStream-1)

//Debug Print functions
#define WRITE(data) do{printf("%c", data); fflush(stdout);} while(0)
#define PRINTLN(data) do{printf("%s \n", data); fflush(stdout);} while(0)
#define PRINT(data) do{printf("%s", data); fflush(stdout);} while(0)
#define PRINT_HEX(data) do{printf("%x", data); fflush(stdout);} while(0)
#define PRINT_DEC(data) do{printf("%d", data); fflush(stdout);} while(0)

//================================================================================
//Arduino only
//================================================================================

#else //Arduino

//Arduino Library
#include <Arduino.h>
#define Arduino

//Serial functions
#define SERIAL_AVAILABLE() mpStream->available()
#define SERIAL_WRITE(data)  mpStream->write(uint8_t(data))
#define SERIAL_READ() mpStream->read()
#define SERIAL_FLUSH()  mpStream->flush()

//Debug Print functions
#define WRITE(data) DEVICE_PRINT.write(uint8_t(data))
#define PRINT(data) DEVICE_PRINT.print(data)
#define PRINTLN(data) DEVICE_PRINT.println(data)
#define PRINT_HEX(data) DEVICE_PRINT.print(data, HEX)
#define PRINT_DEC(data) DEVICE_PRINT.print(data, DEC)

#endif //#ifdef RaspberryPi

//================================================================================
//Global definitions
//================================================================================

//Debug output:
#ifdef DEBUG_OUTPUT_ERRORS
#define PRINT_ERR_HEX(data) do{PRINT("(err)"); PRINT_HEX(data);} while(0)
#define PRINT_ERR(message) do{PRINT("(err)"); PRINTLN(message);} while(0)
#else
#define PRINT_ERR_HEX(x) do{} while(0)
#define PRINT_ERR(x) do{} while(0)
#endif

#ifdef DEBUG_OUTPUT_INFOS
#define PRINT_INFO_HEX(data) do{PRINT("(i)"); PRINT_HEX(data);} while(0)
#define PRINT_INFO(message) do{PRINT("(i)"); PRINT(message);} while(0)
#else
#define PRINT_INFO_HEX(x) do{} while(0)
#define PRINT_INFO(x) do{} while(0)
#endif

//================================================================================
//Protocol_ Class
//================================================================================

class CProtocol{
public:
  CProtocol(); //constructor

  //set filedescriptor/stream pointer
#ifdef RaspberryPi
  void setStream(int fd);
#else //Arduino
  void setStream(Stream *pStream);
  inline void setStream(Stream &stream) { 
    setStream(&stream); 
  }
#endif

  //<-- add a sync?

  // user read/write functions
  uint8_t read(void);
  void write(uint8_t b);
  void write(uint8_t address, uint32_t data);

  // access for the variables
  inline uint32_t getData(){ 
    return mData; 
  }
  inline uint8_t getAddress(){ 
    return mAddress; 
  }

private:
  // Fully read data
  uint32_t mData;
  uint8_t mAddress;

  // in progress reading data
  uint8_t mBlocks;
  uint32_t mWorkData;

  // Filedescriptor/Stream pointer
#ifdef RaspberryPi
  int mpStream; //filedescriptor for Serial
#else //Arduino
  Stream *mpStream; // Stream for io
#endif

};

extern CProtocol Protocol;

#endif
