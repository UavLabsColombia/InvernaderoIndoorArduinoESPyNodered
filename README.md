#Invernadero Arduino V0.1

Implementación de un Invernadero de tipo Indoor con Arduino y tecnologias IoT.
Se pretende controlar el ambiente de una planta a partir de los datos
adquiridos con la sensoria  y los actuadores presentes.
Esta plataforma, podrá conectarse a la nube de Internet y actualizar los datos sobre una GUI creada con Node-Red utilizando un protocolo de publicación llamado MQTT.

Alcance del Proyecto.
Modos de Trabajo:
El invernadero tiene dos estados
Automático: Realiza el control del ambiente utilizando los datos adquiridos por los sensores y variables fijadas para una planta X que sera seteada.

Manual: Se podran alterar variables de temperatura de modo manual dando clic sobre la interfaz y validando en tiempo real la variación.

Comunicación,
En Internet:
La plataforma es accesible via Internet implementando un Servidor en la nube basado en la arquitectura linux la cual tendrá instalado el servidor MQTT y su respectiva plataforma o Dashboard en Node-Red
La aplicación también se puede acceder vía mobil.

en LAN:
La aplicación se puede replicar en Raspberry y desplegar de manera local realizando la actualización del servidor MQTT en el firmware del esp8266

En youtube iniciamos una serie de videos que demuestran como funciona la aplicacion
https://youtu.be/IIO82C1vLGc