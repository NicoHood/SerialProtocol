#how to compile:
#cd /home/pi/Desktop/Arduino/libraries/SerialProtocol/
#sudo make

install: SerialProtocol.cpp
	gcc -Wall -fPIC -c SerialProtocol.cpp -lwiringPi -DRaspberryPi -c -pedantic
	gcc -shared -Wl,-soname,libSerialProtocol.so.1 -o libSerialProtocol.so.1.0   *.o
	install -m 0644 SerialProtocol.h /usr/local/include
	install -m 0755 -d /usr/local/lib
	install -m 0755 libSerialProtocol.so.1.0 /usr/local/lib/libSerialProtocol.so.1.0
	ln -sf /usr/local/lib/libSerialProtocol.so.1.0 /usr/lib/libSerialProtocol.so
	ldconfig
	rm *.o *.so.1.0

serialtest: examples/Arduino_Serial/Pi_Serial.cpp
	gcc -o examples/Arduino_Serial/Pi_Serial.o examples/Arduino_Serial/Pi_Serial.cpp -DRaspberryPi -lwiringPi -lSerialProtocol -pedantic -Wall

ambilight: examples/Ambilight_Serial/Ambilight_Serial.cpp
	gcc -o examples/Ambilight_Serial/Ambilight_Serial.o examples/Ambilight_Serial/Ambilight_Serial.cpp -DRaspberryPi -lwiringPi -lSerialProtocol -pedantic -Wall
