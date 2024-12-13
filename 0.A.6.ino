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

//  int i =550;    //valeur moteur gauche
//  int j =500;    //valeur moteur droite

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




void loop() {

  float D = ultrasonicdroite.MeasureInCentimeters();
  float G = ultrasonicgauche.MeasureInCentimeters();
  float DV = ultrasonicfrontal.MeasureInCentimeters();
  lcd.setCursor(0, 0);
  lcd.print(D);
  lcd.setCursor(0, 1);
  lcd.print(G);
  Serial.begin(9600);

  //   //REGULATION TOUT DROIT
  //  if (D > 40 and G > 40)
  //  {
  //    i=550;
  //    j=500;
  //  }
  //
  //  if (D > 45 and G > 35)
  //  {
  //    i=560;
  //    j=500;
  //  }
  //  if (D > 35 and G > 45)
  //  {
  //    i=550;
  //    j=510;
  //  }
  //
  //  if (D > 55 and G > 25)
  //  {
  //    i=580;
  //    j=500;
  //  }
  //  if (D > 25 and G > 55)
  //  {
  //    i=550;
  //    j=530;
  //  }

  if (D < G) {
    MoteurGD(505, 490);
  }
  if (G < D) {
    MoteurGD(620, 550);
  }

  if (D < 20) {
    MoteurGD(505, 495);
  }
  if (G < 20) {
    MoteurGD(630, 550);
  }

  //  //MUR DEVANT
  //  if (DV<21)
  //  {
  //  MoteurGD(400, 400);
  //  if(D<G)
  //  {
  //  MoteurGD(400, 550);
  //  delay(800);
  //  MoteurGD(550, 500);
  //  delay(2500);
  //  }
  //  if(G<D)
  //  {
  //  MoteurGD(550, 400);
  //  delay(800);
  //  MoteurGD(550, 500);
  //  delay(2500);
  //  }
  //  }


  // MoteurGD(i, j);
  // delay(35);

  if (digitalRead(bumperLeft) == 0 or digitalRead(bumperRight) == 0) {
    MoteurGD(400, 400);
    exit(0);
  }
}
