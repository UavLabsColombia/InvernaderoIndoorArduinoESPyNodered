/*Escrito por: Heberth Ardila
Correo: heberthardila@gmail.com
Invernadero Revision 0.1
Codigo para ESP8266 que comunica los datos seriales al server mqtt separando las cadenas y convirtiendolas en datos y topics

Se recibe por serial la cadena
"/dev/sensor/1,83.5"
se conviente en:
topic: /dev/sensor/1
datos del topic: 83.5
siempre y cuando exista un separador de tipo ',' coma.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Update these with values suitable for your network.

//Red Invernadero
const char* ssid = "invernaderoarduino";
const char* password = "invernaderoarduino0.1";
//Red en casa
//const char* ssid = "......";
//const char* password = "Elchompiras10";
//Red en el local
//const char* ssid = ".......";
//const char* password = "Arditech_.2018";

const char* mqtt_server = "108.61.86.127";
String topic = "";
String dataTopic = "";

//Conexion UDP 
WiFiUDP ntpUDP;


//Para UTC -5.00 el TimeOffset se calculcula: -5 * 60 * 60 : -18000  
NTPClient timeClient(ntpUDP, "0.pool.ntp.org", -18000);

// Cadena de texto recibida por serial para su tratamiento
String str = "";

WiFiClient espClient;
PubSubClient client(espClient);



void setup()
{ //Esperita para iniciar todo :D
  delay(1000);
  //Pines
  pinMode(BUILTIN_LED, OUTPUT);

  //Serial
  Serial.begin(9600);
  while (!Serial) {
      ; //no arranca si el serial no esta conectado
    }

  //Wifi
  setup_wifi();

  //Se inicia el cliente NTP
  timeClient.begin();
  
  
  //MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(1000);
  // Se actualiza la hora
  actualizarHora();
  delay(4000);
  
}

void setup_wifi() {

  delay(5000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(200);
    digitalWrite(BUILTIN_LED, LOW);
    delay(200);
    //Serial.print(".");
  }
  //Serial.print("");
  randomSeed(micros());
}

void callback(char* topic, byte* payload, unsigned int length) {

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);

  }
  Serial.print("\n");
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("vivo", "Invernadero Encendido! :D");
      // ... and resubscribe
      client.subscribe("comandos");
      //Otros topics aqui
    } else {
      delay(3000);
    }
  }
}

String partiCadena(String data, char separator, int index)
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



void actualizarHora(){
  
  timeClient.update();
  if(timeClient.getYear() < 2019 || timeClient.getYear() == 1969){
    client.publish("debug,", "Fecha del server mal, leyedo de nuevo...");
    actualizarHora();
  }
  
//Envia hacia arduino la fecha para actualizarla en RTC
Serial.print("Fecha:");
Serial.print(timeClient.getYear());
Serial.print("-");
Serial.print(timeClient.getMonth());
Serial.print("-");
Serial.print(timeClient.getDate());
Serial.print("-");
Serial.print(timeClient.getHours());
Serial.print("-");
Serial.print(timeClient.getMinutes());
Serial.print("-");
Serial.print(timeClient.getSeconds());
Serial.println();
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
  
  if (Serial.available()){
      // Recibe la cadena por el serial para separar el topic de la data y publicarlo
      str = Serial.readStringUntil('\n');
      
      // Extrae la cadena antes de la ,
      topic = partiCadena(str,',', 0);
      //Serial.print("Topic:");
      //Serial.println(topic);
      // Crea variable de tipo bufer con el largo de la cadena mas uno
      char topicBuffer[topic.length()+1];
      //Convierte topic a char array y lo almacena en topicBuffer
      topic.toCharArray(topicBuffer, topic.length()+1);

      //Extrae la cadena despues de la primera coma
      dataTopic = partiCadena(str,',', 1);
      //Serial.print("Data:");
      //Serial.println(dataTopic);
      char dataTopicBuffer[dataTopic.length()+1];
      dataTopic.toCharArray(dataTopicBuffer, dataTopic.length());

      // Funcion que setea de nuevo la hora sobre el sistema si la red no da ok.
//      Serial.print(topic);
//      Serial.print(",");
//      Serial.println(dataTopic);
//      Serial.print(":");
      
      if (topic == "setear" && dataTopic == "si")
          {
            client.publish("debug","actualizando hora");
            actualizarHora();
          } 
        
      //Publicando en mqtt
      client.publish(topicBuffer,dataTopicBuffer);
      
  }
  delayMicroseconds(100);
  topic = "";
  dataTopic = "";
  //delay(200);
  
}
