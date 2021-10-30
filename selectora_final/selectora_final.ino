/*******************************DEFINICIONES*******************************************/
int BLANCO = 8 ;            // pin led blanco (led indica si el simulador está encendido o apagado)
int VERDE = 7 ;             // pin led_0 verde
int AMARILLO = 6 ;          // pin led_1 amarillo
int AZUL = 5 ;              // pin led_2 azul
int ROJO = 4 ;              // pin led rojo (indica si está alimentando o en transicion de la selectora)
bool encendido = false;     // true = encendido, false = apagado
bool direccion = true;      // true = directa, false = inversa
bool lock = true;           // false = transitando, true = alimentando
int posicion = 0;           // indica la posicion actual de la selectora (se puede mejorar con almacenamiento en memoria del arduino)
int POSICIONES = 8;         // indica la cantidad de posiciones (se utiliza en la funcion avanzar, para hacer la logica mas "dinamica")
int LEDS = 3;               // indica la cantidad de leds disponibles (se utiliza en la funcion activarLeds, para hacer la logica mas "dinamica")
int leds [3] = {7,6,5};     // representa los pines del arduino que tendran las posiciones 0,1 y 2 en la selectora)
bool selector[8][3] =       // representa las 8 posiciones que puede tomar la Selectora, en funcion de los 3 leds), ( 2^3 posiciones/combinaciones binarias posibles)
{
  {false,false,false},  // binario = 000, decimal = 0, posicion selector = 0
  {false,false,true},   // binario = 001, decimal = 1, posicion selector = 1
  {false,true,false},   // binario = 010, decimal = 2, posicion selector = 2
  {false,true,true},    // binario = 011, decimal = 3, posicion selector = 3
  {true,false,false},   // binario = 100, decimal = 4, posicion selector = 4
  {true,false,true},    // binario = 101, decimal = 5, posicion selector = 5
  {true,true,false},    // binario = 110, decimal = 6, posicion selector = 6
  {true,true,true}      // binario = 111, decimal = 7, posicion selector = 7
}; 
unsigned long tiempo1 = 0;                // inicio del temporizador
unsigned long tiempo2 = 0;                // marcador de tiempo para verificar que el temporizador cumple con tiempo necesario (si se cumple que (tiempo2 > tiempo1+"algo"), entonces cambia de estado)
unsigned long tiempo_alimentando = 333;  // tiempo en milisegundos necesario para alimentar
unsigned long tiempo_transitando = 100;  // tiempo en milisegundos necesario para transitar
/*******************************SETUP*******************************************/
void setup()
{
  Serial.begin(9600);           // inicializa el puerto seríal en la velocidad (bit rate) de 9600 bit/seconds
  pinMode(BLANCO, OUTPUT);      // inicializacion del led blanco
  pinMode(VERDE, OUTPUT);       // inicializacion del led led verde
  pinMode(AMARILLO, OUTPUT);    // inicializacion del led led amarilla
  pinMode(AZUL, OUTPUT);        // inicializacion del led led azul
  pinMode(ROJO, OUTPUT);        // inicializacion del led led lock
  tiempo1 = millis();           // inicializacion del temporizador
  mensajeInicial();             // mensaje inicial + instrucciones de uso
}
/*******************************LOOP*******************************************/
void loop()
{
  while(Serial.available() == 0)      // mientras no hayan mensajes (cambios de estado) en el puerto serial, ejecuta:
  {
  simulador();                        // simulador
  } 
  int estado = Serial.parseInt();    // recibe el estado desde el puerto serial  (se define un estado, como un numero entero)
  cambiarEstado(estado);             // maquina de estados finitos
}
/*******************************FUNCIONES*******************************************/

/*******************************mensajeInicial*******************************************/
void mensajeInicial()
{
  Serial.print("Bienvenido al simulador: Selectora \n");
  Serial.print("Nota: ** Selectora se inicia en apagado **\n");
  Serial.print("Instrucciones:\n");
  Serial.print("  - Encender: 1\n");
  Serial.print("  - Apagar: 0\n");
  Serial.print("  - Cambiar Direccion: 2\n");
}
/*******************************simulador*******************************************/
void simulador()
{
  if (encendido)          // maquina en el estado encendido
  {
    estadoEncendido();
  }
  else                    // maquina en el estado apagado
  {
    estadoApagado();
  }   
}
/*******************************estadoEncendido*******************************************/
void estadoEncendido()
{
  encenderLed(BLANCO);                          // no es necesario, pero ayuda a mirar el estado del simulador
  //Serial.print("Selectora encendida\n");
  if(lock)                                      // alimentando si el lock está encendido 
  {
    estadoAlimentando();
  }
  else
  {
    estadoTransitando();                        // transitando si el lock está apagado
  }
}
/*******************************estadoApagar*******************************************/
void estadoApagado()
{
  //Serial.print("Selectora apagada\n");        // todo apagado
  apagarLed(BLANCO);
  apagarLed(VERDE);
  apagarLed(AMARILLO);
  apagarLed(AZUL);
  apagarLed(ROJO);
  lock = false;
}
/*******************************cambiarEstado*******************************************/
void cambiarEstado(int estado)                      // maquina recibe estado nuevo
{
  Serial.print("Cambiar estado selectora\n");     
  switch (estado) 
  {
    case 0:                                          // si el estado es 0, apagar simulador
      apagar();
      Serial.print("Apagar selectora\n");
      break;
    case 1:                                         // si el estado es 1, encender simulador
      encender();
      Serial.print("Encender selectora\n");
      break;
    case 2:                                         // si el estado es 2, cambiar direccion a directa o inversa
      cambiarDireccion();
      Serial.print("Cambiar direccion selectora: ");
      imprimirDireccion();
      break;
  }
}
/*******************************estadoAlimentando*******************************************/
void estadoAlimentando()
{
  //Serial.print("Selectora Alimentando \n");
  activarLeds();
  encenderLed(ROJO);
  tiempo2 = millis();               // marca de tiempo para verificar temporizador
  if(tiempo2 > (tiempo1+tiempo_alimentando)){      // si ha pasado mas tiempo que 1/3 segundos, reinicia el temporizador
    tiempo1 = millis();             // reiniciar temporizador
    bloquear();                     // cambia lock para simbolizar el cambio de estado de alimentar a transitar
    avanzar();                      // simula que avanzó a la siguiente posición el selector.
  }
}
/*******************************estadoTransitando*******************************************/
void estadoTransitando(){
  //Serial.print("Selectora Transitando \n");
  apagarLed(VERDE);                 // led 0, verde apagado
  apagarLed(AMARILLO);              // led 1, amarilo apagado
  apagarLed(AZUL);                  // led 2, azul apagado
  apagarLed(ROJO);                  // led lock, rojo apagado
  tiempo2 = millis();
  if(tiempo2 > (tiempo1+tiempo_transitando)){     // si ha pasado mas tiempo que 1/10 segundos, reinicia el temporizador
    tiempo1 = millis();            // reiniciar temporizador
    bloquear();                    // cambia lock para simbolizar el cambio de estado de transitar a alimentar  
  } 
}
/*******************************apagar*******************************************/
void apagar()
{ 
  Serial.print("Apagando Selectora \n");
  encendido = false;                      // apaga el simulador
}
/*******************************encender*******************************************/
void encender()
{ 
  Serial.print("Encendiendo Selectora \n");
  imprimePosicion();                     // imprime la posicion actual
  encendido = true;                      // enciende simulador
}
/*******************************cambiarDireccion*******************************************/
void cambiarDireccion()
{
  direccion = !direccion;
}
/*******************************imprimirDireccion*******************************************/
void imprimirDireccion()
{
  if(direccion)
  {
    Serial.print("Selectora en Directa\n");
  }
  else
  {
    Serial.print("Selectora en Inversa\n");
  }
}
/*******************************activarLeds*******************************************/
void activarLeds()
{
  for(int i = 0; i < LEDS; i++)     // recorremos la cantidad de leds para encenderlos o apagarlos, según la posicion del selector
  {
    bool l = getLed(i);             // rescatamos el valor del led i (0,1 o 2)
    if(l)                           // si el led indica encendido, etonces encender led i (0,1 o 2)
    {
      encenderLed(leds[i]);         // encender led i (0,1 o 2)
    }
    else                            // sino, apagar led i (0,1 o 2)
    { 
      apagarLed(leds[i]);           // apagar led i (0,1 o 2)
    }
  }
}
/*******************************encenderLed*******************************************/
void encenderLed(int color)
{
  digitalWrite(color, HIGH);  // encender el led (0,1 o 2)
}
/*******************************apagarLed*******************************************/
void apagarLed(int color)
{
  digitalWrite(color, LOW);   // apaga el led (0,1 o 2)
}
/*******************************bloquear*******************************************/
void bloquear()
{
  lock = !lock;              // cambia de alimentar a transitar o viseversa
}
/*******************************avanzar*******************************************/
void avanzar()
{
  Serial.print("Cambiar de posicion \n \n");
  
  if(encendido){                                       // si la maquina está encendida
    if(direccion)
    {                          
      if (posicion >= 0 && posicion < POSICIONES-1 )  // avanzar en directa
      {
        posicion ++;
      }
      else
      {
        posicion = 0;  
      }
      imprimePosicion();
    }
    else{                                             
      if (posicion > 0 && posicion <= POSICIONES-1 )  // avanzar en inversa
      {
        posicion --;
      }
      else
      {
        posicion = POSICIONES-1;  
      }
      imprimePosicion();    
    }      
  }
}
/*******************************imprimePosicion*******************************************/
void imprimePosicion()             // se puede presindir de esta funcion, es solo para observar la posicion del selector
{
  Serial.print("Posicion: ");
  Serial.print(posicion);
  Serial.print("\n");
  imprimeLeds();
}
/*******************************imprimePosicion*******************************************/
void imprimeLeds()                // se puede presindir de esta funcion, es solo para observar los leds que se encenderán
{
  for(int i = 0; i < LEDS; i++)
  {
    bool l = getLed(i);
    if(l)
    {
      Serial.print("Led ");
      Serial.print(i);
      Serial.print(" on\n");
    }
    else
    {
      Serial.print("Led ");
      Serial.print(i);
      Serial.print(" off\n");
    }
  }
}
/*******************************getLed*******************************************/
boolean getLed(int n)
{
  return selector[posicion][n];  // retorna la si el led se encenderá o no
}
