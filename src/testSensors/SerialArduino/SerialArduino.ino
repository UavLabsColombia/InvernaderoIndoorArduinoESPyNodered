// Subir en Arduino

void setup() {
  //delay(3000);
  // Open serial communications and wait for port to open:
  Serial.begin(1200);

  
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Serial.println("Serial Vivo!");
  
  // set the data rate for the SoftwareSerial port

  // 
  delay(15000);
  Serial.println("AT+CIPMUX=1");
  delay(1500);
  Serial.println("AT+CIPSERVER=1,8080");
  delay(2000);
  Serial.println("AT+CIPSEND=0,2");
  delay(100);
  Serial.println("AT");
}



void loop() { // run over and over

  if (Serial.available()) {
    Serial.println(Serial.readStringUntil('\n'));
  } 
}
