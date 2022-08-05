
#include "WizFi360.h"

void setup() {
  // put your setup code here, to run once:

Serial.begin(9600);
Serial1.begin(9600);
//Serial2.begin(115200);
pinMode(LED_BUILTIN, OUTPUT);

}

bool flag = false;
char receivebuff[9]={0,};
void loop() {

  if (Serial1.available()){
    
     if(flag != true){
              byte x = Serial1.read();
              if(x == 0x02)
              {
                   flag = true;                 //STX has arrived
              }
      }else{
                 byte m = Serial1.readBytesUntil(0x03, receivebuff, 9); //0x03 is not saved
                 receivebuff[m] = '\0'; //insert null-charcater;
                 if (validateData(receivebuff)){
                    float weight = parseBuff(receivebuff);
                    Serial.println(weight,3);
                 }else{
                  Serial.print(receivebuff);   //shows received string 
                  Serial.println("error");   //shows received string 
                 }
                 
                 //-- add codes to recompute CHKSUM and check validity of data
                 //-- add codes to process received string
                 memset(receivebuff, 0x00, 9);  //array is reset to 0s.
                 flag = false;
          }

    //}
    
  }

}



bool validateData(char buff[]){
  if (strlen(buff)!=8) return false;
  for (int i=0; i<strlen(buff); i++){    
    switch (i){
      case 0:
        if (isNum(buff[i])) continue; 
      case 1:
        if (isPlusMinus(buff[i])) continue;
      case 2:
        if (isNum(buff[i])) continue;
      case 3:
        if (isNum(buff[i])) continue;
      case 4:
        if (buff[i] == 0x2E) continue;
      case 5 ... 7:
         if (isNum(buff[i])) continue;
      default:
        return false;   
    }   
  }
  return true;
}

bool isNum(char c){
   switch (c){
    case 0x30 ... 0x39:
      return true;    
    case 0x20: //공백문자
      return true; 
    default:
      return false;
  }
  
}

bool isPlusMinus(char c){
  switch (c){
    case 0x2B:
      return true;
    case 0x2D:
      return true;
  }      
  return false;

}

float parseBuff(char buff[]){
  char tmp[6];
  for (int i=2; i<=8;i++){
     tmp[i-2] = buff[i];
  }
  float num= atof(tmp);
  if (buff[1]=='-'){
    num = num * -1;
  }
  return num;
}
  
