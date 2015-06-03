#ifndef PILOT_H
#define PILOT_H

//It is very important to remember this! note that if you are using Arduino 1.0 IDE, change "WProgram.h" to "Arduino.h" 
#include <Arduino.h>
#include <NXShield.h>
#include <NXTLight.h>
#include <RFIDuino.h>

// #define TURN_ROTATION 167.45
#define TURN_ROTATION 175
#define U_TURN_ROTATION 334.9
#define SPEED 15 // was 20
#define DISTANCE 200
// Pilot class provides directional, speed and steering control
class PILOT {
private:
	
public:
	PILOT(NXShield& nxt, NXTLight& light1, NXTLight& light2, RFIDuino& rfid);
	~PILOT();
	int error;
	int correction;
	int motorSpeed_1;
	int motorSpeed_2;
        byte tagData[5];
	NXShield * pNxShield;
	NXTLight * pNxLight_1;
	NXTLight * pNxLight_2;
        RFIDuino * prfid;
	void stop();
	void turnLeft();
	void turnRight();
	void uTurn();
	void straight(byte (&tagData)[5]);
	void resetMotors();
};

#endif
