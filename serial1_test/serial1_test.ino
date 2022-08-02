
#include "WizFi360.h"

void setup() {
  // put your setup code here, to run once:

Serial.begin(115200);
Serial2.begin(115200);
}

void loop() {
 
  if(Serial2.available()){
  Serial.write(Serial2.read());
  }
  

  if (Serial.available()) {
    Serial2.write(Serial.read());
  }

}
