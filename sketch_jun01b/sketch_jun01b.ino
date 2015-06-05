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

#define ID 0  //Unique id for each robot

struct coords{
  int x = 0;
  int y = 0;
};

coords remote_pos[2]; // Initialize to number of robots in system

// used for xbee receive
String inputString = "";
boolean stringComplete = false;

int x_coord;
int y_coord;

int heading = 1;
byte tagData[5];                  //Holds the ID numbers from the tag
byte tagDataBuffer[5];
Tag tagRef[5][5];

void mapInit();
void parseInput();
void get_pos();
int pickMove();
void assignPriority(int (&directions)[4]);
void move(int targ_heading);
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
  bool initRead = true;
  while(initRead){
    initRead = !rfid.decodeTag(tagData);
  }
  memcpy(tagDataBuffer, tagData, 5);
  get_pos();  

  nxshield.ledSetRGB(5, 0, 0);
}

void loop() {

  //Broadcast location
  Serial.print("rob_");
  Serial.print(ID);
  Serial.print(" ");
  Serial.print(x_coord);
  Serial.print(" ");
  Serial.println(y_coord);
    
  serialEvent();
  
  //Pick move
  int targ_heading = pickMove();
  
  serialEvent();
  
  //Move
  move(targ_heading);
  memcpy(tagDataBuffer, tagData, 5);
  
  // update heading
  heading = targ_heading;
  
  //ID new location
  get_pos();
  
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


void parseInput(){
  
   //Recieve remote robot position
  if(inputString.startsWith("rob_")){
    int remoteID = inputString.substring(4, 5).toInt();
    if(remoteID != ID){
      remote_pos[remoteID].x = inputString.substring(6, 7).toInt();
      remote_pos[remoteID].y = inputString.substring(8, 9).toInt();
      Serial.print("Read rob_");
      Serial.print(remoteID);
      Serial.print(" at ");
      Serial.print(remote_pos[remoteID].x);
      Serial.print(" ");
      Serial.println(remote_pos[remoteID].y);
      inputString = "";
    }
  }
  else if(inputString.startsWith("cmd_stop"))
      while(1){}
}

/*
 Identifies the grid position
 corresponding to current tag data
 */
void get_pos() {
  bool tagComp = false;
  for (int x = 0; x < 5; x++)
  {
    for (int y = 0; y < 5; y++)
    {
      tagComp = rfid.compareTagData(tagData, tagRef[x][y].tagData);
      if (tagComp)
      {
        x_coord = x;
        y_coord = y;
        tagRef[x][y].visit();
        return;
      }
    }
  }// end loop
  
}


int pickMove(){
  int directions[4]; //indeces correspond to headings.
  
  assignPriority(directions);
  int targ_heading = 0;
  
  // find heading with highest priority
  for(int i = 1; i < 4; i++){
    if(directions[i] > directions[targ_heading])
       targ_heading = i; 
  }
  return targ_heading; 
}

/*
  Uses a numbering priority scheme to select the best
  position to travel to next, or will timeout if no
  match found. Each adjacent position will be assigned 
  the appropriate priority: 0 if occupied or soon to be
  occupied, 1 if unoccupied and already visited, 2 if 
  unoccupied and unvisited.
*/
void assignPriority(int (&directions)[4]){
  
  //Assign priorites
  for(int i = 0; i < 4; i++){
    int x = x_coord + (i%2)*(2-i);
    int y = y_coord + ((i+1)%2)*((i+1)-2);
    
    // if off the board
    if(x<0 || x>4 || y<0 || y>4){
      directions[i] = 0;
      continue;
    }
    
    // if occupied
    for(int r = 0; r < sizeof(remote_pos)/sizeof(coords); r++){
      if(remote_pos[r].x == x && remote_pos[r].y == y)
        directions[i] = 0;
        continue;
    }
    
    // if already visited
    if(tagRef[x][y].isVisited()){
      directions[i] = 1;
      continue; 
    }
      
    // if unnoccupied and unvisited
    directions[i] = 2;
      
  }// end loop
}

/*
  Takes in a target heading, turns the robot the
  appropriate amount, and moves straight alongh the
  grid till a new RFID is read. Sets tagData as a 
  side effect.
*/
void move(int targ_heading){
  int turn;
  // Calculate turn increment
  if(targ_heading == 0 && heading == 3)
    turn = 1;
  else if(targ_heading == 3 && heading == 0)
    turn = -1;
  else
    turn = targ_heading - heading; //negative is left turn, positive is right
  
  // Turn appropriate direction
  for(int i = 0; i < abs(turn); i++){
    if(turn < 0)
      pilot.turnLeft();
    else if (turn > 0)
      pilot.turnRight(); 
  }
  
  // Travel straight to next position
  pilot.straight(tagData, tagDataBuffer);
  
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
  
  parseInput();
  inputString = "";
}
