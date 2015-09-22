#include <tchar.h>
#include <string>
#include <fstream>
#include <iostream>
#include <queue>
#include <list>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <mutex>
#include <thread>
#include "SerialClass.h"	// Library described above
#include "Robot.h"


using namespace std;

//Global variables
Serial* SP; // Serial connection
queue<string, list<string>> msgQ;
bool systemFin;
mutex foobar;


void messageRead();

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{
	const int ROBCOUNT = 2; //Hardcode to number of bots
	Robot robs[ROBCOUNT];// Hardcode to number of bots
	systemFin = false;
	//fstream fileOut1("data1.txt", ios::out);

	SP = new Serial("COM5");    // adjust as needed
	if (SP->IsConnected())
		printf("System channel up...\n");

	// Create a thread
	thread child(messageRead);


	int doneCount = 0;// number of robots that have completed the game
	int synchCount = 0;// number of robots waiting to synch target data
	int callID = -1;

	//Format: "sys<recipientID>rp_<remoteID>:0,0(current pos):0,0(target pos)_<remoteID>..._\n"
	char reply[100];
	reply[0] = 's';
	reply[1] = 'y';
	reply[2] = 's';
	reply[3] = 'a';
	reply[4] = 'r';
	reply[5] = 'p';
	reply[6] = '_';
	int rchar = 7;
	for(int i = 0; i < ROBCOUNT - 1; i++){
		reply[rchar++] = 'a';
		reply[rchar++] = ':';
		reply[rchar++] = '5';
		reply[rchar++] = ',';
		reply[rchar++] = '5';
		reply[rchar++] = ':';
		reply[rchar++] = '5';
		reply[rchar++] = ',';
		reply[rchar++] = '5';
		reply[rchar++] = '_';
	}// end loop
	reply[rchar++] = '\n';

	while(doneCount < ROBCOUNT && SP->IsConnected())
	{	
		//Sleep(10);
		string datastr = "";

		if(!msgQ.empty()){
		
			cout << msgQ.front() << endl;
			
			foobar.lock();
			datastr = msgQ.front();
			msgQ.pop();
			foobar.unlock();

			
			

			if(datastr.find("rob")!=string::npos){
				char idChar = datastr.at(3);
				int id = idChar - 'A';


				// If position report
				if(datastr.find("pos")!=string::npos){
					coords tempPos;
					tempPos.x = datastr.at(9)-'0';
					tempPos.y = datastr.at(11)-'0';

					// if not a duplicate
					if(tempPos.x!=robs[id].getPos().x || tempPos.y!=robs[id].getPos().y){
						//Store data
						//fileOut1.write(datastr.c_str(), datastr.length());// file
						robs[id].setPos(tempPos);
						robs[id].incrementMoves();
					}// end if duplicate

					//Reply with most recent position data
					reply[3] = id + 'A';

					int ctr = 7;
					for(int i = 0; i < ROBCOUNT; i++){
						if(i != id){
							reply[ctr] = i + '0';
							reply[ctr+2] = robs[i].getPos().x + '0';
							reply[ctr+4] = robs[i].getPos().y + '0';
							reply[ctr+6] = robs[i].getTarg().x + '0';
							reply[ctr+8] = robs[i].getTarg().y + '0';

							ctr += 10;
						}
					}// end loop

					SP->WriteData(reply, rchar);
					// If target position report
				}else if(datastr.find("targ")!=string::npos){
					if(synchCount<ROBCOUNT){
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
						for(int a = 0; a < ROBCOUNT-1; a++){
							for(int b = a + 1; b < ROBCOUNT; b++){
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
							for(int i = 0; i < ROBCOUNT; i++){
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
			datastr = "";
		}// end if complete message

	}// end loop

	systemFin = true;

	//WaitForSingleObject(threadH, threadID);

	// Close all thread handle and free memory allocation.
	//CloseHandle(threadH);
	//HeapFree(GetProcessHeap(), 0, NULL);
	//pThreadData = NULL;    // Ensure address is not reused.

	//fileOut1.close();

	getchar();
	child.join();
	return 0;
}

void messageRead(){
	
	char inT[2];
	string messageT = "";


	while (!systemFin){
		//cout << msgQ.size() << endl;
		SP->ReadData(inT, 1);

		if (inT[0] != '\n'){
			
			if(messageT.empty()){
				if (inT[0] != 'Í'){
					messageT += inT[0];
				}
			}
			if(!messageT.empty()){

				if(inT[0] != messageT.back()){
					if (inT[0] != 'Í'){
						messageT += inT[0];
					}
				}
			}
		}else{
			if (!messageT.empty()){
				foobar.lock();

				msgQ.push(messageT);

				foobar.unlock();
			}
			messageT = "";
		}
	}// end loop

	return; 
}