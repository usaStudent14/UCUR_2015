#include <stdio.h>
#include <tchar.h>
#include "SerialClass.h"	// Library described above
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{

	fstream fileOut1("data1.txt", ios::out);
	fstream fileOut2("data2.txt", ios::out);

	Serial* SP = new Serial("COM5");    // adjust as needed

	if (SP->IsConnected())
		printf("We're connected\n");

	string datastr = "";
	bool stringComplete = false;
	int count = 0;

	while(count < 100 && SP->IsConnected())
	{	
		Sleep(10);

		char data[3] = "";

		SP->ReadData(data, 1);

		if(data[0] == '\n')
			stringComplete = true;

		datastr += data;

		//if(datastr != "")
			//cout << datastr << endl;

		if(stringComplete && datastr.find("rob")!=string::npos){
			int id = datastr.at(3) - '0';// convert robot ID to int
			cout << datastr << endl;

			// If position report
			if(datastr.find("pos")!=string::npos){
				//Store data

				//Reply with most recent position data

				//Format: "sys<recipientID>rp_<remoteID>:0,0(current pos):0,0(target pos)_<remoteID>..._\n"
			}

			// If completion report
			else if(datastr.find("done")!=string::npos){
				// Evaluate system completion
			}

			fileOut1.write(datastr.c_str(), datastr.length());

			stringComplete = false;
			datastr = "";
			count++;

			char confirm[10] = "sys";
			confirm[3] = id;
			SP->WriteData(confirm, 5);
		}

	}
	fileOut1.close();
	fileOut2.close();
	getchar();
	return 0;
}

