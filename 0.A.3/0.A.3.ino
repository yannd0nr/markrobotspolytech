#include <Wire.h>
#include "Ultrasonic.h"
#include "rgb_lcd.h"

#define Thash 800
#define Stop 400
#define Vmax Thash

// Macros

#define LedToggle digitalWrite(13, !digitalRead(13))
#define MoteurG(Vg) OCR5A = Vg  // Vg in [0... 1999]
#define MoteurD(Vd) OCR5B = Vd  // VD in [0... 1999]
#define MoteurGD(Vg, Vd) \
  MoteurG(Vg); \
  MoteurD(Vd)
#define StopMoteurGD MoteurGD(Stop, Stop)

void initMoteurs() {  // MoteurG :OC5A=PIN46-PL3, MoteurD : OC5B=PIN45-PL4

  DDRL = 0x18;  // PL3 et PL4
  DDRB = 0x80;  // PB7 LedToggle
  // COM5B_1:0 = 10 -> clear sur egalite++, set sur egalite--
  // WGM5_3:1 = 1000 -> mode 8 => ICR5 defini le TOP
  TCCR5A = (1 << COM5A1) + (1 << COM5B1);
  TCCR5B = (1 << ICNC5) + (1 << WGM53) + (1 << CS50);  // CS_12:10 = 001 ->prediv par 1
  ICR5 = Thash;                                        // 1999 correspond a f = 4khz
  StopMoteurGD;

  // Interrution de débordement du timer
  TIMSK5 = 1 << TOIE5;
}

ISR(TIMER5_OVF_vect) {  // Pour la lecture du courant
  LedToggle;
}

rgb_lcd lcd;
Ultrasonic ultrasonicfrontal(8);
Ultrasonic ultrasonicgauche(10);
Ultrasonic ultrasonicdroite(12);


void setup() {

  pinMode(43, OUTPUT);
  digitalWrite(43, 0);
  initMoteurs();
  sei();
  digitalWrite(43, 1);
  Serial.begin(9600);
}
void loop() {

  // REGULATION TOUT DROIT
  if (ultrasonicdroite.MeasureInCentimeters() > 20 and ultrasonicgauche.MeasureInCentimeters() > 20) {
    MoteurGD(500, 500);
  }
  // REGULATION MUR A DROITE
  if (ultrasonicdroite.MeasureInCentimeters() < 20) {
    MoteurGD(500, 700);
  }

  // REGULATION MUR A GAUCHE
  if (ultrasonicgauche.MeasureInCentimeters() < 20) {
    MoteurGD(700, 500);
  }
}
