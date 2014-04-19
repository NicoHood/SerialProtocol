#how to compile:
#cd /home/pi/Desktop/Arduino/libraries/SerialProtocol/
#sudo make

SerialProtocol.o: SerialProtocol.cpp
	gcc -o SerialProtocol.o SerialProtocol.cpp -DRaspberryPi -lwiringPi -c -pedantic -Wall
