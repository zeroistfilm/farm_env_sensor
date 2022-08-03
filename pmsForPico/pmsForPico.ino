#include <Wire.h> 

#define DLY_5000  5000
#define DLY_1000  1000
#define HEAD_1 0x42
#define HEAD_2 0x4d
unsigned char pmsbytes[32];

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
  static int CheckFirst=0;
  static int pm_add[3][5]={0,};
  static int pm_old[3]={0,};
  int chksum=0,res=0;;
  unsigned char pms[32]={0,};
  
  
  if(Serial1.available()>=32){

    for(int j=0; j<32 ; j++){
      pms[j]=Serial1.read();
      if(j<30)
        chksum+=pms[j];
    }

    if(pms[30] != (unsigned char)(chksum>>8) 
        || pms[31]!= (unsigned char)(chksum) ){

      return res;
    }
    if(pms[0]!=0x42 || pms[1]!=0x4d )
      return res;

  Serial.print("Dust raw data debugging :  ");
  Serial.print("1.0ug/m3:");
  Serial.print(pms[10]);
  Serial.print(pms[11]);
  Serial.print("  ");
  Serial.print("2.5ug/m3:");
  Serial.print(pms[12]);
  Serial.print(pms[13]);
  Serial.print("  ");
  Serial.print("2.5ug/m3:");
  Serial.print(pms[14]);
  Serial.println(pms[15]);
  } 

}
