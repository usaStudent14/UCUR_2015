#include <tchar.h>
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>
#include "SerialClass.h"	// Library described above
#include "Robot.h"

using namespace std;

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{
	Robot robs[1];// Hardcode to number of bots

	fstream fileOut1("data1.txt", ios::out);

	Serial* SP = new Serial("COM5");    // adjust as needed

	if (SP->IsConnected())
		printf("System channel up...\n");

	string datastr = "";
	bool stringComplete = false;

	int robCount = 1;// number of robots
	int doneCount = 0;// number of robots that have completed the game
	int synchCount = 0;// number of robots waiting to synch target data
	int callID = -1;

	while(doneCount < robCount && SP->IsConnected())
	{	
		Sleep(10);

		char data[2] = "";
		SP->ReadData(data, 1);

		if(data[0] == '\n'){
			stringComplete = true;
			cout << datastr << endl;

		}

		datastr += data;

		if(stringComplete){
			if(datastr.find("rob")!=string::npos){
				char idChar = datastr.at(3);
				int id = idChar - 'A';
				

				// If position report
				if(datastr.find("pos")!=string::npos){
					coords tempPos;
					tempPos.x = datastr.at(9)-'0';
					tempPos.y = datastr.at(11)-'0';

					// if not a duplicate
					if(tempPos.x!=robs[id].getPos().x && tempPos.y!=robs[id].getPos().y){
						//Store data
						fileOut1.write(datastr.c_str(), datastr.length());// file
						robs[id].setPos(tempPos);
						robs[id].incrementMoves();
					}// end if duplicate

					//Reply with most recent position data
					//Format: "sys<recipientID>rp_<remoteID>:0,0(current pos):0,0(target pos)_<remoteID>..._\n"
					char reply[100];
					reply[0] = 's';
					reply[1] = 'y';
					reply[2] = 's';
					reply[3] = idChar;
					reply[4] = 'r';
					reply[5] = 'p';
					reply[6] = '_';
					int rchar = 7;
					for(int i = 0; i < (sizeof(robs)/sizeof(Robot)); i++){
						if(i != id){
							reply[rchar++] = i + '0';
							reply[rchar++] = ':';
							reply[rchar++] = robs[i].getPos().x + '0';
							reply[rchar++] = ',';
							reply[rchar++] = robs[i].getPos().y + '0';
							reply[rchar++] = ':';
							reply[rchar++] = robs[i].getTarg().x + '0';
							reply[rchar++] = ',';
							reply[rchar++] = robs[i].getTarg().y + '0';
							reply[rchar++] = '_';
						}
					}// end loop
					reply[rchar++] = '\n';
				
					SP->WriteData(reply, rchar);
					// If target position report
				}else if(datastr.find("targ")!=string::npos){
					if(synchCount<robCount){
						if(callID!= id){
							synchCount++;
							callID = id;
						}
						// store  data
						robs[id].setTarg(datastr.at(10)-'0', datastr.at(12)-'0');
					}
					else{
						char permiss[7] = "sysa*\n";
						// Compare robot's target positions
						int goID = -1;
						for(int a = 0; a < robCount-1; a++){
							for(int b = a + 1; b < robCount; b++){
								if(robs[a].compareTarg(robs[b].getTarg())){
									if(robs[a].getMoves() > robs[b].getMoves())
										goID = a;
									else
										goID = b;
								}
							}
						}// end loop

						if(goID >= 0){
							permiss[3] = goID+'A';
							SP->WriteData(permiss, 6);
						}
						else{
							for(int i = 0; i < robCount; i++){
								permiss[3] = i + 'A';
								SP->WriteData(permiss, 6);
							}
						}
						synchCount = 0;
					}

				// If completion report
				}else if(datastr.find("done")!=string::npos){
					// Evaluate system completion
					doneCount++;
					Robot replacement;
					robs[id] = replacement;
				}

			}// end if starting with "rob"
			stringComplete = false;
			datastr = "";
		}// end if complete message

	}// end loop

	fileOut1.close();

	getchar();
	return 0;
}

