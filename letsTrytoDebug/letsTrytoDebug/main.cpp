// Authors: Ed Baker, Alex Henderson, Jake Maynard
#include "QueueArray.h"
#include<iostream>

using namespace std;


struct coords {
	int x = 5;
	int y = 5;
	/*
	coords operator=(const coords& o){
		this->x = o.x;
		this->y = o.y;
		return *this;
	}
	*/
};



const char ID = 'B';             // Unique id for each robot
coords remote_pos[2][2];  
// Initialize to number of robots in system
// Holds current pos at the top and target pos at the bottom
QueueArray<coords> targets;       // Destinations, in order, of the robot
QueueArray<coords> path;         
// Current path of the robot

int robNum = 2;                   // Number of robots
//String inputString = "";          // used for xbee recieve
coords currentPos;                // Current position
int heading = 0;                  // Initial heading of robot
char outBuff[14] = "rob0_pos:x,y";// Default position report message

void mapInit();
void targInit();
bool parseInput();
void get_pos();
int nextMove();
void findPath();
void move(int targ_heading);
void serialEvent();
void quit();
void printArray();

int signum(int val);

void main() {
	currentPos.x = 1;
	currentPos.y = 1;
	targInit();
	
	findPath();// initial path
	findPath();
	findPath();

	getchar();
}


void printArray(){
	for (int x = 0; x<2; x++){
		for (int y = 0; y<2; y++){
			printf("%d",remote_pos[x][y].x);
			printf(", ");
			printf("%d",remote_pos[x][y].y);
		}
	}

}

void targInit(){

	coords targ;
	targ.x = 4;
	targ.y = 4;
	targets.push(targ);

	targ.x = 1;
	targ.y = 3;
	targets.push(targ);

	targ.x = 4;
	targ.y = 2;
	targets.push(targ);

}


void printQueue(){
	QueueArray<coords> temp = path;
	while (!temp.isEmpty()){
		printf("%d",temp.peek().x);
		printf(", ");
		printf("%d",temp.peek().y);
		printf("\n");
		temp.pop();
	}

}


void findPath(){

	//rfid.errorSound();
	QueueArray<coords> newPath;
	coords dest;
	dest.x = targets.peek().x;
	dest.y = targets.peek().y;
	coords next;
	next.x = currentPos.x;
	next.y = currentPos.y;

	newPath.push(next);

	while (next.x != dest.x || next.y != dest.y){

		int x_dif = dest.x - next.x;
		int y_dif = dest.y - next.y;
		coords temp;
		temp.x = next.x;
		temp.y = next.y;

		if (abs(x_dif)>abs(y_dif)){
			temp.x = temp.x + signum(x_dif);
		}
		else{
			temp.y = temp.y + signum(y_dif);
		}

		// the first step must be available
		if (0){
			//Serial.println("Picking alternate path");
			temp = next;
			if (abs(x_dif)<abs(y_dif)){
				temp.x = next.x + signum(x_dif);
				if (temp.x<0 || temp.x>4){// if off the board
					temp.x = next.x - signum(x_dif);
				}
			}
			else{
				temp.y = next.y + signum(y_dif);
				if (temp.y<0 || temp.y>4){// if off the board
					temp.y = next.y - signum(y_dif);
				}
			}
		}// end if first step unavailable

		printf("(");
		printf("%d", temp.x);
		printf(",");
		printf("%d",temp.y);
		printf(")-");
		newPath.push(temp);
		next.x = temp.x;
		next.y = temp.y;
	}// end loop

	printf("\n");
	path = newPath;
	printQueue();

}

int signum(int val) {
	return (int)((0 < val) - (0 >= val));
}
