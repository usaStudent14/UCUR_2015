#ifndef ASSEMBLYLINE_H
#define ASSEMBLYLINE_H

#include <stdio.h>      
#include <stdlib.h>    
#include <time.h>  
#include <random>

class AssemblyLine{

public:
	int location, prodCycle, timeWaited, totalTimeWaited, priority, ID;
	bool pallet;
	

	AssemblyLine(int location, int ID){
		std::default_random_engine generator;
		std::uniform_int_distribution<int> dist(4500, 5500);

		srand((unsigned)time(NULL));
		prodCycle = dist(generator);//rand() %1000+4500;
		pallet = false;
		this->location = location;
		this->ID = ID;
		totalTimeWaited = 0;
	}


	void makePallet(int Globaltime);
	void timeOccupied();
	void decide(int robotPos, int globalTime);


};



#endif 