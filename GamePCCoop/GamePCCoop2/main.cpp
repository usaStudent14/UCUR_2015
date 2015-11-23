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
#define ROBCOUNT 2 //Hardcode to number of bots

void init(Robot * robs);
void coopAlg(Robot * robs, vector<coords> targs);
int closest(coords currentPos, vector<coords> targs, Robot &rob);
void messageRead();

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{
	int doneCount = 0;// number of robots that have completed the game
	int synchCount = 0;// number of robots waiting to synch target data
	int callID = -1;
	Robot robs[ROBCOUNT];// Hardcode to number of bots
	systemFin = false;
	SP = new Serial("COM5");    // adjust as needed
	if (SP->IsConnected())
		printf("System channel up...\n");
	// Create a thread
	thread child(messageRead);

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

	init(robs);
	cout << "System ready...\n";



	//BEGIN-----------------------------------
	while (doneCount < ROBCOUNT && SP->IsConnected())
	{
		Sleep(50);
		string datastr = "";

		if (!msgQ.empty()) {
			cout << msgQ.front() << endl;

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

					// if not a duplicate
					if (tempPos.x != robs[id].getPos().x || tempPos.y != robs[id].getPos().y) {
						//Store data
						robs[id].setPos(tempPos.x, tempPos.y);
						robs[id].incrementMoves();
					}// end if duplicate

					//Reply with most recent position data
					reply[3] = id + 'A';

					int ctr = 7;
					for (int i = 0; i < ROBCOUNT; i++) {
						if (i != id) {
							reply[ctr] = i + '0';
							reply[ctr + 2] = robs[i].getPos().x + '0';
							reply[ctr + 4] = robs[i].getPos().y + '0';
							reply[ctr + 6] = robs[i].getTarg().x + '0';
							reply[ctr + 8] = robs[i].getTarg().y + '0';

							ctr += 10;
						}
					}// end loop

					SP->WriteData(reply, rchar);

					// If target position report
				}
				else if (datastr.find("targ") != string::npos) {

					if (synchCount < ROBCOUNT) {
						if (callID != id) {
							synchCount++;
							callID = id;
						}

						// store  data
						robs[id].setTarg(datastr.at(10) - '0', datastr.at(12) - '0');
					}
					else {
						char permiss[7] = "sysa*\n";
						// Compare robot's target positions
						int goID = -1;
						for (int a = 0; a < ROBCOUNT - 1; a++) {
							for (int b = a + 1; b < ROBCOUNT; b++) {
								if (robs[a].compareTarg(robs[b].getTarg())) {
									if (robs[a].getMoves() > robs[b].getMoves())
										goID = a;
									else
										goID = b;
								}
							}
						}// end loop

						if (goID >= 0) {
							permiss[3] = goID + 'A';
							SP->WriteData(permiss, 6);
						}
						else {
							for (int i = 0; i < ROBCOUNT; i++) {
								permiss[3] = i + 'A';
								SP->WriteData(permiss, 6);
							}
						}
						synchCount = 0;

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

	cout << "\nProcess Complete\n------------------------------------------------\n";
	for (int i = 0; i < ROBCOUNT; i++) {
		char id = i + 'A';
		cout << "Robot " << id << " finished in " << robs[i].getMoves() << endl;
	}

	getchar();
	child.join();
	return 0;
}

void init(Robot * robs) {

	//initial position read loop
	int readcount = 0;
	while (readcount < ROBCOUNT) {
		string datastr = "";

		if (msgQ.empty())
			continue;

		cout << msgQ.front() << endl;

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

				//Store data
				robs[id].setPos(tempPos.x, tempPos.y);
				robs[id].incrementMoves();
				readcount++;
			}
		}
	}// end init read loop

	// Hardcode targets for now
	vector<coords> globalTargs;
	coords temptarg;
	temptarg.x = 4;
	temptarg.y = 2;
	globalTargs.push_back(temptarg);
	temptarg.x = 2;
	temptarg.y = 3;
	globalTargs.push_back(temptarg);
	temptarg.x = 0;
	temptarg.y = 4;
	globalTargs.push_back(temptarg);
	temptarg.x = 3;
	temptarg.y = 3;
	globalTargs.push_back(temptarg);

	// call pathing algorithm
	coopAlg(robs, globalTargs);

	/*Send lists to bots-
	* Message protocol: coords sent as ID:#,# multiple
	* times to increase chance of good message, then
	* single character 'd' sent to indicate end
	*/
	getchar();
	cout << "Broadcasting Targets...\n";

	char targ[7] = "a:#,#\n";
	//for each bot
	for (int r = 0; r < ROBCOUNT; r++) {
		targ[0] = r + 'A';
		cout << targ[0] << ":\n";
		for (size_t sr = 0; sr < robs[r].targets.size(); sr++) {
			targ[2] = robs[r].targets[sr].x + '0';
			targ[4] = robs[r].targets[sr].y + '0';
			SP->WriteData(targ, 6);
			cout << targ[2] << "," << targ[4] << endl;
			Sleep(10);
		}
	}

	getchar();
	SP->WriteData("d\n", 2);


}// end init


void coopAlg(Robot * robs, vector<coords> targs) {
	vector<coords> temp;
	double maxSum = 0;

	// loop to calculate maximum distances robots could travel
	for (int r1 = 0; r1 < ROBCOUNT; r1++) {
		temp = targs;// copy of target list
		coords pos;
		pos.x = robs[r1].getPos().x;
		pos.y = robs[r1].getPos().y;
		while (!temp.empty()) {
			int index = closest(pos, temp, robs[r1]);
			pos.x = temp[index].x;
			pos.y = temp[index].y;
			temp.erase(temp.begin() + index);
		}
		maxSum += robs[r1].maxDistance;// sum all max distances
	}// end loop

	int starter = 0;
	int maxDis = 0;
	for (int r2 = 0; r2 < ROBCOUNT; r2++) {
		if (robs[r2].maxDistance > maxDis) {
			starter = r2;
			maxDis = robs[r2].maxDistance;
		}
	}// end second loop

	int moves = ceil(maxSum / pow((double)ROBCOUNT, 2));

	// FOLLOWING SOLUTION IS NOT SCALEABLE--------

	// make target list for first robot
	temp.clear();
	coords pos;
	pos.x = robs[starter].getPos().x;
	pos.y = robs[starter].getPos().y;
	for (int i = 0; i < robs[starter].targets.size(); i++) {
		int distance = abs(pos.x - robs[starter].targets[i].x) + abs(pos.y - robs[starter].targets[i].y);
		if (distance <= moves) {
			temp.push_back(robs[starter].targets[i]);
			moves -= distance;
			pos.x = robs[starter].targets[i].x;
			pos.y = robs[starter].targets[i].y;
		}
		else {
			//robs[starter].targets = temp;
			//break;
		}
	}// end third loop

	robs[starter].targets = temp;

	for (int r4 = 0; r4 < ROBCOUNT; r4++) {// for each bot
		if (r4 != starter) {// except the first one
			for (int t = robs[r4].targets.size() - 1; t >= 0; t--) {// for each of its targets
				for (int s = 0; s < robs[starter].targets.size(); s++) {// for each of the first's targets
					if (robs[r4].targets[t].x == robs[starter].targets[s].x
						&& robs[r4].targets[t].y == robs[starter].targets[s].y) // if match
					{
						robs[r4].targets.erase(robs[r4].targets.begin() + t);// erase from targets
					}
				}
			}
		}
	}// end fourth loop
}

int closest(coords currentPos, vector<coords> targs, Robot &rob) {
	int minDis = 10;
	int index = 0;
	for (int i = 0; i < targs.size(); i++) {
		int distance = abs(currentPos.x - targs[i].x) + abs(currentPos.y - targs[i].y);
		if (distance < minDis) {
			minDis = distance;
			index = i;
		}
	}
	rob.targets.push_back(targs[index]);
	rob.maxDistance += minDis;
	return index;
}

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