/*
El siguiente codigo, lee el valor de humedad existe 
en el suelo, este code se integrara al invernadero
*/


// Pin de Arduino que se utilara para lectura Analoga.
#define higrometro A1

void setup()
{
  //inicia serial para imprimir el dato
  Serial.begin(9600);

}

int leerHigrometro()
{
  //Lee e imprime el valor puro del puerto para despues mapearlo
  //Serial.println(analogRead(higrometro));
  //return analogRead(higrometro);

  // Mapea el valor leido entre el minimo de 288 cuando esta mojado  y 1005 cuando esta seco
  // de 0 a 100 siendo 0 seco y 100 muy mojado
  return map(analogRead(higrometro),380, 1005, 100,0);
}

void loop()
{
  Serial.print("Humedad:");
  Serial.println(leerHigrometro());
  Serial.println("%");
  delay(1000);
}
