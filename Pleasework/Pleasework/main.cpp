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

	int count = 0;
	char newLine[2] = "\n";
	while(count < 20 && SP->IsConnected())
	{	
		string type;
		//getline(cin, type);
		Sleep(10);

		char data[200] = "";

		SP->ReadData(data, 20);

		string datastr = data;
		if(datastr != "")
			cout << datastr << endl;

		if(datastr.find("rob1")!=string::npos){
			//cout << datastr << endl;
			fileOut1.write(data, datastr.length());
			fileOut1.write(newLine, 1);
			count ++;
	}

		if(datastr.find("rob2")!=string::npos){
			//cout << datastr << endl;
			fileOut2.write(data, datastr.length());
			fileOut1.write(newLine, 1);
			count ++;
		}

	}
	fileOut1.close();
	fileOut2.close();
	getchar();
	return 0;
}

