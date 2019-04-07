#include <DS3231.h>
#include <Wire.h>

DS3231 Clock;


byte Year=19;
byte Month=03;
byte Date=24;
byte Hour=16;
byte Minute=30;
byte Second=0;

bool Century=false;
bool h12;
bool PM;



void setup() {
  // Start the serial port
  Serial.begin(9600);

  // Start the I2C interface
  Wire.begin();


   }



void setearFecha(){
    Clock.setClockMode(false);  // set to 24h
    Clock.setYear(Year);
    Clock.setMonth(Month);
    Clock.setDate(Date);
    Clock.setHour(Hour);
    Clock.setMinute(Minute);
    Clock.setSecond(Second);
  }
void imprimirHora(){

    Serial.print(Clock.getYear(), DEC);
    Serial.print("-");
    Serial.print(Clock.getMonth(Century), DEC);
    Serial.print("-");
    Serial.print(Clock.getDate(), DEC);
    Serial.print(" ");
    Serial.print(Clock.getHour(h12, PM), DEC); //24-hr
    Serial.print(":");
    Serial.print(Clock.getMinute(), DEC);
    Serial.print(":");
    Serial.println(Clock.getSecond(), DEC);

}


void validarFechaySetear()
{
  if(Clock.getYear() < 19){
    Serial.println("Fecha No configurada :(");
    setearFecha();
    }
    else{
      Serial.println("Fecha configurada :)");
      }
}


void loop() {
  Serial.print("Esperando 10seg para imprimir en serial");
  delay(10000);
  Serial.print("Fecha actual:");
  imprimirHora();
  Serial.println("Validando y seteando fecha...");
  validarFechaySetear();
  Serial.print("Nueva fecha:");
  while(true)
  {
    delay(1000);
    imprimirHora();
  }
}
