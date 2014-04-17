Pi_Serial: Pi_Serial.o SerialProtocol.o
	gcc -o Pi_Serial Pi_Serial.o SerialProtocol.o -lwiringPi -DRaspberryPi -pedantic -Wall
	rm *.o

Pi_Serial.o: Pi_Serial.cpp 
	gcc -o Pi_Serial.o Pi_Serial.cpp  -lwiringPi -DRaspberryPi -c -pedantic -Wall

SerialProtocol.o: SerialProtocol.cpp
	gcc -o SerialProtocol.o SerialProtocol.cpp -DRaspberryPi -lwiringPi -c -pedantic -Wall

clean:
	rm *.o Pi_Serial