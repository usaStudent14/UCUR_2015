#include <tchar.h>
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>
#include "Line.h"
#include "SerialClass.h"
#include <chrono>
#include <ctime>

using namespace std;


static Line Gfloor[5] = { Line(0, 15000), Line(1, 33000), Line(2, 38000), Line(3, 27465), Line(4, 23000) };



int deciding(){
	double highWait=0;
	int id=9;

	for (Line& o : Gfloor){
		if (o.getPalletCount() > 0){
			
			if (o.getWaitTime() > highWait){
				highWait = o.getWaitTime();
				id = o.ID;
			}
		}
	}

	return id;
}
//add a global timer? thread


int _tmain(int argc, _TCHAR* argv[]){
	//Gfloor[4].setPallet();
	int globalTimer=1;
	int robotpos = 5;
	int robottar=9;
	bool loaded=false;
	int packagesDelivered=0;
	


	Serial* SP = new Serial("COM8");    // adjust as needed

	char buffer[2] = "";

	if (SP->IsConnected()){
		printf("System channel up...\n");
	}


	//initial calling of deciding method to start with a target
	//robottar = deciding(robotpos);

	for (globalTimer; globalTimer < 100000;globalTimer++)
	{
		cout << globalTimer << endl;
		//Check the assembly lines before reading 
		Sleep(10);

		for (Line& o : Gfloor){
			o.makePallet(globalTimer);
		}
	

		char data[2] = "";
		SP->ReadData(data, 1);
		
		if (data[0] != '\0'){
			if (data[0] - '0' == 8){
				getchar();
			}
			else{
				robotpos = data[0] - '0';
				//cout << "robotpos=" << robotpos << " and data[0] was " << data[0] << endl;
			}
			
		}

			if (robotpos == 6||robottar==9){
				loaded = false;
				robottar=deciding();
			}

			if (robotpos == 5 && !loaded){
				buffer[0] = robottar + '0';
				//cout <<"BUFFER"<< buffer << endl;
				SP->WriteData(buffer, 1);		
			}

			if (robottar == robotpos && !loaded){
				cout <<"TARGET: "<< robottar << endl;
				loaded = true;
				Gfloor[robottar].palletLoss();
				packagesDelivered++;
				for (Line& o : Gfloor){
					cout << o.ID << " " << o.getPalletCount() << endl;
				}
			}


			
			
		
		//cout <<"TARGET: "<< robottar << '\n';
	}
	cout << '\a\a\a' << endl;
	cout << "PACKAGES DELIVERED: " << packagesDelivered<<'\n';
	for (Line& o : Gfloor){
		cout<<"LINE "<<o.ID<<" wait time is "<<o.wait<<" seconds.\n";
	}
	getchar();
}