#include <tchar.h>
#include "SerialClass.h"	// Library described above
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>

using namespace std;

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{
	struct coords{
		int x;
		int y;
	};

	coords robPos[4][2]; //Hardcode to number of bots + 2(since we can't use 0 or 1)

	// Fill coordinate array
	for(int i = 0; i < sizeof(robPos)/sizeof(coords[2]);i++){
		for(int j = 0; j < 2; j++){
			robPos[i][j].x = 0;
			robPos[i][j].y = 0;
		}
	}

	fstream fileOut1("data1.txt", ios::out);

	Serial* SP = new Serial("COM5");    // adjust as needed

	if (SP->IsConnected())
		printf("System channel up...\n");

	string datastr = "";
	bool stringComplete = false;

	int robCount = 2;// number of robots
	int doneCount = 0;// number of robots that have completed the tour

	while(doneCount < robCount && SP->IsConnected())
	{	
		Sleep(10);

		char data[3] = "";
		SP->ReadData(data, 1);

		if(data[0] == '\n'){
			stringComplete = true;
			cout << datastr << endl;

		}

		datastr += data;

		if(stringComplete && datastr.find("rob")!=string::npos){
			int id = datastr.at(3) - '0';// convert robot ID to int

			// If position report
			if(datastr.find("pos")!=string::npos){
				//Store data
				fileOut1.write(datastr.c_str(), datastr.length());// file
				robPos[id][0].x = datastr.at(9)-'0';
				robPos[id][0].y = datastr.at(11)-'0';

				//Reply with most recent position data
				//Format: "sys<recipientID>rp_<remoteID>:0,0(current pos):0,0(target pos)_<remoteID>..._\n"
				char reply[100];
				reply[0] = 's';
				reply[1] = 'y';
				reply[2] = 's';
				reply[3] = id + '0';
				reply[4] = 'r';
				reply[5] = 'p';
				reply[6] = '_';
				int rchar = 7;
				for(int i = 2; i < (sizeof(robPos)/sizeof(coords[2])); i++){
					if(i != id){
						reply[rchar++] = i + '0';
						reply[rchar++] = ':';
						reply[rchar++] = robPos[i][0].x + '0';
						reply[rchar++] = ',';
						reply[rchar++] = robPos[i][0].y + '0';
						reply[rchar++] = ':';
						reply[rchar++] = robPos[i][1].x + '0';
						reply[rchar++] = ',';
						reply[rchar++] = robPos[i][1].y + '0';
						reply[rchar++] = '_';
					}
				}// end loop
				reply[rchar++] = '\n';
				//cout << "\t" << reply << endl;
				SP->WriteData(reply, rchar);
				cout << "\a";

				// If target position report
			}else if(datastr.find("targ")!=string::npos){
				// store  data
				robPos[id][1].x = datastr.at(10)-'0';
				robPos[id][1].y = datastr.at(12)-'0';

			// If completion report
			}else if(datastr.find("done")!=string::npos){
				// Evaluate system completion
				doneCount++;

			}

			stringComplete = false;
			datastr = "";
		}// end if complete message starting with "rob"

	}// end loop

	fileOut1.close();

	getchar();
	return 0;
}

