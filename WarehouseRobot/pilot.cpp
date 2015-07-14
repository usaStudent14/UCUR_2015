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
  baseSpeed = 24;
}

PILOT::~PILOT() {
  /*nothing to destruct*/
}

void PILOT::setSpeed(int speed) {
  baseSpeed = speed;
}

// stop
void PILOT::stop(/*NXShield& nxt*/)
{
  pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
}

void PILOT::backupLeft(byte (&tagBuffer)[5]){
  // Multiple calls to motor 2 since first call often doesn't work
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, (3));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, (24));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, (3));
  delay(2200);

  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, (9));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, (9));
  delay(1500);

  int combined = 0;
  byte tagData[5];
  prfid->transferToBuffer(tagBuffer, tagData);

   while(combined < 950 && prfid->compareTagData(tagData, tagBuffer)){
     combined = pNxLight_1->readRaw() + pNxLight_2->readRaw();
     prfid->decodeTag(tagData);
   }

  prfid->transferToBuffer(tagData, tagBuffer);
  stop();
  delay(1000);
  
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, (9));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, (9));
  delay(1500);
  
  while(!prfid->decodeTag(tagData)){}
  stop();
  prfid->transferToBuffer(tagData, tagBuffer);
  turnLeft();
}
 
void PILOT::backupRight(){
  delay(10);
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, (9));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, (9));
  delay(300);
  
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, (0));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, (24));
  delay(1500);

  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, (9));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, (9));
  delay(1500);

  int combined = 0;

   while(combined < 950){
     combined = pNxLight_1->readRaw() + pNxLight_2->readRaw();
   }

  stop();
  delay(1000);
  
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, (9));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, (10));
  delay(1500);

  byte tagData[5];
  while(!prfid->decodeTag(tagData)){}
  stop();
  turnRight();
}
  
// Left turn
void PILOT::turnLeft()
{
  // Turn roughly 60 degrees then search for line
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, (24));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, (24));
  delay(560);

  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, (20));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, (20));
  while (abs(pNxLight_1->readRaw() - pNxLight_2->readRaw()) < 200) {}
  pNxShield->bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
  delay(200);
}

// Right turn
void PILOT::turnRight()
{
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, (24));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, (24));
  delay(560);

  pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, (20));
  pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, (20));
  while (abs(pNxLight_1->readRaw() - pNxLight_2->readRaw()) < 200) {}
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
  while (!digitalRead(9)) // Wait for pin to go low
  {
    error = pNxLight_1->readRaw() - pNxLight_2->readRaw();
    motorSpeed_1 = baseSpeed - (error / 15);
    motorSpeed_2 = baseSpeed + (error / 15);

    if (prfid->decodeTag(tagData))
      return;
    pNxShield->bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, motorSpeed_1);
    if (prfid->decodeTag(tagData))
      return;
    pNxShield->bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, motorSpeed_2);
    if (prfid->decodeTag(tagData))
      return;
  }
}

