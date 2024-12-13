#include <Wire.h>
#include "Ultrasonic.h"
#include "rgb_lcd.h"

#define Thash 800
#define Stop 400
#define Vmax Thash

#define LedToggle digitalWrite(13, !digitalRead(13))
#define MoteurG(Vg) OCR5A = (Vg)
#define MoteurD(Vd) OCR5B = (Vd)
#define MoteurGD(Vg, Vd) \
  MoteurG(Vg); \
  MoteurD(Vd)
#define StopMoteurGD MoteurGD(Stop, Stop)

rgb_lcd lcd;
Ultrasonic ultrasonicfrontal(8);
Ultrasonic ultrasonicgauche(10);
Ultrasonic ultrasonicdroite(12);

ISR(TIMER5_OVF_vect) {
  LedToggle;
}

void setup() {
  pinMode(43, OUTPUT);
  digitalWrite(43, 0);
  DDRL = 0x18;  // Configure les broches 46 (PL3) et 45 (PL4) pour les moteurs
  DDRB = 0x80;  // Configure la broche 13 pour le contrôle de la LED

  // Configuration du Timer 5 en mode 14 (Fast PWM avec ICR5 comme TOP)
  TCCR5A = (1 << COM5A1) | (1 << COM5B1);              // PWM non inversé sur OCR5A et OCR5B
  TCCR5B = (1 << WGM53) | (1 << WGM52) | (1 << CS50);  // Mode 14, prescaler à 1

  ICR5 = Thash;         // Définit la valeur TOP (fréquence du PWM)
  StopMoteurGD;         // Arrêter les moteurs au démarrage
  TIMSK5 = 1 << TOIE5;  // Activer l'interruption de dépassement du Timer 5
  sei();
  digitalWrite(43, 1);

  Serial.begin(9600);
}

void loop() {
  MoteurGD(200, 200);
}
