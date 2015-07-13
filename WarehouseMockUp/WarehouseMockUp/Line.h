#ifndef LINE_H_
#define LINE_H_

#include <chrono>
#include <ctime>


//Automatic production - Record a few ints, whenever the timer hits these numbers flip the pallet boolean. 
class Line{

	//Some regular timing method for production cycles, some way to count time holding package, allowing for multiple pallets
	//with an integer counter, 

public:
	int prodCycle, timeWaited, totalTimeWaited, priority, ID, pallet;
	std::clock_t start;
	double wait=0;

	Line(){
		prodCycle = 0;
		pallet = 0;
		this->ID = ID;
		totalTimeWaited = 0;
	}

	Line(int ID, int prod){
		prodCycle = prod;
		pallet = 0;
		this->ID = ID;
		totalTimeWaited = 0;
	}


	void makePallet(int Globaltime);
	double getWaitTime();
	int getPalletCount();
	void setPallet();
	void decide(int robotPos, int globalTime);
	void palletLoss();
	void timer();

};



#endif 
