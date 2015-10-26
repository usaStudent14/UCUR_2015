#include "vector.h"


//First constructor
vector::vector () {
  size = 0;       // set the size of vector to zero.
  items = 0;      // set the number of items of vector to zero.
  newtIndex = 0;
  // allocate enough memory for the array.
  contents = (coords *) malloc (sizeof (coords) * initialSize);

  // if there is a memory allocation error.
  if (contents == NULL) {
    Serial.println("1VECTOR: insufficient memory to initialize vector.");
    exit(0);
  }

  // set the initial size of the vector.
  size = initialSize;
}


//Constructor, but it takes in a coord object
vector::vector (coords input[]) {
  size = 0;       // set the size of vector to zero.
  items = 0;      // set the number of items of vector to zero.
  newtIndex = 0;
  // allocate enough memory for the array.
  contents = (coords *) malloc (sizeof (coords) * initialSize);

  // if there is a memory allocation error.
  if (contents == NULL) {
    Serial.print("VECTOR: insufficient memory to initialize vector.");
    exit(0);
  }
  // set the initial size of the vector.
  size = initialSize;

  for (int j = 0; j < (sizeof(input) / sizeof(coords)); j++ ) {
    this->addCoords(input[j]);
  }

}



//The Deconstructor
vector::~vector() {
  free (contents); // deallocate the array of the vector.

  contents = NULL; // set vector's array pointer to nowhere.

  size = 0;        // set the size of vector to zero.
  items = 0;       // set the number of items of vector to zero.

}



coords vector::getCoords() {
  int index = newtIndex;
  if (index < 0 || index > (items - 1)) {
    Serial.println("2VECTOR: index out of bounds");
    exit(0);
  }
  return contents[index];
}


void vector::addCoords(coords addend) {
  // check if the vector is full.
  if (items == size)
    // double size of array.
    resize (size * 2);

  // store the item to the array.
  contents[items++] = addend;
}


void vector::deleteCoords() {
  int index = newtIndex;
  // check if the vector is empty.
  if (index < 0 || index > (items - 1)) {
    Serial.println("Index is out of bounds.");
    exit(0);
  }
  if (items == 0) {
    Serial.println("3VECTOR: can't pop item from vector: vector is empty.");
    exit(0);
  }
  // decrease the items.
  items--;

  for (index = index + 1; index <= items ; index++ ) {

    contents[index - 1] = contents[index];

  }


  // shrink size of array if necessary.
  if (items > 0 && (items <= size / 4))
    resize (size / 2);

}


bool vector::findClosest(coords currentPos) {

  if (items == 0) {
    return false;
  }

  float closest = sqrt(sq(currentPos.x - contents[0].x) + sq(currentPos.y - contents[0].y));

  int index = 0;

  for (int i = 1; i < items ; i++) {
    float distance = sqrt(sq(currentPos.x - contents[i].x) + sq(currentPos.y - contents[i].y));

    if (distance < closest) {
      closest = distance;
      index = i;
    }

  }
  newtIndex = index;
  return true;
}

void vector::printVector() {
  for (int k = 0; k < items ; k++) {
    Serial.print(contents[k].x);
    Serial.print(", ");
    Serial.print(contents[k].y);
    Serial.println(" ");
  }
}


bool vector::isEmpty() {
  return (items == 0);
}



void vector::resize(int s) {

  // defensive issue.
  if (s <= 0) {
    Serial.println("4VECTOR: error due to undesirable size for vector size.");
    exit(0);
  }
  // allocate enough memory for the temporary array.
  coords * temp = (coords *) malloc (sizeof (coords) * s);

  // if there is a memory allocation error.
  if (temp == NULL) {
    Serial.println("5VECTOR: insufficient memory to initialize temporary vector.");
    exit(0);
  }
  // copy the items from the old vector to the new one.
  for (int i = 0; i < items; i++)
    temp[i] = contents[i];

  // deallocate the old array of the vector.
  free (contents);

  // copy the pointer of the new vector.
  contents = temp;

  // set the new size of the vector.
  size = s;

}




bool vector::checkOppPos(int robNum, char ID, coords remote_pos[2][2]) {

  bool repath = false;
  for (int i = 0; i < robNum; i++) {
    if (i == ID - 'A') {
      continue;
    }
    for (int j = 0; j < items; j++) {
      if (remote_pos[i][0].x == contents[j].x && remote_pos[i][0].y == contents[j].y) {

        deleteCoords(j);
        if (j == newtIndex)
          repath = true;

      }
    }
  }

  Serial.println(ID);
  this->printVector();


  return repath;
}

void vector::deleteCoords(int index) {

  // check if the vector is empty.
  if (index < 0 || index > (items - 1)) {
    Serial.println("Index is out of bounds.");
    exit(0);
  }
  if (items == 0) {
    Serial.println("6VECTOR: can't pop item from vector: vector is empty.");
    exit(0);
  }
  // decrease the items.
  items--;

  for (index = index + 1; index <= items ; index++ ) {

    contents[index - 1] = contents[index];

  }


  // shrink size of array if necessary.
  if (items > 0 && (items <= size / 4))
    resize (size / 2);

}


bool vector::duplicateCheck(coords ex) {
for(int q=0; q<items;q++){
if(contents[q].x ==ex.x && contents[q].y==ex.y ){
        return true;
    }
  }
  return false;

}
