/*Escrito por: Heberth Ardila
Correo: heberthardila@gmail.com
Invernadero Revision 0.1
Arduino Mega 2560
*/

//Librerias
#include <DS3231.h>
#include <Wire.h>
// libreria para el control de los sensores dht11 y dht22
#include "DHT.h"

// Objetos de Librerias
DS3231 Clock;


//Variables

//---Variables de fecha---
int Year=14;
int Month=03;
int Date=24;
int Hour=16;
int Minute=30;
int Second=0;
bool Century=false;
bool h12;
bool PM;
//Hora de red
String horaNtp;


//---- Variables de comandos y instrucciones recibidas por serial
String cadena="";
String tipo="";
String dato="";

//------Modo de operacion de planta
String modoOperacion = "automatica";

//----Variables de Sensores-----//

/*se definen los pines a utilizar con el sensor de distancia HC-SR04
Conectar pin VCC del sensor a 5V con Arduino
Conectar pin GND con GND de Arduino
Ping trig del sensor al pin 22 de arduino 
Ping echo del sensor al ping 9 de arduino

*/
#define trigPin 8
#define echoPin 9

// Pin de Arduino que se utilara para lectura Analoga del LDR.
#define ldr A0

// Pin de Arduino que se utilara para lectura Analoga del Higrometro.
#define higrometro A1

// Variables de control del nivel de agua sobre la cubeta
int depositoVacio=0;
int depositoLleno=0;
int cmAgua=0;

// se inicializa el panel de 8 reles en los pines digitales 22, 23, 24, 25
// ventilador para la entrada de CO2 y frio al sistema
#define ventiladorInt 22
//ventilador para la extraccion de calor
#define ventiladorOut 23
// Sistema de agua
#define bombaElectrica 24


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



void setup() {

  // Habilita la comunicacion serial
  Serial.begin(9600);
  Serial1.begin(9600);


//  while (!Serial) {
//    ; // Espera a que Serial este disponible
//  }

  while (!Serial1) {
    ; // Espera a que Serial1 este disponible
  }
  //SetTimeDesdeNTP();
  //Serial.println("Serial Vivo!");

  // Inicia la interfaz i2c con el relog RTC
  Wire.begin();
  dhtext.begin();
  dhtint.begin();

  // Ejecuciones sobre variables del sistema con el fin de inicializar sensores en algun estado
  // modo del pin usado para los ventiladores
  pinMode(ventiladorInt, OUTPUT); 
  pinMode(ventiladorOut, OUTPUT); 
  pinMode(bombaElectrica, OUTPUT);
  
  // inicia y deja el ventilador en alto para que no se encienda.
  digitalWrite(ventiladorInt, HIGH);
  digitalWrite(ventiladorOut, HIGH);
  //Inicia la electrovalcula y la deja en High para que esta deshabilitada
  digitalWrite(bombaElectrica, HIGH);

  // pindes de entrada/salida para el sensor de ultra sonido hc-sr04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  // --Calibracion del agua
  calibrarAgua();
}

//--------Funciones del relog---------
void SetTimeRTC(String horaDeRed){

    //Publica en MQTT sobre el topic ntpTime el tiempo de la red
    Serial1.println("debug,Tiempo de Red:" + horaDeRed);
    //Serial.println(horaDeRed);
    String year = partiCadena(horaDeRed,'-',0);
    year = partiCadena(year,'0',1);
    Year = year.toInt();

    String month = partiCadena(horaDeRed,'-',1);
    Month = month.toInt();

    String date = partiCadena(horaDeRed,'-',2);
    Date = date.toInt();

    String hour = partiCadena(horaDeRed,'-',3);
    Hour = hour.toInt();

    String minute = partiCadena(horaDeRed,'-',4);
    Minute = minute.toInt();

    String second = partiCadena(horaDeRed,'-',5);
    Second = second.toInt();

//    Serial.print("Añobyte:");
//    Serial.println(Year);
    Clock.setClockMode(false);  // set to 24h
    Clock.setYear(Year);
    Clock.setMonth(Month);
    Clock.setDate(Date);
    Clock.setHour(Hour);
    Clock.setMinute(Minute);
    Clock.setSecond(Second);
    delay(1000);
    Serial1.println("debug,Relog RTC: "+imprimirHora());
  }
String imprimirHora(){
    //Clock.getYear()
    String str="";
    str=Clock.getYear();
    str+="-";
    str+=Clock.getMonth(Century);
    str+="-";
    str+=Clock.getDate();
    str+=" ";
    str+=Clock.getHour(h12, PM);
    str+=":";
    str+=Clock.getMinute();
    str+=":";
    str+=Clock.getSecond();

    return str;
}


void SetTimeDesdeNTP()
{
  if(Clock.getYear() < 19){
    //Envia el topic setear hacia el esp con el fin de ejecutar denuevo el metodo actualizarHora()
    Serial1.println("setTime,no");
    delayMicroseconds(100);
    Serial1.print("setear,si\n");
    
    }
    else{
      // Se publica en mqtt el dato si para actualizar la hora.
      Serial1.println("setTime,si");
      //Serial1.println("debug,Relog RTC actualizado..");
      }
}


//--------Funcion Partir cadena--------//

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


//-----------Funciones para LDR-------------------------------
int leerLdr()
{
  //Serial.println(analogRead(ldr));
  //return analogRead(ldr);

  // Mapea el valor leido entre el minimo de 40 cuando hay mucha luz y 1020 cuando hay poca luz
  // de 0 a 100 siendo 0 mucha luz y 100
  return map(analogRead(ldr),40, 1020, 100,0);
}


//---------Funciones para Higrometro---------------------------
int leerHigrometro()
{
  //Lee e imprime el valor puro del puerto para despues mapearlo
  //Serial.println(analogRead(higrometro));
  //return analogRead(higrometro);

  // Mapea el valor leido entre el minimo de 288 cuando esta mojado  y 1005 cuando esta seco
  // de 0 a 100 siendo 0 seco y 100 muy mojado
  return map(analogRead(higrometro),315, 1005, 100,0);
}



//--------------Funciones para DHT11 y DHT22
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


//------Funciones para medir la cantidad de agua en el recipiente con HC SR04
int sensorDistancia()
{
  int duracion, distancia;
  digitalWrite(trigPin, LOW); // estabiliza la salida del sensor de ultra sonido 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // envia el pulso hacia el sensor con una duracion de 10microsegundos
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); // apaga el impulso
  duracion = pulseIn(echoPin, HIGH); // se optiene la suracion del pulso enviado por el pin de echo
  distancia = (duracion*0.0136); // se calcula la distancia en cm obtenida por el sensor. 
  //Serial.print("Distancia sonar: "+ distancia);
  //Serial.print("Sonar: ");
  //Serial.println(distancia);
  return distancia;
}

// Calibra el nivel del agua, dando un tiempo para tomar los datos de nivel minimo y nivel maximo 
void calibrarAgua()
{
  
  
  Serial1.print("debug,Ponga el tarro vacio...");
  delay(2000);
  Serial1.print("debug,10seg config para configurar...");
  delay(2000);
  depositoVacio = sensorDistancia();
  Serial1.print("debug,Deposito vacio: ");
  Serial1.print(depositoVacio);
  Serial1.print("cm.");
  Serial1.print("\n");
  delay(2000);
  Serial1.print("debug,Ahora llene el tarro...");
  delay(2000);
  Serial1.print("debug,10seg config para configurar...");
  delay(2000);
  depositoLleno = sensorDistancia();
  Serial1.print("debug,Deposito Lleno: ");
  Serial1.print(depositoLleno);
  Serial1.print("cm.");
  Serial1.print("\n");
  cmAgua= depositoVacio - depositoLleno;

  if(cmAgua < 10){
    Serial1.print("debug,Agua menor a 10cm-recalibrando... ");
    calibrarAgua();
    }
  
}

// retorna los cm de agua actuales en el recipiente. 
int nivelAgua()
{
  int cmActual = depositoVacio - sensorDistancia();
  return cmActual; 
}




// ----------Funciones de control de actuadores
void ventiladorEntrada(int tiempo){
  digitalWrite(ventiladorInt, LOW);
  delay(tiempo);
  digitalWrite(ventiladorInt, HIGH);
  
  }

void ventiladorSalida(int tiempo){
  digitalWrite(ventiladorOut, LOW);
  delay(tiempo);
  digitalWrite(ventiladorOut, HIGH);
  
  }


// habilita el paso de agua en la bomba electrica
void habilitarBombaElectrica(int tiempo)
{
  digitalWrite(bombaElectrica, LOW);
  delay(tiempo);
  digitalWrite(bombaElectrica, HIGH);
}


//-------Funciones que interactuan con los actuadores------------
void regarSuelo(int de0a100){
  habilitarBombaElectrica(1000);
  }

void renovarCO2(){
  ventiladorEntrada(10);
  }

void bajarTemperatura(int temp){
  ventiladorEntrada(100);
  ventiladorSalida(100);
  }

void subirTemperatura(int temp){
  ventiladorEntrada(1000);
  ventiladorSalida(1000);
  }

// -------------Funcion de ejecucion automatica ---------------
void ModoAutomatico()
{ 
  Serial1.println("debug,Modo Automatico en Ejecucion...");
  //Encender ventilador
  
  //renovarCO2();
  //bajarTemperatura();
  //regarSuelo();
  subirTemperatura(26);
  }



//---------------Funcion de iniciacion del codigo--------------

void loop() { // run over and over

  if (Serial.available()) {
    Serial1.println(Serial.readStringUntil('\n'));
  }

  if (Serial1.available()) {
    cadena = Serial1.readStringUntil('\n');
    //Serial.println(cadena);
    // Recibe una cadena y la parte en dos subcadenas
    tipo = partiCadena(cadena,':', 0);
    dato = partiCadena(cadena,':', 1);
  
    if(tipo=="Fecha"){
      horaNtp = dato;
      //Serial.println("TipoFecha");
      //SetTimeDesdeNTP();
      SetTimeRTC(horaNtp);
      //Serial1.println("setTime,sienloooooooooooooooooooooooooop");
    }

   // Si llega desde mqtt sobre el topic comando, este no es separado por coma, los comandos se separan con :
   // desde nodered publicar sobre el topic en mtqq con el nombre (comando)
   // enviar el dato de la siguiente manera desde el topic.
   // comando:accion
   //Recibe el topic comando y valida la accion ejecutando una rutina o subProceso
   //Desde control manual de la planta hasta automatico.
   if(tipo=="comando"){
      Serial.println("comando: "+dato);
      // Ejecucion de rutinas sobre el sistema
        
      if(dato == "actualizarRTCconNTP"){
          
          //Rutina para actualizar RTC...
          //Serial.println("Actualizando RTC por comando desde red..");
          SetTimeDesdeNTP();

        }

      //----------Rutinas de Automatizacion o Manual.
      if(dato == "automatica"){
          // Ejecuta rutina automatica
          // Poner una bandera global para mantener el proceso en ejecucion.
          modoOperacion = "automatica";

       }
       if(dato == "manual"){
        
          // Ejecuta rutina automatica
          // Switchear la bandera para alterar el estado a manual sacando del proceso automatico
          modoOperacion = "manual";
          Serial1.println("debug,Modo Manual en Ejecucion...");
       }

       // Si el modo de operacion es manual, me permitira: regar el suelo, extraer co2, bajar temperatura y subir.

       
        if(dato == "regar"){
          // Ejecucion para regar si el modo de operacion es manual
         if (modoOperacion=="manual")
         {
           Serial1.println("debug,Regando Suelo...");
         }
         else{
          // No se puede regar porque esta automatico
          Serial1.println("debug,No se puede regar en Automatico...");
          }
        }
        
   }
  

  }
  
   
   
  //Aqui van todas las ejecuciones que se desean actualizar automaticamente sobre NodeRed, se debe de enviar el dato al topic indicado separado por una coma
  //Analiza la hora y escribe en el topic si esta o no actualizada.
  SetTimeDesdeNTP();
  //Imprimir Hora en topic horaActual
  Serial1.print("horaActual,");
  Serial1.println(imprimirHora());
  //Imprimir % LDR en topic ldr
  Serial1.print("ldr,");
  Serial1.println(leerLdr());
  //Imprimir % de Humedad en Suelo en topic higrometro
  Serial1.print("higrometro,");
  Serial1.println(leerHigrometro());
  //Imprimir Temperatura y Humedad Interna con DHT22
  //Temperatura Interna en topic tempInt
  Serial1.print("tempInt,");
  Serial1.println(dhtInTemp());
  //Humedad Interna en topic humInt
  Serial1.print("humInt,");
  Serial1.println(dhtInHum());
  //Temperatura Externa en topic
  Serial1.print("tempExt,");
  Serial1.println(dhtExtTemp());
  //Humedad Externa en topic
  Serial1.print("humExt,");
  Serial1.println(dhtExtHum());

  //Nivel de agua en topic nivelAgua
  Serial1.print("nivelAgua,");
  Serial1.println(nivelAgua());
  
  //Se inicia el modo automatico
  if (modoOperacion=="automatica")
  {
    ModoAutomatico();
  }

  //----------------------
  //   Debug
    
  //----------------------
    
  //Tiempo necesario para la actualizacion de los topic en el servidor mqtt
  delay(3000);

}
