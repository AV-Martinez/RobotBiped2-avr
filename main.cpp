#include <Arduino.h>
#include <Wire.h>
#include "main.h"
#include "Servo.h"

#define I2CADDRESS 8

Servo Servos[6];

/** I2C Slave at I2CADDRESS
 
        1st byte            2nd byte
    7 6 5 4 3 2 1 0     7 6 5 4 3 2 1 0
   +-+-+-+-+-+-+-+-+   +-+-+-+-+-+-+-+-+
    R A A A M 0 0 0     <    value    > 

   R   : Read (0) or write (1) servo position. If read operation, 2nd byte is not used
   AAA : Servo Id
   M   : For write operations, absolute (0) or relative (1) mode
	   	   0: 2nd byte contains absolute value
		   1: 2nd byte conitains relative value

**/

void receiveEvent(int howMany) {
	uint8_t ix = 0;
	uint8_t bytes[2];
	while (Wire.available() > 0) bytes[ix++] = Wire.read();
	if (ix == 1) {
		if (bitRead(bytes[0],7) == 1) {
			// Expected read operation
			return;
		}
		uint8_t servoId = (bytes[0] & 0x70) >> 4;
		Wire.write((uint8_t)Servos[servoId].read());
	}
	else if (ix == 2) {
		if (bitRead(bytes[0],7) == 0) {
			// Expected write operation
			return;
		}
		uint8_t servoId = (bytes[0] & 0x70) >> 4;
		uint8_t mode    = (bytes[0] & 0x08) >> 3;
		if (mode) 
			Servos[servoId].write(bytes[1]);
		else
			Servos[servoId].write(Servos[servoId].read() + (int8_t)(bytes[1]));
	}
	else {
		// Unexpected number of bytes received
		return;
	}
}

void Setup() {
	Servos[0].attach(6);
	Servos[1].attach(7);
	Servos[2].attach(8);
	Servos[3].attach(9);
	Servos[4].attach(10);
	Servos[5].attach(11);
	
	Servos[0].write(90);
	Servos[1].write(90);
	Servos[2].write(90);
	Servos[3].write(90);
	Servos[4].write(90);
	Servos[5].write(90);

	Wire.begin(I2CADDRESS);
	Wire.onReceive(receiveEvent);
}

void Loop() {
	// delay(1);
}

