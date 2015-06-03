// Authors: Ed Baker, Alex Henderson, Jake Maynard

#include <NXShield.h>
#include <Wire.h>
#include <NXTLight.h>
#include <RFIDuino.h>
#include "StopWatch.h"
#include "tag.h"
#include "pilot.h"

NXShield nxshield;
NXTLight light1;
NXTLight light2;
RFIDuino rfid;

PILOT pilot(nxshield, light1, light2, rfid);

#define SPEED 15

// used for xbee receive
String inputString = "";
boolean stringComplete = false;

int x_coord;
int y_coord;

int motorSpeed_1;
int motorSpeed_2;
int correction;
int error;
int heading;
byte tagData[5];                  //Holds the ID numbers from the tag
byte tagDataBuffer[5];            //A Buffer for verifying the tag data
Tag tagRef[5][5]; 

void mapInit();

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
}

void loop() {
  
  
  // put your main code here, to run repeatedly:
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

// initialize the map with tag IDs
void mapInit()
{
 tagRef[0][0].setTagData(112,0,39,35,62);
 tagRef[1][0].setTagData(112,0,39,29,194);
 tagRef[2][0].setTagData(112,0,39,53,177);
 tagRef[3][0].setTagData(112,0,39,50,27);
 tagRef[4][0].setTagData(112,0,39,27,46);
 tagRef[0][1].setTagData(112,0,39,35,208);
 tagRef[1][1].setTagData(112,0,38,249,99);
 tagRef[2][1].setTagData(112,0,39,45,110);
 tagRef[3][1].setTagData(112,0,39,11,27);
 tagRef[4][1].setTagData(108,0,66,242,128);
 tagRef[0][2].setTagData(112,0,39,63,5);
 tagRef[1][2].setTagData(112,0,38,228,83);
 tagRef[2][2].setTagData(112,0,39,13,234);
 tagRef[3][2].setTagData(112,0,38,225,35);
 tagRef[4][2].setTagData(112,0,39,16,72);
 tagRef[0][3].setTagData(112,0,38,244,82);
 tagRef[1][3].setTagData(112,0,39,56,139);
 tagRef[2][3].setTagData(112,0,38,252,57);
 tagRef[3][3].setTagData(112,0,38,242,65);
 tagRef[4][3].setTagData(112,0,38,239,75);
 tagRef[0][4].setTagData(112,0,39,26,79);
 tagRef[1][4].setTagData(112,0,39,10,181);
 tagRef[2][4].setTagData(112,0,39,26,6);
 tagRef[3][4].setTagData(112,0,38,238,119);
 tagRef[4][4].setTagData(112,0,39,20,118);
}

