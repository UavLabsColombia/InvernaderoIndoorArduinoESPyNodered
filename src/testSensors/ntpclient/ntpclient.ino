#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

const char *ssid     = "......";
const char *password = "Elchompiras10";

WiFiUDP ntpUDP;

//For UTC -5.00 : -5 * 60 * 60 : -18000  TimeOffset
NTPClient timeClient(ntpUDP, "0.pool.ntp.org", -18000);

void setup(){
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
}

void loop() {
  timeClient.update();
  //Serial.println(timeClient.getFormattedTime());
  Serial.println(timeClient.getFullFormattedTime());
  
  
}
