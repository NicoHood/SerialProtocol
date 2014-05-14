/*
SerialProtocol.h - SerialProtocol library - description
Copyright (c) 2014 NicoHood.  All right reserved.
Daniel Garcia from the FASTLED library helped me with this code
*/

#ifndef SerialProtocol_h
#define SerialProtocol_h

//================================================================================
//Settings
//================================================================================

// empty

//================================================================================
//Raspberry only
//================================================================================

#ifdef RaspberryPi

//wiring Pi
#include <wiringPi.h>
#include <wiringSerial.h>

//include system librarys
#include <stdint.h> //uint8_t definitions

//Devices, check with ~ls /dev/tty*
#define ARDUINO_UNO "/dev/ttyACM0"
#define FTDI_PROGRAMMER "/dev/ttyUSB0"
#define HARDWARE_UART "/dev/ttyAMA0"

//================================================================================
//Arduino only
//================================================================================

#else //Arduino

//Arduino Library
#include <Arduino.h>

#endif //#ifdef RaspberryPi

//================================================================================
//Explanation
//================================================================================

/*
Calculate bytes (blocks) being send depending on the bitlength.
For the protocol I do a little trick here:
If it needs to send 6 blocks save the first bit of the data in the length.
This works, because the length is never more than 6. The reader has to decode this of course
Same works for the 4 bit command.

================================================
|11LLLDDD||0DDDDDDD|0DDDDDDD|0DDDDDDD||10AAAAAA|
================================================
Lead: 2bit lead indicator, 3bit length (including data bit #31, command bit #3), 3bit data/3bit command
Data: 7bit optional data (0-4 blocks)
End : 2bit end indicator, 6bit address

3bit length in leader:
command  00(0) 4bit command
command  00(1) 4bit command
0-3   =2 010   2 blocks
4-10  =3 011   3 blocks
11-17 =4 100   4 blocks
18-24 =5 101   5 blocks
25-30 =6 11(0) 6 blocks + bit #31 is zero
31    =7 11(1) 6 blocks + bit #31 is one

ErrorLevel:
left 4 bits for error type (lead, data, end, no reading Serial), other 4 bits for error count
===========
|LDER CCC|
===========
*/

#define PROTOCOL_ERR_LEAD 0x80
#define PROTOCOL_ERR_DATA 0x40
#define PROTOCOL_ERR_END  0x20
#define PROTOCOL_ERR_READ 0x10
#define PROTOCOL_ERR_MAX  0x0F

//================================================================================
//Protocol_ Class
//================================================================================

class CProtocol{
public:
	CProtocol();

#ifdef RaspberryPi
	// set filedescriptor
	void setSerial(int fd);
#else //Arduino
	// set serial stream pointer/reference
	inline void setSerial(Stream &stream) { setSerial(&stream); }
	inline void setSerial(Stream *pStream){ mSerial = pStream;  }

	inline bool read(Stream &stream){
		setSerial(&stream); return read();}
	inline int8_t sendCommand(uint8_t command, Stream &stream){
		setSerial(&stream); return sendCommand(command);}
	inline int8_t sendAddress(uint8_t address, uint32_t data, Stream &stream){
		setSerial(&stream); return sendAddress(address, data);}
#endif

	// user read/write functions
	bool read(void);
	int8_t sendCommand(uint8_t command);
	int8_t sendAddress(uint8_t address, uint32_t data);

	// access for the variables
	inline uint8_t  getCommand()   { return mCommand;    }
	inline uint8_t  getAddress()   { return mAddress;    }
	inline uint32_t getData()      { return mData;       }
	inline uint8_t  getErrorLevel(){ return mErrorLevel; }

private:
	// Fully read data
	uint8_t mCommand;
	uint8_t mAddress;
	uint32_t mData;
	uint8_t mErrorLevel;

	// in progress reading data
	uint8_t mBlocks;
	uint32_t mWorkData;

#ifdef RaspberryPi
	// filedescriptor for Serial
	int mSerial; 
#else //Arduino
	// Streampointer for Serial
	Stream *mSerial; 
	Stream *mPrevSerial; 
#endif

};

extern CProtocol Protocol;

#endif
