/*
SerialProtocol.cpp - SerialProtocol library - implementation
Copyright (c) 2014 NicoHood.  All right reserved.
Daniel Garcia from the FASTLED library helped me with this code
*/

#include "SerialProtocol.h"

// extern Protocol for easy use
CProtocol Protocol;

//================================================================================
//CProtocol (Constructor)
//================================================================================

CProtocol::CProtocol(){
	// initialize variables. mBlock and mWorkData doesnt need to be initialized
	// they will be initialized in setStream. mSerial=NULL/-1 stops reading without Initialization
	mData=0;
	mAddress=0;
	mCommand=0;
#ifdef RaspberryPi
	// for the filedescriptor
	mSerial=-1; 
#else //Arduino
	// for the pointer
	mSerial=NULL; 
#endif
}

//================================================================================
//SetSerial
//================================================================================

// Change the Serial being read from, this will cancel any
// pending data reads
#ifdef RaspberryPi
void CProtocol::setSerial(int fd) {
	mSerial=fd;
	mBlocks = 0;
	mWorkData = 0;
}
#else //Arduino
void CProtocol::setSerial(Stream *pStream) {
	mSerial = pStream;
	mBlocks = 0;
	mWorkData = 0;
}
#endif

//================================================================================
//Serial Functions
//================================================================================

uint8_t CProtocol::serRead(void){
	// we do not need to check mSerial because it was checked in serAvailable
#ifdef RaspberryPi
	return serialGetchar (mSerial);
#else //Arduino
	return mSerial->read();
#endif
}

int CProtocol::serAvailable(void){
#ifdef RaspberryPi
	if(mSerial<0) {
		printError("No Serial set");
		return 0;
	}
	else return serialDataAvail (mSerial);
#else //Arduino
	if (mSerial==NULL){
		printError("No Serial set");
		return 0;
	}
	else return mSerial->available();
#endif
}

void CProtocol::serWrite(uint8_t b){
#ifdef RaspberryPi
	if(mSerial<0){
		printError("No Serial set");
		return;
	}
	else serialPutchar (mSerial, b);
#else //Arduino
	// only write if there is a filedescriptor/pointer to write to
	if (mSerial==NULL){
		printError("No Serial set");
		return;
	}
	else mSerial->write(b); 

#endif
}

//================================================================================
//Debug Print
//================================================================================

void CProtocol::print(const char chars[]){	
#ifdef RaspberryPi
	// print to console
	printf("%s", chars);
	fflush(stdout);
#else //Arduino
	// print to debug Serial
	PROTOCOL_PRINT_OUTPUT.print(chars);
#endif
}

void CProtocol::print(uint32_t hex){	
#ifdef RaspberryPi
	// print to console
	printf("%x", hex);
	fflush(stdout);
#else //Arduino
	// print to debug Serial
	PROTOCOL_PRINT_OUTPUT.print(hex, HEX);
#endif
}

void CProtocol::printError(const char chars[]){	
#ifdef PROTOCOL_PRINT_ERROR
	print(chars);
#endif
}

void CProtocol::printError(uint32_t hex){	
#ifdef PROTOCOL_PRINT_ERROR
	print(hex);
#endif
}

void CProtocol::printInfo(const char chars[]){	
#ifdef PROTOCOL_PRINT_INFO
	print(chars);
#endif
}

void CProtocol::printInfo(uint32_t hex){	
#ifdef PROTOCOL_PRINT_INFO
	print(hex);
#endif
}

//================================================================================
//Read
//================================================================================

uint8_t CProtocol::read(void){
	while(serAvailable()){

		// get new input
		uint8_t input=serRead();
		printInfo(input);

		// check the lead/end/data indicator
		switch(input&0xC0){

			//lead 11
		case(0xC0): 
			{
				if(mBlocks!=0){
					// we were still reading!  Log an error
					printError("Overwriting lead\n"); 
				}

				// save block length + first data bits
				mBlocks = (input&0x38)>>3;
				if(mBlocks == 7) {
					mWorkData = input & 0x0F;
					mBlocks -=2;
				} 
				else if(mBlocks >= 2){
					mWorkData = input & 0x07;
					mBlocks--;
				} 
				else {
					// We got a 4 bit command
					// save command and return true for new input
					mCommand=(input & 0x0F)+1;
					mAddress=0;
					mData=0;
					mBlocks = 0;

					printInfo("4Bit Command: ");
					printInfo(mCommand);
					printInfo(" Finished!\n");

					return true;
				}
			}
			break;

			//end 10
		case(0x80): 
			{
				if(mBlocks--==1){
					// save data + address and return true for new input
					mAddress=(input&0x3F)+1;
					mData=mWorkData;
					mCommand=0;

					printInfo("16bit Address/Data: ");
					printInfo(mAddress);
					printInfo(mWorkData);
					printInfo(" Finished!\n");

					return true;
				}
				else{
					// log an error, not ready for an address byte, and reset data counters
					printError("End Err\n"); 
					mBlocks=0;
				}
			}
			break;

			//data 0x
		default: 
			{
				if(mBlocks--<2){
					// log an error, expecting an address or header byte
					printError("Data Err\n");
					mBlocks=0;
				}
				else{
					mWorkData<<=0x07;
					//dont need &0x7F because first bit is zero!
					mWorkData|=input; 
				}
			} 
			break;
		} // end switch
	}
	//no new input
	return false; 
}

//================================================================================
//Send Command/Address
//================================================================================

void CProtocol::sendCommand(uint8_t command){
	// send lead mask 11 + length 000 or 001 including the last bit for the 4 bit command
	serWrite(0xC0 | ((command-1)&0x0F));
	printInfo("Command: ");
	printInfo(command);
	printInfo(" Sending... ");
	printInfo(0xC0 | ((command-1)&0x0F));
	printInfo(" Done.\n");
}

void CProtocol::sendAddress(uint8_t address, uint32_t data){
	printInfo("Data: ");
	printInfo(data);
	uint8_t b[6];

	// b[0] has the ‘address’ byte
	b[0] = 0x80 | ((address-1) & 0x3F);

	// fill in the rest of the data, b[1]-b[5] is going to have data
	// in LSB order, e.g. b[1] will have the lowest 7 bits, b[2] the next
	// lowest, etc...
	uint8_t blocks=0;
	b[++blocks] = data & 0x7F;

	// only loop/shift if there's data to put out
	while(data > 0x7F) {
		data >>= 7;
		b[++blocks] = data & 0x7F;
	}

	// setup the header
	uint8_t lead = 0xC0;

	// if we can fit our highest bits in the first block, add them here
	if((blocks==5) || b[blocks] < 8) {
		lead |= b[blocks--] & 0x0F;
	}

	// add in the block count to lead
	lead |= (blocks+2)<<3;

	// now write out the data - lead, then the blocks array in reverse, which will
	// get your data written out in MSB order, ending with the address block
	printInfo(" Sending... ");
	printInfo(lead);
	serWrite(lead);
	do { 
		serWrite(b[blocks]); 
		printInfo(b[blocks]);
	} 
	while(blocks--);
	printInfo(" Done.\n");
}
