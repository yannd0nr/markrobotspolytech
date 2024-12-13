#include <Wire.h>
#include "Ultrasonic.h"
#include "rgb_lcd.h"

#define Thash 800
#define Stop 400
#define Vmax Thash

const byte bumperLeft = 3;
const byte bumperRight = 2;

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
Ultrasonic ultrasonicgauche(12);
Ultrasonic ultrasonicdroite(10);



void setup() {

  pinMode(43, OUTPUT);
  digitalWrite(43, 0);
  initMoteurs();
  sei();
  digitalWrite(43, 1);
  lcd.begin(16, 2);
  pinMode(bumperLeft, INPUT_PULLUP);
  pinMode(bumperRight, INPUT_PULLUP);
}

float Regulation(float coefficient, float USDroite, float USGauche) {

  // REGULATION TOUT DROIT
  if (USDroite > 35 and USGauche > 35) {
    MoteurGD(600 * coefficient, 600);
  }


  //REGULATION MUR A DROITE
  if (USDroite < 35) {
    MoteurGD(600 * coefficient, 640);
  }

  //REGULATION MUR A GAUCHE
  if (USGauche < 35) {
    MoteurGD(640 * coefficient, 600);
  }
}
void loop() {
  float coef = 1.17;
  float D = ultrasonicdroite.MeasureInCentimeters();
  float G = ultrasonicgauche.MeasureInCentimeters();
  lcd.setCursor(0, 0);
  lcd.print(D);
  lcd.setCursor(0, 1);
  lcd.print(G);

  Regulation(1.17, D, G);


  if (digitalRead(bumperLeft) == 0 or digitalRead(bumperRight) == 0) {
    MoteurGD(400, 400);
    exit(0);
  }
}
