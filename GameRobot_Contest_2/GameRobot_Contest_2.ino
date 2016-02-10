// Authors: Ed Baker, Alex Henderson, Jake Maynard
#include <QueueArray.h>

#include <NXShield.h>
#include <Wire.h>
#include <NXTLight.h>
#include <RFIDuino.h>
#include "tag.h"
#include "pilot.h"

NXShield nxshield;
NXTLight light1;
NXTLight light2;
RFIDuino rfid;

PILOT pilot(nxshield, light1, light2, rfid);



struct coords {
  int x = 5;
  int y = 5;

  coords operator=(const coords& o) {
    this->x = o.x;
    this->y = o.y;
    return *this;
  }

};


const char ID = 'A';              // Unique id for each robot
coords remote_pos[2][2];          // Initialize to number of robots in system
// Holds current pos at the top and target pos at the bottom
QueueArray<coords> moveList;      //List of robot moves, used for backtracking
int robNum = 2;                   // Number of robots
String inputString = "";          // used for xbee recieve
coords currentPos;                // Current position
int heading = 0;                  // Initial heading of robot
byte tagData[5];                  // Holds the ID numbers from the tag
byte tagDataBuffer[5];            // Holds last read tag data
Tag tagRef[5][5];                 // Map
char outBuff[14] = "rob0_pos:x,y";// Default position report message

void mapInit();
bool parseInput();
void get_pos();
int nextMove();
void findPath(); //cut?
void move(int targ_heading);
void serialEvent();
void quit(); //cut functionality

int signum(int val);


void setup() {

  Serial.begin(9600);
  delay(500);
  nxshield.init(SH_HardwareI2C);
  nxshield.waitForButtonPress(BTN_GO);

  mapInit();


  bool initRead = true;

  outBuff[3] = ID;
  //ID initial location


  while (initRead) {
    initRead = !(rfid.decodeTag(tagData));
  }
  rfid.successSound();

  rfid.transferToBuffer(tagData, tagDataBuffer);
  get_pos();

  inputString.reserve(50);

  light1.init(&nxshield, SH_BAS2);
  light2.init(&nxshield, SH_BBS2);

  light1.setReflected();
  light2.setReflected();

  delay(500);
  pilot.resetMotors();
  delay(500);


  nxshield.ledSetRGB(5, 0, 0);

}


void loop() {


  outBuff[9] = currentPos.x + '0';
  outBuff[11] = currentPos.y + '0';
  bool goodString = true;

  do {
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

  } while (!goodString);


  char shakeBuf[7] = "rob_p";
  shakeBuf[3] = ID;
  Serial.println(shakeBuf);


  //Pick move
  int targ_heading = nextMove();

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

  int index = inputString.indexOf('_') + 1;
  if (index == 0)
    return false;

  if ((inputString.charAt(index) - '0') < 0 || (inputString.charAt(index) - '0') >= robNum ) {
    return false;
  }

  while (inputString.charAt(index) != '\n') {
    int a = inputString.indexOf(':', index);
    int b = inputString.indexOf(',', a);
    if (a < 0 || b < 0)
      return false;
    // Set current position
    int posX = inputString.substring(a + 1, b).toInt();

    a = b;
    b = inputString.indexOf(':', a);
    if (b < 0)
      return false;
    int posY = inputString.substring(a + 1, b).toInt();
    // Adjust availablilty

    if (posX >= 0 && posX <= 4 && posY >= 0 && posY <= 4) {
      tagRef[posX][posY].setAvailable();
    }
    remote_pos[inputString.charAt(index) - '0'][0].x = posX;
    remote_pos[inputString.charAt(index) - '0'][0].y = posY;

    a = b;
    b = inputString.indexOf(',', a);
    if (b < 0)
      return false;

    // Set target
    int targX = inputString.substring(a + 1, b).toInt();


    a = b;
    b = inputString.indexOf('_', a);
    if (b < 0)
      return false;
    int targY = inputString.substring(a + 1, b).toInt();


    remote_pos[inputString.charAt(index) - '0'][1].x = targX;
    remote_pos[inputString.charAt(index) - '0'][1].y = targY;

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
        tagRef[x][y].setVisited();
        currentPos.x = x;
        currentPos.y = y;
        return;
      }
    }
  }// end loop
}


int nextMove() {


  coords next;
  next.x = 5;
  next.y = 5;


  int enX, enY, usX, usY;
  //int tempHeading = -1;

  coords enemyPos;
  enemyPos.x = 5;
  enemyPos.y = 5;

  int enemyID;

  if (ID == 'A') {
    enemyID = 1;
  }
  else {
    enemyID = 0;
  }

  enemyPos.x = remote_pos[enemyID][0].x;
  enemyPos.y = remote_pos[enemyID][0].y;

  if (enemyPos.x > 2) {
    enX = 4;
  }
  else {
    enX = 0;
  }

  if (enemyPos.y > 2) {
    enY = 4;
  }
  else {
    enY = 0;
  }

  usX = abs(enX - currentPos.x);
  usY = abs(enY - currentPos.y);

  if (usX > usY) {
    //heading = abs(enX - 1);
    next.x = currentPos.x + signum(enX);
    next.y = currentPos.y;

    if ( abs(currentPos.x - enemyPos.x) == 1 || !tagRef[next.x][next.y].isAvailable() || next.x > 4 || next.x < 0 ) {
      next.x = currentPos.x;
      next.y = currentPos.y - signum(currentPos.y - 2);
    }

    if ( tagRef[next.x][next.y].isVisited() || !tagRef[next.x][next.y].isAvailable() || next.y > 4 || next.y < 0) {
      next.y = currentPos.y + signum(currentPos.y - 2);
      next.x = currentPos.x;
    }




  }
  else {
    //heading = enY / 2;

    next.y = currentPos.y + signum(enY);
    next.x = currentPos.x;

    if ( abs(currentPos.y - enemyPos.y) == 1 || !tagRef[next.x][next.y].isAvailable() || next.y > 4 || next.y < 0 ) {
      next.y = currentPos.y;
      next.x = currentPos.x - signum(currentPos.x - 2);
    }

    if ( tagRef[next.x][next.y].isVisited() || !tagRef[next.x][next.y].isAvailable() || next.x > 4 || next.x < 0) {
      next.x = currentPos.x + signum(currentPos.x - 2);
      next.y = currentPos.y;
    }

  }


  if (next.x > 4 || next.x < 0 || next.y > 4 || next.y < 0 || !tagRef[next.x][next.y].isAvailable() ) {

    for (int i = 0; i < 4; i++) {
      next.x = currentPos.x + (i % 2) * (2 - i);
      next.y = currentPos.y + ((i + 1) % 2) * ((i + 1) - 2);

      if (next.x <= 4 && next.x >= 0 && next.y <= 4 && next.y >= 0 && tagRef[next.x][next.y].isAvailable()) {
        break;
      }

      if (i == 3) {
        Serial.println("There are no available moves! Error!");
      }
    }


  }



  char targBuff[15] = "roba_targ:0,0";
  targBuff[3] = ID;
  targBuff[10] = next.x + '0';
  targBuff[12] = next.y + '0';


  inputString = "";
  //Broadcast target
  Serial.println(targBuff);
  serialEvent();

  int count = 1;
  while (inputString.charAt(0) != 's' || inputString.charAt(3) != ID || inputString.charAt(4) != 't' ) {
    inputString = "";
    serialEvent();
    if (count % 20000 == 0) {
      Serial.println(targBuff);
    }
    count++;
  }// end loop


  char shakeBuf[7] = "rob_t";
  shakeBuf[3] = ID;
  Serial.println(shakeBuf);



  //& is go, * is wait
  if (inputString[4] != '*') {
    next.x = moveList.peek().x;
    next.y = moveList.peek().y;


  }


  // Calculate target heading
  int xDiff = next.x - currentPos.x;
  int yDiff = next.y - currentPos.y;
  int targ_heading = -1;

  if (xDiff == 0) {
    if (yDiff > 0)
      targ_heading = 2;
    else
      targ_heading = 0;
  } else if (yDiff == 0) {
    if (xDiff > 0)
      targ_heading = 1;
    else
      targ_heading = 3;
  }


  moveList.push(currentPos);



  if (targ_heading < 0 || targ_heading > 3) {
    rfid.errorSound();
  }


  return targ_heading;

}



void printQueue() {
  QueueArray<coords> temp;

  while (!temp.isEmpty()) {
    //Serial.print(temp.peek().x);
    //Serial.print(", ");
    //Serial.println(temp.peek().y);
    temp.pop();
  }

}

/*
void findPath() {
  // targets.printVector();

  if (!targets.findClosest(currentPos)) {
    quit();
  }

  rfid.errorSound();
  QueueArray<coords> newPath;
  coords dest;
  dest.x = 0;//targets.getCoords().x;
  dest.y = 0;//targets.getCoords().y;
  coords next;
  next.x = currentPos.x;
  next.y = currentPos.y;

  newPath.push(next);

  while (next.x != dest.x || next.y != dest.y) {

    int x_dif = dest.x - next.x;
    int y_dif = dest.y - next.y;
    coords temp;
    temp.x = next.x;
    temp.y = next.y;

    if (abs(x_dif) > abs(y_dif)) {
      temp.x = temp.x + signum(x_dif);
    } else {
      temp.y = temp.y + signum(y_dif);
    }

    // the first step must be available
    if (newPath.count() == 1 && !tagRef[temp.x][temp.y].isAvailable()) {
      //Serial.println("Picking alternate path");
      temp.x = next.x;
      temp.y = next.y;
      if (abs(x_dif) <= abs(y_dif)) {
        temp.x = next.x + signum(x_dif);
        if (temp.x < 0 || temp.x > 4) { // if off the board
          temp.x = next.x - signum(x_dif);
        }
      } else {
        temp.y = next.y + signum(y_dif);
        if (temp.y < 0 || temp.y > 4) { // if off the board
          temp.y = next.y - signum(y_dif);
        }
      }
    }// end if first step unavailable

    //Serial.print("(");
    //Serial.print(temp.x);
    //Serial.print(",");
    //Serial.print(temp.y);
    //Serial.print(")-");
    newPath.push(temp);
    next.x = temp.x;
    next.y = temp.y;
  }// end loop

  //Serial.println();
  path = newPath;


}

*/
/*
  Takes in a target heading, turns the robot the
  appropriate amount, and moves straight along the
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

    if (rfid.decodeTag(tempTagData)) {
      //rfid.successSound();
      rfid.transferToBuffer(tempTagData, tagData);
    }
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
  return (int)((0 < val) - (0 >= val));
}

/**
 * Sends a message to the channel indicating this unit
 * has completed its tour, then enters a read routine
 * that identifies messages sent from any units still
 * in the tour process. This data is used to enable this
 * unit to maneuver out of the way of any approaching units.
 */
void quit() {


  outBuff[9] = currentPos.x + '0';
  outBuff[11] = currentPos.y + '0';
  Serial.println(outBuff);
  Serial.print(F("rob"));
  Serial.print(ID);
  Serial.println(F("_done"));
  char targBuff[15] = "roba_targ:5,5";
  targBuff[3] = ID;

  int id = -1;
  while (1) {
    while (1) {
      inputString = "";
      serialEvent();
      id = inputString.charAt(3) - 'A';

      while (id < 0 || id > robNum) {
        inputString = "";
        serialEvent();
        id = inputString.charAt(3) - 'A';
      }// end loop

      if (inputString.charAt(5) == 'p') {
        remote_pos[id][0].x = inputString.charAt(9) - '0';
        remote_pos[id][0].y = inputString.charAt(11) - '0';
      }
      else if (inputString.charAt(5) == 't') {
        remote_pos[id][1].x = inputString.charAt(10) - '0';
        remote_pos[id][1].y = inputString.charAt(12) - '0';
        break;
      }
    }// end inner loop

    coords remoteTarg;
    remoteTarg.x = inputString.charAt(10) - '0';
    remoteTarg.y = inputString.charAt(12) - '0';
    coords remotePos;
    remotePos.x = remote_pos[id][0].x;
    remotePos.y = remote_pos[id][0].y;

    int xdif = abs(remoteTarg.x - currentPos.x);
    int ydif = abs(remoteTarg.y - currentPos.y);

    if ( (xdif <= 1 && ydif == 0) || (ydif <= 1 && xdif == 0) ) {
      int targHeading = 10;

      for (int i = 0; i < 4; i++) {
        int x = currentPos.x + (i % 2) * (2 - i);
        int y = currentPos.y + ((i + 1) % 2) * ((i + 1) - 2);

        // if off the board
        if (x < 0 || x > 4 || y < 0 || y > 4) {
          continue;
        } else if (x == remoteTarg.x && y == remoteTarg.y) {
          continue;
        } else {
          targHeading = i;
          break;
        }
      }

      move(targHeading);
      rfid.transferToBuffer(tagData, tagDataBuffer);

      // update heading
      heading = targHeading;

      //ID new location
      get_pos();

    }// end if target equals position

    Serial.println(targBuff);
  }// end loop

}
