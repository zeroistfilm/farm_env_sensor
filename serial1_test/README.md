### Wiznet360 AT command

- https://medium.com/wiznet/wizfi360-at-command-%EC%86%8C%EA%B0%9C-9bb8dcfca099

```cpp
#include "WizFi360.h"

void setup() {
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
```

boardrate : 115200
아두이노 시리얼 모니터
Both NL & CR 모드 

```AT
AT+CWMODE_CUR=1  //wifi station mod
AT+CWDHCP_CUR=1,1 // DHCP
AT+CWJAP_CUR="yd2","123a123a11" //connect to AP

AT+CIPSTA_CUR? // current connection infomation, ip, gateway, netmask 
AT+CIFSR // ip, mac

AT+CWQAP // disconnect 

```