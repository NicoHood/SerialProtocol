SerialProtocol
==============

A Protocol for Arduino - Raspberry Pi communication

*Click Download as zip to download*

Dokumentation:
==============
http://nicohood.wordpress.com/2014/04/18/arduino-raspberry-pi-serial-communication-protocol-via-usb-and-cc/

This protocol sends dynamic 32Bit of data with up to 64 addresses between the Arduino and Raspberry pi.
It can also be used for communication between two Pis or two Arduinos.

Main goal with this was to keep it small, fast and with a wide range of use. 64 addresses means that you can define your own functions for every address. For example address 0 ist for analog read between 0 and 1023 and address for only an led state. What you do with that is up to you.

More details about the Protocol:
maybe have a look at the concept.jpg or the .h file
It has some adventages:
You always know where you are in the receiving process. If data was lost: no problem, it will find the next leading byte. This is good for maybe wireless communication
It is designed small: Sending just a high/low state for leds only takes 2 bytes with address. Sending 10bit (analog value) takes exactly 3 bytes. And the maximum 32bit takes 6 byte. What you could do is to also add the length header 000 and 001 with 16 bit information. But then you have to edit the library. Maybe i could do that later.
You can send/receive from different Serial ports. Create another class or just reuse setStream. Only the pending data will be lost but normally it should just work fine.
And finally i defined some general functions that works on Pi and Arduino. You may not see all of them if you dont look into the .h but the PRINT() function is one example
  /*
   Calculate bytes (blocks) being send depending on the bitlength.
   For the protocol I do a little trick here:
   If it needs to send 6 blocks save the first bit of the data in the length.
   This works, because the length is never more than 6. The reader has to decode this of course
   
   ================================================
   |11LLLDDD||0DDDDDDD|0DDDDDDD|0DDDDDDD||10AAAAAA|
   ================================================
   Lead: 2bit lead indicator, 3bit length (including data bit #31), 3bit data
   Data: 7bit optional data (0-4 blocks)
   End : 2bit end indicator, 6bit address
   
   3bit length in leader:
   error    000   (Probably used for direct 3 bit commands)
   error    001   (Probably used for direct 3 bit commands)
   0-3   =2 010   2 blocks
   4-10  =3 011   3 blocks
   11-17 =4 100   4 blocks
   18-24 =5 101   5 blocks
   25-30 =6 11(0) 6 blocks + bit #31 is zero
   31    =7 11(1) 6 blocks + bit #31 is one
   */
  
   Questions? Just ask under my blog entry.

