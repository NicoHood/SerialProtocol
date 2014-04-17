/*
 SerialProtocol.cpp - SerialProtocol library - implementation
 Copyright (c) 2014 NicoHood.  All right reserved.
 Daniel Garcia from the FASTLED library helped me with this code
 */

// include this library's description file
#include "SerialProtocol.h"

//extern Protocol for easy use
CProtocol Protocol;

//================================================================================
//CProtocol (Constructor)
//================================================================================

CProtocol::CProtocol(){
  // initialize variables. mBlock and mWorkData doesnt need to be initialized
  // they will be initialized in setStream. mpStream=0 stops reading without Initialization
  mData=0;
  mAddress=0;
#ifdef RaspberryPi
  mpStream=0; //for the filedescriptor
#else //Arduino
  mpStream=NULL; //for the pointer
#endif
}

//================================================================================
//SetStream
//================================================================================

// Change the stream being read from, this will cancel any
// pending data reads
#ifdef RaspberryPi
void CProtocol::setStream(int fd) {
  mpStream=fd+1; //add +1 to use if(mpStream), for writing use -1
  mBlocks = 0;
  mWorkData = 0;
}
#else //Arduino
void CProtocol::setStream(Stream *pStream) {
  mpStream = pStream;
  mBlocks = 0;
  mWorkData = 0;
}
#endif

//================================================================================
//Read
//================================================================================

uint8_t CProtocol::read(void){
  while(mpStream && SERIAL_AVAILABLE()){

    // get new input
    uint8_t input=SERIAL_READ();
    PRINT_INFO_HEX(input); //debug output

      // check the lead/end/data indicator
    switch(input&0xC0){

      //lead 11
      case(0xC0): 
      {
        if(mBlocks!=0){
          // we were still reading!  Log an error
          PRINT_ERR("Overwriting lead"); 
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
          // We got corrupted data
          PRINT_ERR("Lead/4Bit info");
          // save 4bit information?<--
          mBlocks = 0;
        }
      }
      break;

      //end 10
      case(0x80): 
      {
        if(mBlocks--==1){
          // save data + address and return true for new input
          mAddress=input&0x3F;
          mData=mWorkData;

          // debug output
          PRINT_INFO("Data/Address:");
          PRINT_INFO_HEX(mWorkData);
          PRINT_INFO_HEX(mAddress);
          PRINT_INFO("Finished!\n");

          return true;
        }
        else{
          // log an error, not ready for an address byte, and reset data counters
          PRINT_ERR("End"); 
          mBlocks=0;
        }
      }
      break;

      //data 0x
    default: 
      {
        if(mBlocks--<2){
          // log an error, expecting an address or header byte
          PRINT_ERR("Data");
          mBlocks=0;
        }
        else{
          mWorkData<<=0x07;
          mWorkData|=input; //dont need &0x7F because first bit is zero!
        }
      } 
      break;
    } // end switch
  }
  return false; //no new input
}

//================================================================================
//Write
//================================================================================

//basic write function, can be used as raw input
void CProtocol::write(uint8_t b){
  if(mpStream) SERIAL_WRITE(b); // only write if there is a filedescriptor/pointer to write to
}

//actually used by the user
void CProtocol::write(uint8_t address, uint32_t data){
  PRINT_INFO("Data:");
  PRINT_INFO_HEX(data);
  uint8_t b[6];

  // b[0] has the ‘address’ byte
  b[0] = 0x80 | (address & 0x3F);

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
  PRINT_INFO("Sending...");
  PRINT_INFO_HEX(lead);
  write(lead);
  do { 
    write(b[blocks]); 
    PRINT_INFO_HEX(b[blocks]);
  } 
  while(blocks--);
  PRINT_INFO("Done.\n");
}
