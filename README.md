# Pass_Sensor
1. UART
   1. Serial
      1. default USB serial
      ```c++
      Serial.begin(9600)
      ```
   2. Serial1
      1. UART0 TX GP0 1pin
      2. UART0 RX GP1 2pin
      ```C++
      Serial1.begin(9600)
      ```
   3. Serial2
      1. UART1 TX GP8 11pin
      2. UART1 RX GP9 12pin
      ```C++
      Serial2.begin(9600)
      ```
3. I2C
   1. 환경센서 0x22
   ```c++
   ```
   3. 가스센서 0x78
   4. O2센서  0x72
   
4. Ethernet w5100s-evb-pico 보드는 `https://inmile.tistory.com/42` 여기 글에서 처럼 
```
https://github.com/WIZnet-ArduinoEthernet/arduino-pico/releases/download/global/package_rp2040-ethernet_index.json
```
이 보드 매니저를 써야 하고

5. Wiz360-evb-pico 보드는  `https://www.hackster.io/giung-kim/how-to-use-wizfi360-evb-pico-in-arduino-065b98` 글에서 보면
```
https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
```
이 보드 매니저를 써야한다..
해당 보드 매니저는 Ethernet w5100s-evb-pico의 정보도 가지고 있으나 호환이 안되는듯.
