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
      