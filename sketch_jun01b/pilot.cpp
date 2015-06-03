// pilot.cpp
// Provides driving control to robots
#include "pilot.h" //include the declaration for this class

// constructor
PILOT::PILOT(NXShield& nxt, NXTLight& light1, NXTLight& light2, RFIDuino& rfid)
{
  pNxShield = &nxt;
  pNxLight_1 = &light1;
  pNxLight_2 = &light2;
  prfid = &rfid;
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

// Right turn
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

// U-Turn
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

void PILOT::resetMotors()
{
	pNxShield->bank_a.motorReset();
}

void PILOT::straight(byte (&tagData)[5])
{
	//int combined = pNxLight_1->readRaw() + pNxLight_2->readRaw();
	while(!(prfid->decodeTag(tagData)))	{
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
		//combined = pNxLight_1->readRaw() + pNxLight_2->readRaw();
	}

	pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);

        /* Replace this code with new loop condition
        
	delay(1000);
	while(pNxShield->bank_a.motorGetEncoderPosition(SH_Motor_1)!=0){
		pNxShield->bank_a.motorResetEncoder(SH_Motor_1);
	}

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
	pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
	delay(500);
      */
}

