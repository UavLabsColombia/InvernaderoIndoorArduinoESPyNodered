// Cadena de texto recibida por serial para su tratamiento
String str = "";


void setup() {
  delay(100);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  // No arranco hasta que el serial este disponible :)
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Conectado :) escribre algo! y lo devuelvo...");

 
}


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() { // run over and over
  if (Serial.available()) {
    str = Serial.readStringUntil('\n');
    String topic = getValue(str,',', 0);
    String dataTopic = getValue(str,',', 1);
//---
    //str =Serial.readStringUntil('\n');
    //Serial.println(dato);
  
//  Serial.print("Topic:");
//  Serial.println(topic);
//  Serial.print("Dato:");
//  Serial.println(dataTopic);
  }
  
}
