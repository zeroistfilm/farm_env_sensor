#include <Wire.h> 

#define DLY_5000  5000
#define DLY_1000  1000
#define HEAD_1 0x42
#define HEAD_2 0x4d
unsigned char pmsbytes[31];

void ClearPMS7003(){
  char temp;
  while (Serial1.available() ){                 // While nothing is received
    temp=Serial1.read();
   }

  delay(100);
}


void setup()
{
  Serial.begin(9600);
  Serial.println("start ");
  Serial1.begin(9600);

}

void loop()
{
 
 delay(500);
 if(Serial1.available()>=31){
    int i=0;

    //initialize first two bytes with 0x00
    pmsbytes[0] = 0x00;
    pmsbytes[1] = 0x00;
    
    for(i=0; i<31 ; i++){
      pmsbytes[i] = Serial1.read();

      //check first two bytes - HEAD_1 and HEAD_2, exit when it's not normal and read again from the start
      if( (i==0 && pmsbytes[0] != HEAD_1) || (i==1 && pmsbytes[1] != HEAD_2) ) {
        break;
      }
    }

    if(i>2) { // only when first two stream bytes are normal
      if(pmsbytes[29] == 0x00) {  // only when stream error code is 0
        int PM1_0_val = (pmsbytes[10]<<8) | pmsbytes[11]; // pmsbytes[10]:HighByte + pmsbytes[11]:LowByte => two bytes
        int PM2_5_val = (pmsbytes[12]<<8) | pmsbytes[13]; // pmsbytes[12]:HighByte + pmsbytes[13]:LowByte => two bytes
        int PM10_val = (pmsbytes[14]<<8) | pmsbytes[15]; // pmsbytes[14]:HighByte + pmsbytes[15]:LowByte => two bytes
        
        Serial.print("PMS7003 sensor - PM1.0 : ");
        Serial.print(PM1_0_val);
        Serial.print(" ug/m^3,  PM2.5 : ");
        Serial.print(PM2_5_val);
        Serial.print(" ug/m^3,  PM10 : ");
        Serial.print(PM10_val);
        Serial.println(" ug/m^3");
      } else {
        Serial.println("Error skipped..");
      }
    } else {
      Serial.println("Bad stream format error");
    }
  }

}
