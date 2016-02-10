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
#include <vector>
#include <stdlib.h>
#include "SerialClass.h"	// Library described above
#include "Robot.h"


using namespace std;

//Global variables
Serial* SP; // Serial connection
queue<string, list<string>> msgQ;
bool systemFin;
mutex foobar;

#define ROBCOUNT 2

void init(Robot * robs, char * reply, int rchar);
void messageRead();

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{
	Robot robs[ROBCOUNT];// Hardcode to number of bots
	systemFin = false;

	SP = new Serial("COM5");    // adjust as needed
	if (SP->IsConnected())
		printf("System channel up...\n");

	// Create a thread
	thread child(messageRead);


	int doneCount = 0;// number of robots that have completed the game
	int posSynch = 0;
	int targSynch = 0;// number of robots waiting to synch target data
	int posID = -1;
	int targID = -1;

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
	for (int i = 0; i < ROBCOUNT - 1; i++) {
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


	init(robs, reply, rchar);
	cout << "GO!\n";

	//BEGIN-----------------------------------
	while (doneCount < ROBCOUNT && SP->IsConnected())
	{
		//Sleep(10);
		string datastr = "";

		if (!msgQ.empty()) {

			foobar.lock();
			datastr = msgQ.front();
			msgQ.pop();
			foobar.unlock();

			if (datastr.find("rob") != string::npos) {
				char idChar = datastr.at(3);
				int id = idChar - 'A';
				if (id > ROBCOUNT || id < 0)
					continue;

				// If position report
				if (datastr.find("pos") != string::npos) {

					coords tempPos;
					tempPos.x = datastr.at(9) - '0';
					tempPos.y = datastr.at(11) - '0';
					if (posSynch < ROBCOUNT) {
						if (posID != id) {
							posSynch++;
							posID = id;
						}

						// if not a duplicate
						if (tempPos.x != robs[id].getPos().x || tempPos.y != robs[id].getPos().y) {
							cout << datastr << endl;
							//Store data
							robs[id].setPos(tempPos.x, tempPos.y);
							robs[id].incrementMoves();
						}// end if duplicate
					}
					if(posSynch == ROBCOUNT) {
						//Reply with most recent position data
						for (int i = 0; i < ROBCOUNT; i++) {
							reply[3] = i + 'A';

							int ctr = 7;
							for (int j = 0; j < ROBCOUNT; j++) {
								if (j != i) {
									reply[ctr] = j + '0';
									reply[ctr + 2] = robs[j].getPos().x + '0';
									reply[ctr + 4] = robs[j].getPos().y + '0';
									reply[ctr + 6] = robs[j].getTarg().x + '0';
									reply[ctr + 8] = robs[j].getTarg().y + '0';

									ctr += 10;
								}
							}// end loop

							string ack = "xxxxxxxxxxxxx";
							char pid = i + 'A';
							do {
								SP->WriteData(reply, rchar);
								if (!msgQ.empty()) {
									foobar.lock();
									ack = msgQ.front();
									msgQ.pop();
									foobar.unlock();
									cout << ack << endl;
								}
							} while (ack.at(3) != pid || ack.at(4) != 'p');
						}
						posSynch = 0;
					}

					// If target position report
				}
				else if (datastr.find("targ") != string::npos) {
					if (targSynch < ROBCOUNT) {
						if (targID != id) {
							targSynch++;
							targID = id;
						}
						// check duplicate
						if (datastr.at(10)-'0' != robs[id].getTarg().x || datastr.at(12) - '0' != robs[id].getTarg().y) {
							cout << datastr << endl;
							//Store data
							robs[id].setTarg(datastr.at(10) - '0', datastr.at(12) - '0');
						}// end if duplicate
					}
					if (targSynch == ROBCOUNT) {
						char permiss[8] = "sysat&\n";
						// Compare robot's target positions
						int goID = -1;
						for (int a = 0; a < ROBCOUNT - 1; a++) {
							for (int b = a + 1; b < ROBCOUNT; b++) {
								if (robs[a].compareTarg(robs[b].getTarg())) {
									// flip a coin
									srand(time(NULL));
									int flip = (rand() * 100) % 2;
									if (flip == 0)
										goID = a;
									else
										goID = b;
								}
							}
						}// end loop

						permiss[3] = goID + 'A';
						string ack = "xxxxxxxxxxxxx";
						char tid = goID + 'A';
						do {
							SP->WriteData(permiss, 6);
							if (!msgQ.empty()) {
								foobar.lock();
								ack = msgQ.front();
								msgQ.pop();
								foobar.unlock();
								cout << ack << endl;
							}
						} while (ack.at(3) != tid || ack.at(4) != 't');

						permiss[5] = '*';
						for (int i = 0; i < ROBCOUNT; i++) {
							permiss[3] = i + 'A';
							tid = goID + 'A';
							do {
								SP->WriteData(permiss, 6);
								if (!msgQ.empty()) {
									foobar.lock();
									ack = msgQ.front();
									msgQ.pop();
									foobar.unlock();
									cout << ack << endl;
								}
							} while (ack.at(3) != tid || ack.at(4) != 't');
						}
						targSynch = 0;
					}

					// If completion report
				}
				else if (datastr.find("done") != string::npos) {
					// Evaluate system completion
					doneCount++;
				}

			}// end if starting with "rob"
			datastr = "";
		}// end if complete message

	}// end loop
	 //END------------------------------------------

	systemFin = true;

	//Determine winner
	int winIndex = 0;
	int max = 0;
	for (int r = 0; r < ROBCOUNT; r++) {
		char printID = r + 'A';
		cout << "Robot " << printID << " score:" << robs[r].getScore() << endl;
		if (robs[r].getScore() > max)
			winIndex = r;
	}
	char printID = winIndex + 'A';
	cout << "Winner is Robot " << printID << "!\n\n";

	cout << "\nProcess Complete";

	getchar();
	child.join();
	return 0;
}


void init(Robot * robs, char * reply, int rchar) {

	//initial position read loop
	int readcount = 0;
	int prevID = -1;
	while (readcount < ROBCOUNT) {
		string datastr = "";

		if (msgQ.empty())
			continue;

		foobar.lock();
		datastr = msgQ.front();
		msgQ.pop();
		foobar.unlock();

		if (datastr.find("rob") != string::npos) {
			char idChar = datastr.at(3);
			int id = idChar - 'A';

			if (id > ROBCOUNT || id < 0 || id == prevID)
				continue;

			cout << datastr << endl;
			prevID = id;
			if (datastr.find("pos") != string::npos) {
				coords tempPos;
				tempPos.x = datastr.at(9) - '0';
				tempPos.y = datastr.at(11) - '0';

				//Store data
				robs[id].setPos(tempPos.x, tempPos.y);
				robs[id].incrementMoves();
				readcount++;
			}
		}
	}// end init read loop


	// begin init write loop
	for (int r = 0; r < ROBCOUNT; r++) {
		//Reply with most recent position data
		reply[3] = r + 'A';

		int ctr = 7;
		for (int i = 0; i < ROBCOUNT; i++) {
			if (i != r) {
				reply[ctr] = i + '0';
				reply[ctr + 2] = robs[i].getPos().x + '0';
				reply[ctr + 4] = robs[i].getPos().y + '0';
				reply[ctr + 6] = robs[i].getTarg().x + '0';
				reply[ctr + 8] = robs[i].getTarg().y + '0';

				ctr += 10;
			}
		}// end loop
		string ack = "xxxxxxxxxxxxx";
		char id = r + 'A';
		do {
			SP->WriteData(reply, rchar);
			if (!msgQ.empty()) {
				foobar.lock();
				ack = msgQ.front();
				msgQ.pop();
				foobar.unlock();
				cout << ack << endl;
			}
		} while (ack.at(3) != id || ack.at(4)!='p');
	}// end init write loop
}// end init


void messageRead() {

	char inT[2];
	string messageT = "";


	while (!systemFin) {
		SP->ReadData(inT, 1);

		if (inT[0] != '\n') {

			if (messageT.empty() && inT[0] > 0) {
				messageT += inT[0];
			}
			if (!messageT.empty()) {

				if (inT[0] != messageT.back() && inT[0] > 0) {
					messageT += inT[0];
				}
			}
		}
		else {
			if (!messageT.empty()) {
				foobar.lock();

				msgQ.push(messageT);

				foobar.unlock();
			}
			messageT = "";
		}
	}// end loop

	return;
}