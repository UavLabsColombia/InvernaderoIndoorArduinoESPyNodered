

// libreria para el control de los sensores dht11 y dht22
#include "DHT.h"


// pin de datos para el sensor DHT11
#define DHTPIN11 2
// pin de datos para el sensor DHT22
#define DHTPIN22 3


// se define un tipo de dato DHT11 y DHT22 para enviar a la clase que identifica el tipo de sensor.
#define dhtTipo11 DHT11   // DHT 22  
#define dhtTipo22 DHT22


// inicializa los objetos para la clase DHT
DHT dhtext(DHTPIN11, dhtTipo11);
DHT dhtint(DHTPIN22, dhtTipo22);

void setup(){
   Serial.begin(9600);
   dhtext.begin();
   dhtint.begin();
  }


float dhtExtTemp()
{
  float dht11Temp= dhtext.readTemperature();
//  Serial.print("Temdht11: ");
//  Serial.print(dht11Temp);
//  Serial.print("\n");
  return dht11Temp;
}

// leer la humedad del sensor exterior
float dhtExtHum()
{
  float dht11Hum= dhtext.readHumidity();
//  Serial.print("Humdht11: ");
//  Serial.print(dht11Hum);
//  Serial.print("\n");
  return dht11Hum;
}

// leer la temperatura del sensor interior 
float dhtInTemp()
{
  float dht22Temp= dhtint.readTemperature();
//  Serial.print("Tempht22: ");
//  Serial.print(dht22Temp);
//  Serial.print("\n");
  return dht22Temp;
}

// leer la humedad del sensor interior
float dhtInHum()
{
  float dht22Hum= dhtint.readHumidity();
//  Serial.print("Humdht22: ");
//  Serial.print(dht22Hum);
//  Serial.print("\n");
  return dht22Hum;
}



void loop(){
  delay(3000);
  Serial.print("DHT11:");
  Serial.print("Hum Ext:");
  Serial.print(dhtExtHum());
  Serial.print("Temp Ext:");
  Serial.println(dhtExtTemp());

  Serial.print("DHT22:");
  Serial.print("Hum In:");
  Serial.print(dhtInHum());
  Serial.print("Temp In:");
  Serial.println(dhtInTemp());
  
  }
