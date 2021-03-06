struct coords{
		int x;
		int y;
};

#ifndef ROBOT_H_
#define ROBOT_H_

class Robot{
private:
	coords currentPos;
	coords targ;
	int moves;

public:
	Robot(){
		moves = 0;
		currentPos.x = 5;
		currentPos.y = 5;
		targ.x = 5;
		targ.y = 5;
	}
	void setPos(coords pos);
	void setPos(int x, int y);
	void setTarg(coords settarg);
	void setTarg(int x, int y);
	coords getPos();
	coords getTarg();
	bool compareTarg(coords rTarg);
	void incrementMoves();
	int getMoves();
};
#endif