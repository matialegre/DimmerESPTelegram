#include <ESP8266TimerInterrupt.h>

volatile int i=0;
volatile boolean cruce_cero=0;
int Triac[6] = {5, 4, 0, 2, 14, 12}; // Define los pines de los triacs
int dim[6]; // Define el nivel de dimming para cada triac
int T_int = 100;
int currentTriac = 0; // Variable para llevar la cuenta del triac actual

unsigned long lastZeroCrossMillis = 0; 
unsigned int zeroCrossCount = 0; 

unsigned long previousMillis = 0; 
unsigned long interval = 20; 
bool increasing = true; 

ESP8266Timer ITimer;

void setup() { 
  Serial.begin(9600);  
  for(int j=0; j<6; j++) {
    pinMode(Triac[j], OUTPUT);
  }
  attachInterrupt(digitalPinToInterrupt(13), deteccion_Cruce_cero, RISING);
  ITimer.attachInterruptInterval(T_int, Dimer);
}                                                            

ICACHE_RAM_ATTR void deteccion_Cruce_cero() 
{
  cruce_cero = true;
  i=0;
  for(int j=0; j<6; j++) {
    digitalWrite(Triac[j], LOW);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastZeroCrossMillis >= 1000) { 
    float frequency = zeroCrossCount / 2.0; 
    Serial.print("Frecuencia: ");
    Serial.print(frequency);
    Serial.println(" Hz");
    lastZeroCrossMillis = currentMillis;
    zeroCrossCount = 0;
  } else {
    zeroCrossCount++;
  }
}   

ICACHE_RAM_ATTR void Dimer() 
{                   
  if (cruce_cero == true ){
    for(int j=0; j<6; j++) {
      if (i>=dim[j] ) {
        digitalWrite(Triac[j], HIGH);
      }
    }
    if(i>=100) {
      i=0;
      cruce_cero=false;
    }
    else {
      i++;
    }
  }
}

void loop ()
{
  unsigned long currentMillis = millis();
  static bool decrementing = true; // Variable para controlar la dirección del dimming

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    for(int j=0; j<6; j++) {
      if (j == currentTriac) {
        if (decrementing && dim[j] > 0) {
          dim[j]--;
        } else if (!decrementing && dim[j] < 100) {
          dim[j]++;
        } 

        if(dim[j] == 0) {
          decrementing = false; // Cambia la dirección del dimming a incrementar
        } else if (dim[j] == 100 && !decrementing) {
          decrementing = true; // Cambia la dirección del dimming a decrementar
          currentTriac = (currentTriac + 1) % 6; // Pasa al siguiente triac
        }
      } else {
        dim[j] = 100;
      }
    }
  }
}
