struct coords{
		int x;
		int y;
};

#ifndef ROBOT_H_
#define ROBOT_H_
#include <vector>

class Robot{
private:
	coords currentPos;
	coords targ;
	int moves;
	
public:
	std::vector<coords> targets;
	int maxDistance;

	Robot(){
		moves = 0;
		currentPos.x = 5;
		currentPos.y = 5;
		targ.x = 5;
		targ.y = 5;
		maxDistance = 0;
	}
	void setPos(int x, int y);
	void setTarg(int x, int y);
	coords getPos();
	coords getTarg();
	bool compareTarg(coords rTarg);
	void incrementMoves();
	int getMoves();
};
#endif