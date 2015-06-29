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

const char ID =  'B';  // Unique id for each robot

struct coords {
  int x = 5;
  int y = 5;
};

coords remote_pos[2][2]; // Initialize to number of robots in system
// Holds current pos at the top and target pos at the bottom

// used for xbee receive
String inputString = "";

coords currentPos;

int heading = 0;
byte tagData[5];                  //Holds the ID numbers from the tag
byte tagDataBuffer[5];
Tag tagRef[5][5];

char outBuff[14] = "rob0_pos:x,y";

void mapInit();
bool parseInput();
void get_pos();
int pickMove();
void assignPriority(int (&directions)[4]);
coords nearestUnvisited(coords pos);
void move(int targ_heading);
void serialEvent();
void quit();

int signum(int val);

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

  delay(500);
  pilot.resetMotors();
  delay(500);

  outBuff[3] = ID;
  //ID initial location
  bool initRead = true;

  while (initRead) {
    initRead = !(rfid.decodeTag(tagData));
  }

  rfid.transferToBuffer(tagData, tagDataBuffer);
  get_pos();

  nxshield.ledSetRGB(5, 0, 0);
}


void loop() {
  outBuff[9] = currentPos.x + '0';
  outBuff[11] = currentPos.y + '0';
  bool goodString = false;
  do{
    inputString = "";
  
    //Broadcast location
    Serial.println(outBuff);
    serialEvent();
  
    int count = 1;
    while (inputString.charAt(3) != ID) {
      inputString = "";
      serialEvent();
      if (count % 10000 == 0) {
        Serial.println(outBuff);
      }
      count++;
    }// end loop
  
    if (inputString.length() > 8)
     goodString = parseInput();

  }while(!goodString);

  //Pick move
  int targ_heading = pickMove();

  //Move
  move(targ_heading);
  rfid.transferToBuffer(tagData, tagDataBuffer);

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


bool parseInput() {
  Serial.println(inputString);
  int index = inputString.indexOf('_') + 1;
  if(index==0)
    return false;
  while (inputString.charAt(index)!= '\n') {
    int a = inputString.indexOf(':', index);
    int b = inputString.indexOf(',', a);
    if(a<0 || b<0)
      return false;
    remote_pos[inputString.charAt(index) - '0'][0].x = inputString.substring(a + 1, b).toInt();
   
    a = b;
    b = inputString.indexOf(':', a);
    if(b<0)
      return false;
    remote_pos[inputString.charAt(index) - '0'][0].y = inputString.substring(a + 1, b).toInt();
   
    a = b;
    b = inputString.indexOf(',', a);
    if(b<0)
      return false;
    remote_pos[inputString.charAt(index) - '0'][1].x = inputString.substring(a + 1, b).toInt();
    
    a = b;
    b = inputString.indexOf('_', a);
    if(b<0)
      return false;
    remote_pos[inputString.charAt(index) - '0'][1].y = inputString.substring(a + 1, b).toInt();
 
    index = b + 1;
  }

  return true;
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
        currentPos.x = x;
        currentPos.y = y;
        if (tagRef[x][y].isVisited())
          rfid.errorSound();
        else {
          tagRef[x][y].visit();
          rfid.successSound();
        }
        return;
      }
    }
  }// end loop

}


int pickMove() {
  int directions[4]; //indeces correspond to headings.

  assignPriority(directions);
  int targ_heading = -1;

  // look for adjacent unvisited position
  for (int i = 0; i < 4; i++) {
    if (directions[i] == 2)
      targ_heading = i;
  }

  // if no adjacent unvisited positions found
  if (targ_heading < 0) {
    coords dest = nearestUnvisited(currentPos);// find nearest unvisited position
    if (dest.x == 500)
      quit();

    int x_dif = dest.x - currentPos.x;
    int y_dif = dest.y - currentPos.y;

    if (abs(y_dif) > abs(x_dif)) {
      if (signum(y_dif) < 0 && directions[0] > 0 ) {
        targ_heading = 0;
      } else if (signum(y_dif) > 0 && directions[2] > 0) {
        targ_heading = 2;
      }
    } else if (abs(x_dif) > abs(y_dif) || abs(x_dif) == abs(y_dif)) {
      if (signum(x_dif) < 0 && directions[3] > 0) {
        targ_heading = 3;
      } else if (signum(x_dif) > 0 && directions[1] > 0) {
        targ_heading = 1;
      }
    }

    if(targ_heading<0){
      for(int i = 0; i < 4; i++){
        if(directions[i]>0){
          targ_heading = i;
          break;
        }
      } 
    }// end if targ_heading still equals -1
    
  }// end if targ_heading equals -1

  //Calculate and broadcast target position
  coords targPos;
  targPos.x = currentPos.x + (targ_heading % 2) * (2 - targ_heading);
  targPos.y = currentPos.y + ((targ_heading + 1) % 2) * ((targ_heading + 1) - 2);
  
  Serial.print("rob");
  Serial.print(ID);
  Serial.print("_targ:");
  Serial.print(targPos.x);
  Serial.print(",");
  Serial.println(targPos.y);

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
void assignPriority(int (&directions)[4]) {

  //Assign priorites
  for (int i = 0; i < 4; i++) {
    int x = currentPos.x + (i % 2) * (2 - i);
    int y = currentPos.y + ((i + 1) % 2) * ((i + 1) - 2);

    // if off the board
    if (x < 0 || x > 4 || y < 0 || y > 4) {
      directions[i] = 0;
      continue;
    }

    // if occupied
    bool gate = false;
    for (int r = 0; r < sizeof(remote_pos) / sizeof(coords[2]); r++) {
      for (int c = 0; c < 2; c++) {
        if (remote_pos[r][c].x == x && remote_pos[r][c].y == y){
          directions[i] = 0;
          gate = true;
        }
      }
    }

    if(gate)
      continue;

    // if already visited
    if (tagRef[x][y].isVisited()) {
      directions[i] = 1;
      continue;
    }

    // if unnoccupied and unvisited
    directions[i] = 2;

  }// end loop
}

/*
  Locates and returns the nearest unvisited tag to
  the robot's current position
*/
coords nearestUnvisited(coords pos) {
  coords dest;
  dest.x = 500;
  dest.y = 500;

  int minDist = 1000;

  for (int x = 0; x < 5; x++) {
    for (int y = 0; y < 5; y++) {
      if (!tagRef[x][y].isVisited()) {
        // Check if distance is smaller than current dest
        int dist = sqrt(sq(currentPos.x - x) + sq(currentPos.y - y));

        if (dist < minDist) {
          dest.x = x;
          dest.y = y;
          minDist = dist;
        }
      }// end if not visited
    }
  }// end outer loop
  return dest;
}

/*
  Takes in a target heading, turns the robot the
  appropriate amount, and moves straight alongh the
  grid till a new RFID is read. Sets tagData as a
  side effect.
*/
void move(int targ_heading) {
  int turn;
  // Calculate turn increment
  if (targ_heading == 0 && heading == 3)
    turn = 1;
  else if (targ_heading == 3 && heading == 0)
    turn = -1;
  else
    turn = targ_heading - heading; //negative is left turn, positive is right

  // Turn appropriate direction
  for (int i = 0; i < abs(turn); i++) {
    if (turn < 0)
      pilot.turnLeft();
    else if (turn > 0)
      pilot.turnRight();
  }

  byte tempTagData[5];
  // Travel straight to next position
  nxshield.bank_a.motorRunUnlimited(SH_Motor_Both, SH_Direction_Forward, 20);
  delay(500);
  while (rfid.compareTagData(tagData, tagDataBuffer)) {
    pilot.straight();

    if (rfid.decodeTag(tempTagData))
      rfid.transferToBuffer(tempTagData, tagData);

  }
  nxshield.bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
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

int signum(int val) {
  return ((0 < val) - (0 > val));
}

void quit() {
  Serial.print("rob");
  Serial.print(ID);
  Serial.println("_done");
  while (1) {}
}
