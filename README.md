# FrameWork-Soldadoras
Framework para soldadoras con STM32

juandiazainscough@gmail.com - Técnico en Electrónica - Estudiante Cs. Físicas (UBA)

Se utilizó STM32CubeIDE v1.81

Problema:
Normalmente las soldadoras eléctricas suelen manejar control de corriente, lectura de preset de usuario, funciones HotStart, ArcForce, AntiStick, control térmico de protección entre otras funciones, mediante una densa placa de control.
Últimamente, las soldadoras más nuevas y personalizadas, han reducido la densidad de componentes en la placa de control mediante el uso de microcontroladores como STM8 y STM32 que permiten mejor control de variables, personalización y menores costos.
Una característica de los microcontroladores es que necesitan ser programados y vienen de fábrica “vacíos”. A diferencia de los componentes antiguamente usados que podían ser simplemente reemplazados, los microcontroladores no pueden ser reemplazados si no se les carga el código correspondiente.
Esto representa un problema para un servicio técnico especializado en reparar placas sin necesidad de reemplazarlas ya que, dado el caso que un microcontrolador se queme, no es posible reparar la placa.
Tampoco es posible extraer el código de un microcontrolador sano ya que suelen estar protegidos contra lectura.

Solución:
La solución a esta problemática es desarrollar un programa propio que logre imitar las funciones que tenía el microcontrolador original.

Iron 300 Black:
Un caso concreto donde sucede esto es en la Iron 300 Black.
El equipo cuenta con 3 placas: La de potencia y principal (IGBTs, fuente, etc.), placa de generación de PWM y control y una última exclusiva para un STM8 y un display con conexión paralela.
En ocasiones, este STM8 se quema y deja inutilizable el resto del equipo. La solución típica es obtener esa placa sana de otra Iron 300 Black.
Dado que esta no es una solución real y depende de stock de más placas, se desarrolló una placa propia.

Ingeniería Inversa:
La placa de PWM ( conectada mediante peine a la principal) se conecta por medio de un conector de 10 pines a la placa del STM.
Trabajando con placas sanas , realizando ingeniería inversa, midiendo con osciloscopio y tester el comportamiento de cada pin ante la activación de diferentes funciones del menú mostrado en el display (HotStart, ArcForce, AntiStick, VRD, TIG, Diametro de electrodo y Corriente) se descubrieron las siguientes funcionalidades:

- Pin 1: NC (No connected) 
- Pin 2: GND	           
- Pin 3: Control Térmico  
- Pin 4: No descubierto	  
- Pin 5: Control PWM		 
- Pin 6: NC
- Pin 7:GND
- Pin 8: Orden de generación en vacío
- Pin 9: Detección arco de soldadura
-	Pin 10: Alimentación (5V)

Pin 3: cuando está en 5V, la temperatura del equipo es adecuada. Si está en 0V, hay sobretemperatura (OVERHEAT).

Pin 4: No se logró descubrir su uso, ante múltiples pruebas y funciones siempre se mantuvo en 0V. De todas formas, no fue relevante.

Pin 5: La placa encargada de generar el PWM cuenta con un SG3525 que, según la tensión que recibe en uno de sus pines, genera un PWM con cierto ciclo de trabajo. Esta tensión que necesita es enviada directamente por este pin.

Pin 8: En vacío, la soldadora genera una tensión entre bornes de salida de 60V, pero, ante las funciones VRD y TIG, debe generar una tensión menor a 25V. Si este pin está en 5V, la tensión de salida generada por el equipo será 60V; en cambio, si el pin está en 0V, será de 24V.

Pin 9: Este pin varía su tensión entre 0V y 5V según la tensión censada del arco de soldadura. Por ejemplo, cuando el arco está en 60V (sin soldar), este pin se pone en 5V; pero cuando está en 0V (electrodo pegado), este pin se pone en 0V.

Con la función de cada pin descubierta, es posible continuar con el desarrollo embebido.
Desarrollo Embebido
Para el desarrollo de una nueva placa que pudiera reemplazar a la original se utilizó:
-	STM32F103C8T6 (bluepill)
-	Display 128x64 SSD1306
-	Encoder rotativo
Se buscó escribir un código estructurado y escalable con múltiples librerías y funciones reutilizables con el fin de crear un framework que sirva de base no solo para la Iron 300 Black sino para casi cualquier otra soldadora.

main.c
Aquí podemos encontrar un loop infinito que permite el funcionamiento ininterrumpido de la soldadora. Está librería en particular debe ser reescrita para cada soldadora ya que aquí se colocan las funciones esperadas.

SSD1306.c
Esta librería permite el control de cualquier display  monocromático con driver SSD1306. Es reutilizable para cualquier otro proyecto. Cuenta con funciones que permiten borrar la pantalla, actualizarla, dibujar bitmaps, caracteres, etc.
Este display es I2C y usa 2 bits por píxel.

ST7789.c
Esta librería permite el control de cualquier display RGB con driver ST7789. Es reutilizable para cualquier otro proyecto que use este tipo de display. Tiene funciones muy similares a la líbreria SSD1306.c añadiendo la posibilidad de utilizar 65535 colores.
Este display es SPI y usa 2 bytes por píxel.
Debido a la cantidad de bytes por píxel, el barrido y la actualización es muy lenta, por lo que se utilizó el protocolo DMA que permite escribir rápidamente en pantalla sin bloquear al microcontrolador.

user.c
Esta líbreria contiene las funciones principales de cualquier soldadora. Se encarga de la generación del PWM, el control térmico, el control de VRD, TIG, HotStart, ArcForce y AntiStick, lectura de botones y encoder (con antirrebote y progresiones) , etc.
Esta librería es reutilizable para casi cualquier otra soldadora ya que las funciones son globales, no específicas de la Iron 300 Black.

fsm.c
Esta librería es la encargada de controlar la maquina de estados, una forma ordenada de programar.
Controla específicamente el menú del display donde se encuentra la activación y desactivación de las funciones principales y el control de corriente. Es lo que ve directamente el usuario.
Esta librería es única de cada soldadora. Debe ser reescrita.

fonts.c
Esta librería contiene todas las tipografías utilizadas por el display.  Es reutilizable para cualquier maquina e incluso para cualquier display.

flash.c
Esta librería se encarga de leer y guardar en memoria las variables seleccionadas por el usuario. Si el usuario apagó el equipo con una configuración especifica, al prenderse de nuevo, la configuración será la misma que antes de apagarse en lugar de asignar valores genéricos de Corriente, HotStart, etc. en cada encendido.
La memoria Flash se desgasta con cada uso, pero esta librería está diseñada para usar distintas secciones de memoria para permitir que dure años e, incluso, décadas. Si no fuera así, podría desgastarse en días de uso.

Diseño de PCB
Una vez listo el trabajo embebido, para llevarlo a la realidad fue necesario desarrollar un PCB.
La placa diseñada cuenta con conectores hembras para poder colocar directamente la bluepill sin necesidad de soldar un microcontrolador y permitiendo así, reutilizar la misma placa para distintos proyectos si es necesario e incluso cambiarla rápidamente en caso  de quemarse.
Además cuenta con:
-	 Encoder rotativo con sistema antirrebote (debounce) y pull-ups para evitar falsas lecturas.
-	Conector de 10 pines para conexión directa.
-	LED HeartBeat.
-	Display con pull-ups
-	Transistor para control Pin 8
-	Fuente 5V a 3V3 para utilizar la alimentación de la soldadora (Pin 10).
-	Capacitores de filtro.
-	Filtro pasabajos activo de segundo orden con LM358 (Fc = 8Hz).
-	Fuente secundaria de 220VAC a 5VDC
  
El filtro pasabajos evita que el arco de soldadura genere ruidos (sobretodo en el Pin 9) para evitar falsas detecciones y comportamientos erráticos.
El STM selecciona por software y mediante un relé la fuente a utilizar. Si están disponibles los 5V de la soldadora, corre el programa normalmente. Si por alguna razón no están presentes (la soldadora no funciona), no se activa un relé por lo que la fuente que alimentará al microcontrolador será la secundaria. Esto es detectado y se imprime en el display “CALL SERVICE” para avisar al cliente sobre un problema interno en la soldadora, ya sea IGBTs quemados, fuente quemada, etc.
