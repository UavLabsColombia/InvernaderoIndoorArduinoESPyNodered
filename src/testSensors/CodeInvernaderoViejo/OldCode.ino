/*Escrito por: Heberth Ardila
Correo: heberthardila@gmail.com
Invernadero Revision 0.1
Arduino Mega 2560
*/

// libreria para el control de los sensores dht11 y dht22
#include <DHT.h>
//libreria para el display lsc 16x2
#include <LiquidCrystal.h>

//libreria para el relog RTC 3231
#include <DS3231.h>
#include <Wire.h>

/*se definen los pines a utilizar con el sensor de distancia HC-SR04
Conectar pin VCC del sensor a 5V con Arduino
Conectar pin GND con GND de Arduino
Ping trig del sensor al pin 22 de arduino 
Ping echo del sensor al ping 9 de arduino

*/
#define trigPin 8
#define echoPin 9

// pin por el cual se usara el LDR (analogo 0)
#define ldr A0
// pin de datos para el sensor DHT11
#define DHTPIN11 2
// pin de datos para el sensor DHT22
#define DHTPIN22 3


// se define un tipo de dato DHT11 y DHT22 para enviar a la clase que identifica el tipo de sensor.
#define dhtTipo11 DHT11   // DHT 22  
#define dhtTipo22 DHT22

// se define el pin el cual sera conectado el higrometro para medir la humedad del suelo. 
#define higrometro A1

// se inicializa el panel de 8 reles en los pines digitales 22, 23, 24, 25
// ventilador para la entrada de CO2 y frio al sistema
#define ventiladorInt 22
//ventilador para la extraccion de calor
#define ventiladorOut 23

#define bombaElectrica 24

// se inicializan los pines del display LCD 16*2
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

// inicializa los objetos para la clase DHT
DHT dhtext(DHTPIN11, dhtTipo11);
DHT dhtint(DHTPIN22, dhtTipo22);

// inicializa un objeto de la clase DS3231
DS3231  relogRtc;
// variable de tipo tiempo para el relog
//Time t;
// Variables de control del nivel de agua sobre la cubeta
int depositoVacio=0;
int depositoLleno=0;
int cmAgua=0;
//int ldrMin = 1023;
//int ldrMax = 0; 

void setup()
{
  //inicia serial
  Serial.begin(9600);
  // Start the I2C interface
  Wire.begin();
  // inicializa el display lcd 
  lcd.begin(16, 2);
  //inicia los objetos de la clase DHT 
  dhtext.begin();
  dhtint.begin();
  //inicia el objeto para la clase DS3231
  //relogRtc.begin();
  // confirma que el anio del sistema no sea 2000, si lo es lo modifica por el actual.
  calibrarRelog();
  //versionSoft();
  // pindes de entrada/salida para el sensor de ultra sonido hc-sr04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //pinMode(pulsadorSi, INPUT);
  
  // modo del pin usado para los ventiladores
  pinMode(ventiladorInt, OUTPUT); 
  pinMode(ventiladorOut, OUTPUT); 
  
  pinMode(bombaElectrica, OUTPUT);
  // inicia y deja el ventilador en alto para que no se encienda.
  digitalWrite(ventiladorInt, HIGH);
  digitalWrite(ventiladorOut, HIGH);
  //Inicia la electrovalcula y la deja en High para que esta deshabilitada
  digitalWrite(bombaElectrica, HIGH);
  // se calibra el nivel del agua 
  //calibrarAgua();
  //calibra el nivel de luz
  //calibrarLdr();
  
}

void versionSoft()
{
  lcd.print("Invernadero Automazido Arduino");
  delay(800);
    for (int positionCounter = 0; positionCounter < 14; positionCounter++) {
    // scroll one position right:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(150);  
  }
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.setCursor(0,1);
  lcd.print("Version 0.1");
  delay(2000);
  lcd.clear();
}

// determina la distancia en cm que emite el sensor. 
long sensorDistancia()
{
  long duracion, distancia;
  digitalWrite(trigPin, LOW); // estabiliza la salida del sensor de ultra sonido 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // envia el pulso hacia el sensor con una duracion de 10microsegundos
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); // apaga el impulso
  duracion = pulseIn(echoPin, HIGH); // se optiene la suracion del pulso enviado por el pin de echo
  distancia = (duracion*0.0136); // se calcula la distancia en cm obtenida por el sensor. 
  return distancia;
}
void calibrarRelog()
{
  // se le india a arduino que el tiempo va a ser el del
 // t = relogRtc.getTime();
  
//  if(t.year<2016)
//  {
//    Serial.print("Calibrando el relog");
//    delay(1000);
//    relogRtc.setDOW(SATURDAY);     // Envia el dia de la semana 
//    relogRtc.setTime(18, 46, 0);     //Envia la hora en formato 24horas
//    relogRtc.setDate(2, 4, 2016);    // Envia el año en formato dia/mes/anio
//  }
//  Serial.print(t.hour, DEC);
//  Serial.print(" hour(s), ");
//  Serial.print(t.min, DEC);
//  Serial.print(" minute(s) and ");
//  Serial.print(t.sec, DEC);
//  Serial.println(" second(s) since midnight.");
  
}

// Calibra el nivel del agua, dando un tiempo para tomar los datos de nivel minimo y nivel maximo 
void calibrarAgua()
{
  
  lcd.clear();
  lcd.home();
  lcd.print("NivelMinimoAgua");
  lcd.setCursor(0,1);
  lcd.print("10seg config");
  delay(5000);
  depositoVacio = sensorDistancia();
  lcd.home();
  lcd.print("NivelMaximoAgua");
  lcd.setCursor(0,1);
  lcd.print("10Seg config");
  delay(5000);
  depositoLleno = sensorDistancia();
  cmAgua= depositoVacio - depositoLleno;
  Serial.print("Vacio:");
  Serial.print(depositoVacio);
  Serial.print("Lleno:");
  Serial.print(depositoLleno);
  Serial.print("Agua en cm:");
  Serial.print(cmAgua);
  
}

// retorna los cm de agua actuales en el recipiente. 
int nivelAgua()
{
  int cmActual = depositoVacio - sensorDistancia();
  return cmActual; 
}


// Metodos para LDR

int sensorLdr()
{
  int lecturaLdr = analogRead(ldr);
//  Serial.print("ldr:");
//  Serial.print(lecturaLdr);
//  Serial.print("\n");
//  Serial.print("Mapeado:");
//  Serial.print(map(lecturaLdr,22, 922, 100,0));
//  Serial.print("\n");
  return map(lecturaLdr,22, 922, 100,0);
}


//Metodos para los dos sensores DHT11 y DHT22, devuelve sus valores en  grados Celcius  y porcentaje de humedad en el hambiente
// leer la temperatura del sensor exterior 
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

// leer el dato entregado por el higrometro enterrado.
double humedadHigrometro()
{
  double humedad= analogRead(A1);
  // Serial.print("HumedadHigrometro: ");
   //Serial.print(humedad);
   //Serial.print("\n");
  return map(humedad,350, 1023, 100,0);
}

// control del ventilador, los enciente y los apaga dependiendo del tiepo de entrada.
void ventilarFrio(int tiempo)
{
  digitalWrite(ventiladorInt, LOW);
  delay(tiempo/3);
  digitalWrite(ventiladorOut, LOW);
  delay(tiempo/3);
  digitalWrite(ventiladorInt, HIGH);
  delay(tiempo/3);
  digitalWrite(ventiladorOut, HIGH);
}

void ventilarCaliente(int tiempo)
{
  Serial.print("Se calienta el aire");
}

// returna 1 si esta de noche o 0 si esta de dia
int validarDiaNoche()
{
//  t = relogRtc.getHour();
//  
//  if (t.hour > 18)
//  {
//    //Serial.print("Esta de noche");
//    return 1;
//    
//  }
//  else 
//  {
//    //Serial.print("Esta de dia");
//    return 0;
//  }
  return 0;
}
  
void controlTempertura()
{
  Serial.print(dhtInTemp());
  Serial.print("\n");
  if(validarDiaNoche()==0)
  {
    if(dhtInTemp()>25)
    {
      Serial.print("Ventilando Frio\n");
      ventilarFrio(5000);
      controlTempertura();
    }
    else
    { 
      if(dhtInTemp()<23)
      { 
        Serial.print("Ventilando Caliente\n");
        ventilarCaliente(5000);
        controlTempertura();
      }
      else
      {
        Serial.print("la temperatura es estable\n");
      }
      
    }
    
  }
  else
  {
    if(dhtInTemp()>28)
    {
      ventilarFrio(5000);
      controlTempertura();
    }
    else
    { 
      if(dhtInTemp()<26)
      {
        ventilarCaliente(5000);
        controlTempertura();
      }
      else
      {
        Serial.print("la temperatura es estable\n");
      }
      
    }
    
  }
}

// habilita el paso de agua en la bomba electrica
void habilitarBombaElectrica(int tiempo)
{
  digitalWrite(bombaElectrica, LOW);
  delay(tiempo);
  digitalWrite(bombaElectrica, HIGH);
}

// riega el suelo con agua, durante 5 segundos 
void regarSuelo(int tiempo)
{
  if(nivelAgua()> 5)
  {
    habilitarBombaElectrica(tiempo);
  }
  else
  {
    Serial.print("llenar Valde Agua");
  }
}

// retorna 0 para esqueje, 1 para crecimiento, 2 floracion
int estadoPlanta()
{
  
  // el estado dependera de la programacion inicial de cuando fue sembrada
  return 0;
}


// controla la Humedad del suelo por medio del higrometro
void controlHumedadSuelo()
{
  // si esta "Esqueje"
  if(estadoPlanta()==0)
  {
    if(humedadHigrometro()<85)
    {
      regarSuelo(5000);
      //delay(10000);
      controlHumedadSuelo();
    }
  }
  else
  {
    // si esta en "Crecimiento"
    if(estadoPlanta()==1)
    {
      if(humedadHigrometro()<60)
    {
      regarSuelo(5000);
      //delay(10000);
      controlHumedadSuelo();
      
    }
    }
    else
    {
      // si esta en "Floracion"
      if(humedadHigrometro()<45)
       {
        regarSuelo(5000);
        //delay(10000);
        controlHumedadSuelo();
       }
    }
  }
}

// controla la humedad del ambiente interno. 
void controlHudadHambiente()
{
  
}
  
void loop()
{ 
  lcd.clear();
  lcd.print (sensorDistancia());
  lcd.print("cm");
  lcd.setCursor(0,0);
  lcd.print("Nivel Agua:");
  lcd.print(nivelAgua());
  lcd.print(nivelAgua());
  lcd.setCursor(0,1);
  lcd.print("Nivel Luz:");
  lcd.print(sensorLdr());
  delay(1000);
  //delay(500);
  //dhtExtTemp();
  //dhtExtHum();
  //dhtInTemp();
  //dhtInHum();
  //Serial.print(humedadHigrometro());
  //calibrarRelog();
  //sensorLdr();
 //ventilar(20000);
  //delay(5000);
  //validarDiaNoche();
  //controlTempertura();
  
  
 
}
