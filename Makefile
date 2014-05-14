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
	@echo Successfully installed SerialProtocol library

hyperionmod:
	set -e sudo killall hyperiond
	sudo cp ./HyperionSource/hyperion-remote /usr/bin/
	sudo cp ./HyperionSource/hyperiond /usr/bin/
	@echo Successfully installed hyperionmod
	@echo please restart hyperion or reboot now

hyperionconfig:
	sudo cp ./HyperionSource/hyperion.config.json /etc/hyperion.config.json
	@echo Successfully installed hyperionconfig

serialtest: examples/Arduino_Serial/Pi_Serial.cpp
	gcc -o examples/Arduino_Serial/Pi_Serial.o examples/Arduino_Serial/Pi_Serial.cpp -DRaspberryPi -lwiringPi -lSerialProtocol -pedantic -Wall

ambilight: examples/Ambilight/Ambilight_Serial.cpp
	gcc -o examples/Ambilight/Ambilight_Serial.o examples/Ambilight/Ambilight_Serial.cpp -DRaspberryPi -lwiringPi -lSerialProtocol -pedantic -Wall
