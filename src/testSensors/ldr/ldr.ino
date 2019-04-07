/*
El siguiente codigo, lee el valor de luz que
se refleja en el LRD, este code se integrara al invernadero
*/


// Pin de Arduino que se utilara para lectura Analoga.
#define ldr A0

void setup()
{
  //inicia serial para imprimir el dato
  Serial.begin(9600);

}

int leerLdr()
{ 
  //Serial.println(analogRead(ldr));
  //return analogRead(ldr);

  // Mapea el valor leido entre el minimo de 40 cuando hay mucha luz y 1020 cuando hay poca luz
  // de 0 a 100 siendo 0 mucha luz y 100
  return map(analogRead(ldr),40, 1020, 100,0);
}

void loop()
{
  Serial.print("Luz:");
  Serial.print(leerLdr());
  Serial.println("%");
  delay(1000);
}
