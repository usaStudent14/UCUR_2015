#include "AssemblyLine.h"
#include "Robots.h"

#include <iostream>
using namespace std;


static AssemblyLine A(58, 0);
static AssemblyLine B(84, 1);
static AssemblyLine C(117, 2);
static AssemblyLine D(142, 3);
static AssemblyLine E(159, 4);
static AssemblyLine F(186, 5);

static Robots* robota = new Robots();

static AssemblyLine array[6] = { A, B, C, D, E, F };

static int docLoc = 375;
static int workingDay = 120000;



//This has flexible inner code
void RobotDecide(){
	
	if (robota->cargo){
		robota->target = docLoc;
		return;
	}

	int maxPriority = 10000;
	int index=0;
	for (AssemblyLine& line : array){
		
		if (line.pallet && line.priority < maxPriority){
			//cout << line.ID << " READY";
			index = line.location;
			maxPriority=line.priority;
		}

	}
	
	robota->target = index;
	//cout << "ROBOT HEADING FOR " << index << endl;
}

int main(){

	int a=0, b=0, c=0, d=0, e=0, f=0, g=0;

	srand(time(NULL));
	for (int u = 0; u < 100; u++){
		
		for (AssemblyLine& line : array){
			line.prodCycle =rand() % 1000 + 4500;
		}



		//cout << "FACTORY TEST PROGRAM\n" << endl;
		for (AssemblyLine& line : array){
			//cout << line.ID<<" "<<line.prodCycle << endl;
		}


		for (int x = 1; x < workingDay; x++){
			//if (x == 100){ getchar(); }
			//produce things, work out priorities, 
			for (AssemblyLine& line : array){
				line.makePallet(x);
				line.decide(robota->location, x);
			}
			//fine so far

			
				RobotDecide();
			

			//settle the robots target, move it,
			if (robota->target >= 0)
				robota->motion();



			if (robota->location == robota->target){
				for (AssemblyLine& line : array){
					if (line.location == robota->target){
						robota->pickup(line);
					}
				}
			}

			if (robota->location == docLoc&&robota->cargo){
				robota->deliver();
			}

			//settle time waited in assemblies
			for (AssemblyLine& line : array){
				line.timeOccupied();
			}


		}

		//cout << "\nNUMBER OF PALLETS DELIVERED " << robota->totalDeliveries << endl;

		a += robota->totalDeliveries;
		b += array[0].totalTimeWaited;
		c += array[1].totalTimeWaited;
		d += array[2].totalTimeWaited;
		e += array[3].totalTimeWaited;
		f += array[4].totalTimeWaited;
		g += array[5].totalTimeWaited;
		for (AssemblyLine& line : array){
		//	cout << line.ID << " " << line.totalTimeWaited << endl;
			line.totalTimeWaited = 0;
			line.timeWaited = 0;
			line.priority = 0;
			line.pallet = false;
		}
		robota->totalDeliveries = 0;
		robota->location = 0;
		robota->cargo = false;
		robota->target = 0;



	}
	int check = 100;
	cout << "Total packages delivered: " << a / check << " \n" 
		<< b / check << "\n" << c / check << "\n" << d / check << "\n" << e / check << "\n" << f / check << "\n" << g / check << "\n";


	getchar();
}


