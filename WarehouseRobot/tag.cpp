// tag.cpp
// Provides holder for tag IDs
#include "Arduino.h"
#include "tag.h" //include the declaration for this class

// constructor
Tag::Tag(){}

void Tag::setTagData(byte one, byte two, byte three, byte four, byte five)
{
  tagData[0]=one;
  tagData[1]=two;
  tagData[2]=three;
  tagData[3]=four;
  tagData[4]=five;
};


byte Tag::getTagData(int whichByte)
{
  return tagData[whichByte];
};

byte * Tag::getTag()
{
  return tagData;
}
