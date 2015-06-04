// Authors: Ed Baker, Alex Henderson, Jake Maynard

#include <NXShield.h>
#include <Wire.h>
#include <NXTLight.h>
#include <RFIDuino.h>
#include "tag.h"
#include "StopWatch.h"

NXShield nxshield;
NXTLight light1;
NXTLight light2;

#define SPEED 15

int motorSpeed_1;
int motorSpeed_2;
int correction;
int error;

void setup() {


  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(500);

  nxshield.init(SH_HardwareI2C);
  nxshield.waitForButtonPress(BTN_GO);
  nxshield.bank_a.motorReset();
  light1.init(&nxshield, SH_BAS2);
  light2.init(&nxshield, SH_BBS2);

  light1.setReflected();
  light2.setReflected();

  delay(1000);

  nxshield.bank_a.motorReset();
  delay(500);


  //byte tagData[5];
  //timer.start();
  int combined = light1.readRaw() + light2.readRaw();
  while (1)	{
    error = light1.readRaw() - light2.readRaw();
    
   correction = 5;
    if ( error < -10 ) {
      motorSpeed_1 = SPEED + correction;
      motorSpeed_2 = SPEED - correction;
    }
    else if ( error > 10 ) {
      motorSpeed_1 = SPEED - correction;
      motorSpeed_2 = SPEED + correction;
    } else {
      motorSpeed_1 = SPEED;
      motorSpeed_2 = SPEED;
    }
    nxshield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, motorSpeed_1);
    nxshield.bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, motorSpeed_2);
    combined = light1.readRaw() + light2.readRaw();
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
