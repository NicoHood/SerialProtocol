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
	mErrorLevel=0;
#ifdef RaspberryPi
	// for the filedescriptor
	mSerial=-1; 
#else //Arduino
	// for the pointer
	mSerial=NULL; 
	mPrevSerial=NULL;
#endif
}

//================================================================================
//SetSerial
//================================================================================

// Change the Serial being read from, this will cancel any
// pending data reads
#ifdef RaspberryPi
void CProtocol::setSerial(int fd) {
	if(mSerial!=fd){
		mSerial=fd;
		mBlocks = 0;
		mWorkData = 0;
	}
}
#else //Arduino
// inline
//void CProtocol::setSerial(Stream *pStream) {
//	mSerial = pStream;
//}
#endif

//================================================================================
//Read
//================================================================================

bool CProtocol::read(void){
	// reset fully read data
	mErrorLevel=0; 
	mCommand=0;
	mAddress=0;
	mData=0;

	// only read if there is a filedescriptor/pointer to write to
	// and only if there is new input and get this input
#ifdef RaspberryPi
	while(mSerial>=0 && serialDataAvail (mSerial)){
		uint8_t input = serialGetchar (mSerial);
#else //Arduino
	if(mSerial==NULL) {
		mErrorLevel=PROTOCOL_ERR_READ;
		return false;
	}
	// only reset pending data if the reading stream changes
	if(mSerial!=mPrevSerial){
		mPrevSerial=mSerial;
		mBlocks = 0;
		mWorkData = 0;
	}
	while(mSerial->available()){
		uint8_t input = mSerial->read();
#endif

		// check the lead/end/data indicator
		switch(input&0xC0){

			//lead 11
		case(0xC0): 
			{
				// we were still reading!  Log an error
				if(mBlocks!=0) ++mErrorLevel |= PROTOCOL_ERR_LEAD;

				// read block length or command indicator
				mBlocks = (input&0x38)>>3;
				switch(mBlocks){
					// save 4 bit command
				case 0:
				case 1:
					mCommand=(input & 0x0F)+1;
					mBlocks = 0;
					return true;
					break;
					// save block length + first 3 data bits
				case 7:
					mWorkData = input & 0x0F;
					mBlocks -=2;
					break;
				default:
					mWorkData = input & 0x07;
					mBlocks--;
					break;
				}
			}
			break;

			//end 10
		case(0x80): 
			{
				if(mBlocks--==1){
					// save data + address
					mAddress=(input&0x3F)+1;
					mData=mWorkData;
					return true;
				}
				else{
					// log an error, not ready for an address byte, and reset data counters
					++mErrorLevel |= PROTOCOL_ERR_DATA;
					mBlocks=0;
				}
			}
			break;

			//data 0x
		default: 
			{
				if(mBlocks--<2){
					// log an error, expecting an address or header byte
					++mErrorLevel |= PROTOCOL_ERR_END;
					mBlocks=0;
				}
				else{
					// get next 7 bits of data
					mWorkData<<=7;
					// dont need &0x7F because first bit is zero!
					mWorkData|=input; 
				}
			} 
			break;
		} // end switch

		// too many errors, stop reading
		if((mErrorLevel&PROTOCOL_ERR_MAX)==PROTOCOL_ERR_MAX) return false;
	}
	//no new input
	return false; 
}

//================================================================================
//Send Command/Address
//================================================================================


int8_t CProtocol::sendCommand(uint8_t command){
	// Only write if there is a Stream/filedescriptor to write to
	// send lead mask 11 + length 00|0 or 00|1 including the last bit for the 4 bit command
	uint8_t b ={(0xC0 | ((command-1)&0x0F))};
#ifdef RaspberryPi
	if(mSerial<0) return-1;
	serialPutchar (mSerial, b);
#else //Arduino
	if (mSerial==NULL) return-1;
	mSerial->write(b);
#endif
	return 1;
}


int8_t CProtocol::sendAddress(uint8_t address, uint32_t data){
	// Only write if there is a Stream/filedescriptor to write to
#ifdef RaspberryPi
	if(mSerial<0) return-1;
#else //Arduino
	if (mSerial==NULL) return-1;
#endif

	// block buffer for sending
	uint8_t b[6];

	// b[5] has the ‘address’ byte
	b[5] = 0x80 | ((address-1) & 0x3F);

	// fill in the rest of the data, b[0]-b[4] is going to have data
	// in MSB order, e.g. b[4] will have the lowest 7 bits, b[3] the next
	// lowest, etc...
	b[4] = data & 0x7F;

	uint8_t blocks=2;

	// only loop/shift if there's data to put out
	while(data>0x7F) {
		data >>= 7;
		b[6-(++blocks)] = data & 0x7F;
	}

	// if we can fit our highest bits in the first block, add them here
	if((blocks==6) || b[6-blocks] < 8) {
		// add to existing data
	}
	// if not just initialize the next byte
	else b[6-(++blocks)] =  0;

	// add in the block count to lead
	b[6-blocks] |=  0xC0 | (blocks<<3);

	// now write out the data - the blocks array in reverse, which will
	// get your data written out in LSB order
#ifdef RaspberryPi
	for(int i=0;i<blocks;i++){
		serialPutchar (mSerial, b[6-blocks+i]);
	}
#else //Arduino
	// we need to write the buffer as array to work for the Wire library
	mSerial->write(&b[6-blocks],blocks);
#endif

	// return the number of blocks written to the Serial
	return blocks;
}


//void CProtocol::sendAddress(uint8_t address, uint32_t data){
//	// block buffer for sending
//	uint8_t b[6];
//
//	// b[0] has the ‘address’ byte
//	b[0] = 0x80 | ((address-1) & 0x3F);
//
//	// fill in the rest of the data, b[1]-b[5] is going to have data
//	// in LSB order, e.g. b[1] will have the lowest 7 bits, b[2] the next
//	// lowest, etc...
//	uint8_t blocks=0;
//	b[++blocks] = data & 0x7F;
//
//	// only loop/shift if there's data to put out
//	while(data > 0x7F) {
//		data >>= 7;
//		b[++blocks] = data & 0x7F;
//	}
//
//	// setup the header
//	uint8_t lead = 0xC0;
//
//	// if we can fit our highest bits in the first block, add them here
//	if((blocks==5) || b[blocks] < 8) {
//		lead |= b[blocks--] & 0x0F;
//	}
//
//	// add in the block count to lead
//	lead |= (blocks+2)<<3;
//
//	// now write out the data - lead, then the blocks array in reverse, which will
//	// get your data written out in MSB order, ending with the address block
//	serWrite(lead);
//	do { 
//		serWrite(b[blocks]); 
//	} 
//	while(blocks--);
//}
