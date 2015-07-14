#include "Line.h"
#include <iostream>

void Line::makePallet(int Globaltime){
	if (Globaltime%prodCycle == 0){
		std::cout << '\a';
		
		if (++pallet == 1){
			start = std::clock();
		}
		std::cout << "INCREMENT " <<ID<<" "<< pallet << '\n';
	}

}

double Line::getWaitTime(){
	return std::clock()-start/(double)(CLOCKS_PER_SEC);
}
int Line::getPalletCount(){
	return pallet;
}
void Line::setPallet(){
	pallet = 2;
	start = std::clock();
}

void Line::decide(int robotPos, int globalTime){

}

void Line::palletLoss(){
	if (pallet > 0){
		
		if (--pallet == 0){
			wait+=std::clock() - start / (double)(CLOCKS_PER_SEC);
		}
		std::cout << "DECREMENT " << ID << " " << pallet << '\n';
	}
}

void Line::timer(){}
