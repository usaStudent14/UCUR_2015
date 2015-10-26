#ifndef VECTOR_H
#define VECTOR_H

#include "Arduino.h"

struct coords {
  int x = 5;
  int y = 5;

  coords operator=(const coords& o) {
    this->x = o.x;
    this->y = o.y;
    return *this;
  }

};


class vector {

  public:
    vector();
    vector(coords input[]);
    ~vector();

    coords getCoords();
    void addCoords(coords addend);
    void deleteCoords();
    bool findClosest(coords currentPos);
    void printVector();
    bool isEmpty();
    bool checkOppPos(int robNum, char ID, coords remote_pos[2][2]);
    bool duplicateCheck(coords ex);


  private:
    int dummy = 0;
    int newtIndex;
    int size;
    int items;
    const int initialSize = 2;
    
    void deleteCoords(int target);

    coords * contents;

    void resize(int s);


};
#endif
