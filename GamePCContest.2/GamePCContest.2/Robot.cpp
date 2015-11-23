#include "Robot.h"
#include <iostream>

void Robot::setPos(int x, int y) {
	currentPos.x = x;
	currentPos.y = y;
}

void Robot::setTarg(int x, int y) {
	targ.x = x;
	targ.y = y;
}

coords Robot::getPos() {
	return currentPos;
}

coords Robot::getTarg() {
	return targ;
}

int Robot::getScore() {
	return score;
}

bool Robot::compareTarg(coords rTarg) {
	if (rTarg.x == targ.x && rTarg.y == targ.y)
		return true;
	else
		return false;
}

void Robot::incrementMoves() {
	moves++;
}

void Robot::incrementScore() {
	score++;
}

int Robot::getMoves() {
	return moves;
}

