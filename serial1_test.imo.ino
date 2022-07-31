void setup() {
  // put your setup code here, to run once:

Serial.begin(9600);
Serial1.begin(9600); // Tx GP0, RX GP1 Default
Serial2.begin(9600); // TX GP8, RX GP9 Default

}

void loop() {
  // put your main code here, to run repeatedly:
Serial2.println("Hello");
delay(500);

}
