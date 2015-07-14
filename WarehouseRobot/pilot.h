// Authors: Ed Baker, Alex Henderson, Jake Maynard

#ifndef PILOT_H
#define PILOT_H

//It is very important to remember this! note that if you are using Arduino 1.0 IDE, change "WProgram.h" to "Arduino.h" 
#include <Arduino.h>
#include <NXShield.h>
#include <NXTLight.h>
#include <RFIDuino.h>

#define U_TURN_ROTATION 334.9
// Pilot class provides directional, speed and steering control
class PILOT {
private:
	
public:
	PILOT(NXShield& nxt, NXTLight& light1, NXTLight& light2, RFIDuino& rfid);
	~PILOT();
	int error;
	int correction;
  int baseSpeed;
	int motorSpeed_1;
	int motorSpeed_2;
  byte tagData[5];
	NXShield * pNxShield;
	NXTLight * pNxLight_1;
	NXTLight * pNxLight_2;
  RFIDuino * prfid;

  void setSpeed(int speed);
	void stop();
  void backupLeft(byte (&tagBuffer)[5]);
  void backupRight();
	void turnLeft();
	void turnRight();
	void uTurn();
	void straight();
	void resetMotors();
};

#endif
