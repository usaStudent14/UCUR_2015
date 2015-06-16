#ifndef ROBOT_H
#define ROBOT_H
#include "AssemblyLine.h"

class Robots{
	public:
		int location, target, totalDeliveries;
		bool cargo;

		Robots(){
			totalDeliveries = 0;
			location = 0;
			cargo = false;
			target = 0;

		}

		void motion();
		void pickup(AssemblyLine& ehh);
		void deliver();
		//void decide();

};




#endif