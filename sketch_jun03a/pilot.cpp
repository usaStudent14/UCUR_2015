// pilot.cpp
// Provides driving control to robots
#include "pilot.h" //include the declaration for this class
#include "StopWatch.h"

StopWatch driveTimer;
// constructor
PILOT::PILOT(NXShield& nxt, NXTLight& light1, NXTLight& light2)
{
  pNxShield = &nxt;
  pNxLight_1 = &light1;
  pNxLight_2 = &light2;
}

PILOT::~PILOT(){/*nothing to destruct*/}

// stop
void PILOT::stop(/*NXShield& nxt*/)
{
  pNxShield->bank_a.motorStop(SH_Motor_1, SH_Next_Action_Float);
  pNxShield->bank_a.motorStop(SH_Motor_2, SH_Next_Action_Float);
}

// Left turn
void PILOT::turnLeft()
{
	pNxShield->bank_a.motorRunDegrees(SH_Motor_1, 
                     SH_Direction_Reverse, 
                     (25),//speed
                     TURN_ROTATION,
                     SH_Completion_Dont_Wait,
                     SH_Next_Action_Float);
	pNxShield->bank_a.motorRunDegrees(SH_Motor_2, 
                     SH_Direction_Forward, 
                     (25),//speed
                     TURN_ROTATION,
                     SH_Completion_Wait_For,
                     SH_Next_Action_Float);

	pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
	delay(500);
}

void PILOT::turnRight()
{	
	pNxShield->bank_a.motorRunDegrees(SH_Motor_1, 
                     SH_Direction_Forward, 
                     (25),//speed
                     TURN_ROTATION,//rotation
                     SH_Completion_Dont_Wait,
                     SH_Next_Action_Float);
	pNxShield->bank_a.motorRunDegrees(SH_Motor_2, 
                     SH_Direction_Reverse, 
                     (25),//speed
                     TURN_ROTATION,//rotation
                     SH_Completion_Wait_For,
                     SH_Next_Action_Float);
	pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
	delay(500);
	
}

void PILOT::uTurn()
{
	pNxShield->bank_a.motorResetEncoder(SH_Motor_Both);

	pNxShield->bank_a.motorRunDegrees(SH_Motor_1, 
                     SH_Direction_Forward, 
                     (25),//speed
                     U_TURN_ROTATION,//rotation
                     SH_Completion_Dont_Wait,
                     SH_Next_Action_Float);
	pNxShield->bank_a.motorRunDegrees(SH_Motor_2, 
                     SH_Direction_Reverse, 
                     (25),//speed
                     U_TURN_ROTATION,//rotation
                     SH_Completion_Wait_For,
                     SH_Next_Action_Float);
	pNxShield->bank_a.motorReset();
}

void PILOT::testMe()
{
	// Serial.println(pNxLight_1->readRaw());
	// Serial.println(pNxLight_2->readRaw());
	// Serial.println("");
	// Serial.println(pNxLight_1->readRaw() + pNxLight_2->readRaw());
	// Serial.println("");
	// pNxShield->bank_a.motorResetEncoder(SH_Motor_1);
	// delay(500);
	// Serial.println(pNxShield->bank_a.motorGetEncoderPosition(SH_Motor_1));
}

void PILOT::resetMotors()
{
	pNxShield->bank_a.motorReset();
}

void PILOT::straight()
{
	int combined = pNxLight_1->readRaw() + pNxLight_2->readRaw();
	// Serial.print("Combined: ");
	// Serial.println(combined);
	while(combined < 950)	{
		// Serial.println("straight part one");
		error = pNxLight_1->readRaw() - pNxLight_2->readRaw();
		// correction = min( abs(error), 5 );
		correction = 5;
		if ( error < -10 ){
			motorSpeed_1 = SPEED + correction;
			motorSpeed_2 = SPEED - correction;
		}
		else if ( error > 10 ){
			motorSpeed_1 = SPEED - correction;
			motorSpeed_2 = SPEED + correction;
		}else{
			motorSpeed_1 = SPEED;
			motorSpeed_2 = SPEED;
		}
		pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, motorSpeed_1);
		pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, motorSpeed_2);
		combined = pNxLight_1->readRaw() + pNxLight_2->readRaw();
	}
	// Serial.print("last combined value: ");
	// Serial.println(combined);
	
	pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
	delay(1000);
	while(pNxShield->bank_a.motorGetEncoderPosition(SH_Motor_1)!=0){
		pNxShield->bank_a.motorResetEncoder(SH_Motor_1);
	}

	
	// Serial.print("encoder value after reset: ");
	// Serial.println(pNxShield->bank_a.motorGetEncoderPosition(SH_Motor_1));
	// delay(1000); // must delay after encoder reset

	while((pNxShield->bank_a.motorGetEncoderPosition(SH_Motor_1)) < DISTANCE) 
	{
		// Serial.println("straight part two");
		error = pNxLight_1->readRaw() - pNxLight_2->readRaw();
		// correction = min( abs(error), 5 );
		correction = 5;
		if ( error < -10 ){
			motorSpeed_1 = SPEED + correction;
			motorSpeed_2 = SPEED - correction;
		}else if ( error > 10 ){
			motorSpeed_1 = SPEED - correction;
			motorSpeed_2 = SPEED + correction;
		}else{
			motorSpeed_1 = SPEED;
			motorSpeed_2 = SPEED;
		}
		pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, motorSpeed_1);
		pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, motorSpeed_2);
	}
	// Serial.print("motor-1 speed: ");
	// Serial.println(pNxShield->bank_a.motorGetSpeed(SH_Motor_1));
	// Serial.print("motor-2 speed: ");
	// Serial.println(pNxShield->bank_a.motorGetSpeed(SH_Motor_2));
	// stop the motors after limit reached
	pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
	delay(500);
	// Serial.print("motor-1 speed after stop: ");
	// Serial.println(pNxShield->bank_a.motorGetSpeed(SH_Motor_1));
	// Serial.print("motor-2 speed after stop: ");
	// Serial.println(pNxShield->bank_a.motorGetSpeed(SH_Motor_2));
}

