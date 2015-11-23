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
  pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
}

// Left turn
void PILOT::turnLeft()
{
  // Turn roughly 60 degrees then search for line
        pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, (25));
        pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, (25));
        delay(560);

        pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, (20));
        pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, (20));
        while(abs(pNxLight_1->readRaw()-pNxLight_2->readRaw())<200){}
        pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
	      delay(200);
}

// Right turn
void PILOT::turnRight()
{	      
	      pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, (25));
        pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, (25));
        delay(560);
        
        pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, (25));
        pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, (25));
        while(abs(pNxLight_1->readRaw()-pNxLight_2->readRaw())<200){}
        pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
	      delay(200);
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

void PILOT::straight()
{       
        byte tagData[5];
	while(!digitalRead(9))  // Wait for pin to go low
        {               
	       error = pNxLight_1->readRaw() - pNxLight_2->readRaw();
               motorSpeed_1 = SPEED - (error / 15);
               motorSpeed_2 = SPEED + (error / 15);
               
               if(prfid->decodeTag(tagData))
                 return;
               pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, motorSpeed_1);
               if(prfid->decodeTag(tagData))
                 return;
	       pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, motorSpeed_2);
               if(prfid->decodeTag(tagData))
                 return;
	}
}

