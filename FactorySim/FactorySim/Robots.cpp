#include "Robots.h"
#include <iostream>


void Robots::motion(){
	if (location < target){
		location++;
	}
	else if (location > target){
		location--;
	}
	else if (location == target){
		target = 0;
		//Stay still, we assume target changes.
	}
	//std::cout << "ROBOT AT " << location << std::endl;
}

void Robots::pickup(AssemblyLine& line){
	//std::cout << "PICKUP AT " << line.ID << std::endl;
	if(line.pallet && !cargo){
		line.pallet = false;
		cargo = true;
		line.totalTimeWaited += line.timeWaited;
		line.timeWaited = 0;
	}
	
}
void Robots::deliver(){
	if (cargo){
		cargo = false;
		totalDeliveries++;
		//target = 0;
	}


}

//Blank space for testing
//void Robot::decide(){}