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

const int LDOCK = 12;
const int START = 4;

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
  
  delay(500);
}

void loop() {
  inputString = "";
  serialEvent();
  
  // Read target from system
  while(inputString == ""){
    inputString = "";
    serialEvent();
  }// end loop

  target = inputString.toInt();
  
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
  
}

/* 
 *Sets the current position based on corresponding
 *reference value of current tag data.
*/
void getPos(){
   bool tagComp = false;
   for (int i = 0; i < 12; i++){
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
    getPos();// ID current tag
    Serial.println(position);// Broadcast location
    
    //Tag checks to compensate for scale difference
    if(position == 5){// adjust number later
      if(loaded)
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
}

/*
 * Short routine simulates robot movement
 * as it delivers a pallet to the loading
 * dock
 */
void dropPallet(){
  loaded = false;
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

