#ifndef TAG_H
#define TAG_H

#include "Arduino.h"
#include <rfiduino.h>

class Tag
{
  public:
    Tag();
    byte tagData[5];
    void setTagData(byte one, byte two, byte three, byte four, byte five);
    byte getTagData(int whichByte);
    byte * getTag();
};

#endif
