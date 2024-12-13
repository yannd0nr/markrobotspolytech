#include <Wire.h>
#include "Ultrasonic.h"
#include "rgb_lcd.h"

#define Thash 800
#define Stop 400
#define Vmax Thash

const byte bumperLeft = 3;
const byte bumperRight = 2;

//variable pour étape

int etape = 1;


// Macros

#define LedToggle digitalWrite(13, !digitalRead(13))
#define MoteurG(Vg) OCR5A = Vg + 10  // Vg in [0... 1999]  correction + 2 pour l avance droit statique du prof qui le dit
#define MoteurD(Vd) OCR5B = Vd       // VD in [0... 1999]
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

void loop() {


  float D = ultrasonicdroite.MeasureInCentimeters();
  float G = ultrasonicgauche.MeasureInCentimeters();
  float DV = ultrasonicfrontal.MeasureInCentimeters();
  lcd.setCursor(0, 0);
  lcd.print(D);
  lcd.setCursor(0, 1);
  lcd.print(G);

  if (etape == 1) {
    lcd.setRGB(255, 0, 0);
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

    if (G > 90) {
      etape = 2;
    }
  }
  if (etape == 2) {
    lcd.setRGB(0, 255, 0);

    if (D < 65) {
      MoteurGD(505, 490);
    }
    if (D > 68) {
      MoteurGD(620, 550);
    }
    if (D > 65 and D < 68) {
      MoteurGD(550, 500);
    }

    if (DV < 30) {
      MoteurGD(400, 400);
      delay(1000);
      etape = 3;
      MoteurGD(400, 600);
      delay(400);
    }
  }
  if (etape == 3) {
    lcd.setRGB(0, 0, 255);

    if (D < 20) {
      MoteurGD(505, 490);
    }
    if (D > 30) {
      MoteurGD(620, 550);
    }
    if (D > 20 and D < 30) {
      MoteurGD(550, 500);
    }
    if (D > 90) {
      lcd.setRGB(0, 255, 255);
      MoteurGD(550, 500);
      etape = 4;
      delay(600);
      MoteurGD(400, 400);
      delay(1000);
      MoteurGD(600, 400);
      delay(300);
      MoteurGD(550, 500);
      delay(2000);
    }
  }
  if (etape == 4) {
    lcd.setRGB(50, 200, 100);

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

    if (DV < 30) {
      MoteurGD(400, 400);
    }
  }
  if (digitalRead(bumperLeft) == 0 or digitalRead(bumperRight) == 0) {
    MoteurGD(400, 400);
    exit(0);
  }
}
