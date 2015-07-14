// Authors: Alex Henderson, Jake Maynard

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

const int LDOCK = 6;
const int START = 5;

int position = START;     //Current Position of the robot
int target = -1;          //Current target of the robot
bool loaded = false;      //Whether the robot has a pallet
String inputString = "";  //Input from the XBEE channel
int heading = 0;          /*Current heading (0 = away from loading, 
                           *1 = toward loading, 2 = parallel to assembly line)
                           */
byte tagData[5];          //ID numbers of the current tag
byte tagDataBuffer[5];    //ID numbers of the previous tag
Tag tagRef[12];           //Road map

void mapInit();
void getPos();
void move(int currentTarg);
void grabPallet();
void dropPallet();
void serialEvent();

void setup() {
  mapInit();
  Serial.begin(9600);
  delay(500);

  inputString.reserve(200);

  nxshield.init(SH_HardwareI2C);
  nxshield.waitForButtonPress(BTN_GO);
  light1.init(&nxshield, SH_BAS2);
  light2.init(&nxshield, SH_BBS2);

  light1.setReflected();
  light2.setReflected();
  pilot.resetMotors();

  nxshield.ledSetRGB(0, 5, 0);
   
  delay(500);

  Serial.print(START);
}

void loop() {
  target = 9;// Default dummy value

  // Read target from system
  while(target == 9){
    inputString = "";
    while(inputString == ""){
      serialEvent();
    }// end loop
  
    target = inputString[0] - '0';
  }

  // Move towards assembly line target
  move(target);
  
  // Pallet retrieval routine
  grabPallet();
  
  // Move towards loading dock
  move(LDOCK);
  
  // Pallet delivery routine
  dropPallet();
  
  // Return to start
  move(START);
}

// Initializes map with tag ID's
void mapInit(){
  tagRef[0].setTagData(121, 0, 114, 163, 72); //Line A
  tagRef[1].setTagData(121, 0, 114, 187, 52); //Line B
  tagRef[2].setTagData(121, 0, 114, 134, 137);//Line C
  tagRef[3].setTagData(121, 0, 114, 192, 13); //Line D
  tagRef[4].setTagData(121, 0, 114, 150, 213);//Line E
  tagRef[5].setTagData(121, 0, 114, 90, 77);  //Door
  tagRef[6].setTagData(121, 0, 114, 133, 51); //Loading Dock
}

/* 
 *Sets the current position based on corresponding
 *reference value of current tag data.
*/
void getPos(){
   bool tagComp = false;
   for (int i = 0; i < 7; i++){
     tagComp = rfid.compareTagData(tagData, tagRef[i].tagData);
     if (tagComp){
       position = i;
       rfid.successSound();
       return;
     }
  }// end loop
}

/*
 * Keeps robot in motion till the current position
 * is equivalent to currentTarg. Includes tag checks
 * to adjust for scale differences.
 */
void move(int currentTarg){
  while(position != currentTarg){
    byte tempTagData[5];
    
    // Travel straight to next position
    nxshield.bank_a.motorRunUnlimited(SH_Motor_Both, SH_Direction_Forward, 20);
    delay(300);
    
    while (rfid.compareTagData(tagData, tagDataBuffer)) {
      pilot.straight();
  
      if (rfid.decodeTag(tempTagData))
        rfid.transferToBuffer(tempTagData, tagData);
    }// end inner loop
    rfid.transferToBuffer(tagData, tagDataBuffer);
    getPos();// ID current tag
    Serial.print(position);// Broadcast location
    
    //Tag checks to compensate for scale difference
    if(position == 5){
      if(heading == 1)
        pilot.setSpeed(9);
      else
        pilot.setSpeed(24);
    }
  }// end loop
  
  pilot.stop();
}

/*
 * Short routine simulates robot movement
 * as it retrieves a pallet from an assembly
 * line
 */
void grabPallet(){
  loaded = true;
  pilot.backupLeft(tagDataBuffer);
  
  heading = 1;
}

/*
 * Short routine simulates robot movement
 * as it delivers a pallet to the loading
 * dock
 */
void dropPallet(){
  loaded = false;
  pilot.backupRight();
    
  heading = 0;
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    inputString += inChar;
    delay(10);
    if (inChar == '\n' || inChar == '\r') {
      return;
    }
  }
}

