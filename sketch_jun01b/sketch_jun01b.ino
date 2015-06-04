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
#define ID 1  //Unique id for each robot

struct coords{
  int x = 0;
  int y = 0;
};

coords rob_track[2]; // Initialize to number of robots in system

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
void get_pos();
void serialEvent();

void setup() {
  mapInit();
  Serial.begin(9600);
  delay(500);

  nxshield.init(SH_HardwareI2C);
  nxshield.waitForButtonPress(BTN_GO);
  light1.init(&nxshield, SH_BAS2);
  light2.init(&nxshield, SH_BBS2);

  light1.setReflected();
  light2.setReflected();

  delay(1000);
  pilot.resetMotors();
  delay(500);

  //ID initial location
  rfid.decodeTag(tagData);
  get_pos();  

  nxshield.ledSetRGB(5, 0, 0);
}

void loop() {

  //Broadcast location
  Serial.print(ID);
  Serial.print(" ");
  Serial.print(x_coord);
  Serial.print(" ");
  Serial.println(y_coord);
  
  //Recieve other robots' info
  rob_track[input
  
  //Check for locations visited

  //Pick move

  //Broadcast intended location

  //Move

  //ID new location
 
}

// initialize the map with tag IDs
void mapInit()
{
  tagRef[0][0].setTagData(112, 0, 39, 35, 62);
  tagRef[1][0].setTagData(112, 0, 39, 29, 194);
  tagRef[2][0].setTagData(112, 0, 39, 53, 177);
  tagRef[3][0].setTagData(112, 0, 39, 50, 27);
  tagRef[4][0].setTagData(112, 0, 39, 27, 46);
  tagRef[0][1].setTagData(112, 0, 39, 35, 208);
  tagRef[1][1].setTagData(112, 0, 38, 249, 99);
  tagRef[2][1].setTagData(112, 0, 39, 45, 110);
  tagRef[3][1].setTagData(112, 0, 39, 11, 27);
  tagRef[4][1].setTagData(108, 0, 66, 242, 128);
  tagRef[0][2].setTagData(112, 0, 39, 63, 5);
  tagRef[1][2].setTagData(112, 0, 38, 228, 83);
  tagRef[2][2].setTagData(112, 0, 39, 13, 234);
  tagRef[3][2].setTagData(112, 0, 38, 225, 35);
  tagRef[4][2].setTagData(112, 0, 39, 16, 72);
  tagRef[0][3].setTagData(112, 0, 38, 244, 82);
  tagRef[1][3].setTagData(112, 0, 39, 56, 139);
  tagRef[2][3].setTagData(112, 0, 38, 252, 57);
  tagRef[3][3].setTagData(112, 0, 38, 242, 65);
  tagRef[4][3].setTagData(112, 0, 38, 239, 75);
  tagRef[0][4].setTagData(112, 0, 39, 26, 79);
  tagRef[1][4].setTagData(112, 0, 39, 10, 181);
  tagRef[2][4].setTagData(112, 0, 39, 26, 6);
  tagRef[3][4].setTagData(112, 0, 38, 238, 119);
  tagRef[4][4].setTagData(112, 0, 39, 20, 118);
}


void get_pos() {
  bool tagComp = false;
  for (int x = 0; x < 5; x++)
  {
    for (int y = 0; y < 5; y++)
    {
      tagComp = rfid.compareTagData(tagData, tagRef[x][y].tagData);
      delay(100);
      if (tagComp)
      {
        x_coord = x;
        x_coord = y;
      }
    }
  }// end loop
  
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

    if (inChar == 0x0d || inChar == 0x0a) {
      stringComplete = true;
    }
  }
}
