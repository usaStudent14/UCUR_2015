#include <Wire.h>
#include <NXShield.h>

int ID = 1;

NXShield nxshield;

String inputString = "";
bool stringComplete = false;

void setup() {
  inputString.reserve(200);
  Serial.begin(9600);        // connect to the serial port
  
  nxshield.init(SH_HardwareI2C);
  
  delay(500);
}
String output;
int count = 0;
void loop () {
    delay(500);
    output = "rob";
    output += ID;
    output += "_sent";
    output += count;
    output +='\n';
    Serial.println(output);
    
    count++;
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    inputString += inChar;

    if (inChar == '\n' || inChar == '\r') {
      stringComplete = true;
    }
  }
  
}
