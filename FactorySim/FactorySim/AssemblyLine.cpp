#include "AssemblyLine.h"
#include <iostream>

void AssemblyLine::makePallet(int GlobalTime){
	if ( GlobalTime%prodCycle == 0 && !pallet ){
		pallet = true;
	//	std::cout << "PALLET MADE "<<ID << std::endl;
	}
	
}

void AssemblyLine::timeOccupied(){
	if (pallet){
		timeWaited++;
	}

	
}
	//This method is used to test out custom priority ideas.
void AssemblyLine::decide(int robotPos, int globalTime){
	if (!pallet){
		priority = (prodCycle - globalTime%prodCycle) + abs(location - robotPos);
	}
	else{
		priority = abs(location - robotPos) - 2*timeWaited;
	}



}