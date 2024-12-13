/* Programme du robot M.A.R.K

    GE-A promo 2027

    Baptiste POIDVIN
    Pierre BOURIAUD
    Alexandre GUILBAUD
    Yann DONNARD

    Version 0.B.9 du 21/11/2024

    OPTIMISATION DES VARIABLES #DEFINE
*/

//*************************IMPLEMENTATION DES BIBLIOTHEQUES************************//

#include <Wire.h>  //bibliothèque nécessaire pour la communication I2C
#include "Ultrasonic.h" //bibliothèque nécessaire pour les capteurs ultrasons
#include <Encoder.h>
#include "rgb_lcd.h"


//**************************INITIALISATION DES VARIABLES***************************//

// Variables a paramétrer
#define distanceCritiqueCourse 20                 // Définition de la distance critique à 20 cm
#define distanceArretCourse 5                     // Définition de la distance d'arret du robot à 5 cm


// Déclaration des entrées/sorties
#define joystickX A2                // Axe X du joystick sur le pin A2
#define joystickY A3                // Axe Y du joystick sur le pin A3
#define ledInterne 13               // LED interne sur le pin TOR 12
#define capteurInfrarouge 6         // Capteur Infrarouge sur le pin TOR 6
#define bumperGauche 3             // Capteur moustache de gauche sur le pin TOR 2
#define bumperDroite 2             // Capteur moustache de droite sur le pin TOR 3
Ultrasonic capteurUltrasonsFrontal(8);  // Capteur ultrasons frontal
Ultrasonic capteurUltrasonsGauche(10);  // Capteur ultrasons de gauche
Ultrasonic capteurUltrasonsDroite(12);  // Capteur ultrasons de droite
Encoder encodeurDroite(18, 29);         // Initialisation de l'encodeur de gauche
Encoder encodeurGauche(27, 19);         // Initialisation de l'encodeur de droite


// Variables du programme
bool course = false;                // Variable pour être dans la couse
bool finCourse = false;             // Variable pour signaler de la fin de course (réussite ou non)
byte etape = 1;                     // Variable pour savoir sans quelle étape est le robot

// Temps
unsigned long tpsDemarrageCourse;       // Variable recuperant la date de départ du robot
unsigned long tpsFinCourse;             // Variable recuperant la date d'arrivee du robot
unsigned long tpsDebutBoucle;           // Variable recuperant la date du debut de l'exécution d'une boucle
unsigned long tpsFinBoucle;             // Variable recuperant la date de fin d'exécution d'une boucle
// Capteurs ultrasons
int valeurCuFrontal, valeurCuGauche, valeurCuDroite;  // Données des capteurs ultrasons
int compteurDistanceCritique = 0;                     // Valeur calculée du nombre de fois ou le robot est a une distace critique d'un mur
bool distanceCritique5cmIn;                           // Bit a 1 lorsque le robot détecte qu'il est a une distance inférieure a 5cm
bool distanceCritique20cmIn;                          // Bit a 1 lorsque le robot détecte qu'il est a une distance inférieure a 20cm
//Encodeurs
unsigned long positionEncodeurGauche = 0;             // Position de la roue encodeuses de droite du robot
unsigned long positionEncodeurDroite = 0;             // Position des roues encodeuses du robot

//**************************INITIALISATION DES FONCTIONS***************************//

// fonction du calcul du temps d'execution de la boucle principale
unsigned long tpsExecProg(unsigned long tpsDebut, unsigned long tpsFin) { // Fonction calculant le temps d'exécution du programme : nécessite en entrée un temps 1 et 2 pour calculer le delta
  unsigned long resultat = tpsFin - tpsDebut;
  return resultat;  // renvoi du delta sur la variable de la fonction
}

// fonction de calcul du nombre de fois que le robot se trouve à une distance critique d'un mur et mise a 1 d'un bit si le robot a dépassé ce seuil
// Cette fonction est optimisée pour être utilisée pour le seuil critique et le seuil d'arret, d'où les paramètres booléens en entrée
int calculNbDistanceCritique(int seuilDistanceCritique, bool calculSeuil, bool &distCritiqueOn) { // Condition de front montant lorsque le robot entre dans une zone ou il est a une distance critique d'un mur
  if ((valeurCuFrontal <= seuilDistanceCritique|| valeurCuDroite <= seuilDistanceCritique || valeurCuGauche <= seuilDistanceCritique) && !distCritiqueOn) {
    if(calculSeuil == true){ // Fonction uniquement dédiée au compteur du nombre de fois ou le robot est a moins de 20 cm d'un mur
    compteurDistanceCritique++;
    }
    distCritiqueOn = true;
  } // Condition de front descendant lorsque le robot quitte une zone de distance critique d'un mur
  else if (valeurCuFrontal > seuilDistanceCritique && valeurCuDroite > seuilDistanceCritique && valeurCuGauche > seuilDistanceCritique && distCritiqueOn) {
    distCritiqueOn = false;
  }
  return distCritiqueOn; // Renvoyer en paramètre de sortie si le robot à une distance d'un mur inférieur au seuil. (pour le seuil d'arrêt)
}

int finDeCourse(){
  course=false; // arrêt de la couse
}

//*************************INITIALISATION DU CONTROLLEUR***************************//
void setup(void) {
  // Initialisation des pins (entrées / sorties)
  pinMode(ledInterne, OUTPUT); // Led interne (pin D13) en sortie
  pinMode(joystickX, INPUT); // Axe X du Joystick (pin A2) en entrée
  pinMode(joystickY, INPUT); // Axe Y du Joystick (pin A3) en entrée
  pinMode(bumperGauche, INPUT_PULLUP);
  pinMode(bumperDroite, INPUT_PULLUP);

  // Initialisation des interruptions
  /*
  attachInterrupt(digitalPinToInterrupt(bumperGauche), finDeCourse, CHANGE);
  attachInterrupt(digitalPinToInterrupt(bumperDroite), finDeCourse, CHANGE);
  attachInterrupt(digitalPinToInterrupt(capteurInfrarouge), finDeCourse, CHANGE);
  */
  // Initialisation de la communication avec le moniteur série
  Serial.begin(9600); // Initialisation du moniteur série à 9600 bauds
}


//*********************************BOUCLE PRINCIPALE********************************//
void loop(void) {
/*
  while (digitalRead(capteurInfrarouge)) {Serial.println("souleve");} // Attendre que le robot soit au sol

  while (digitalRead(bumperGauche)) {Serial.println("btn relache");}  // Attendre que le bouton soit appuyé

  while (!digitalRead(bumperGauche)) {Serial.println("btn appuye");}  // Attendre que le bouton soit relaché

  Serial.println("GOGOGOGOGOGOOGO");
  delay(2000);
*/
  course = true;

  //****************************LE ROBOT DEBUTE SA COURSE***************************//

  tpsDemarrageCourse = millis(); // Enregistrement du temps de départ de la course par rapport à l'horloge du microcontroleur
  while (course) {
    // GESTION DU TEMPS
    tpsDebutBoucle = millis(); // Enregistrement du temps du début de l'exécution de la boucle principale
    // FIN GESTION DU TEMPS

    // Acquisition des capteurs
    valeurCuFrontal = capteurUltrasonsFrontal.MeasureInCentimeters();
    valeurCuGauche = capteurUltrasonsGauche.MeasureInCentimeters();
    valeurCuDroite = capteurUltrasonsDroite.MeasureInCentimeters();
    positionEncodeurGauche = encodeurGauche.read();
    positionEncodeurDroite = encodeurDroite.read();

    // Calcul des seuils de distance avec les capteurs ultrasons
    calculNbDistanceCritique(distanceCritiqueCourse,true,distanceCritique20cmIn);
    calculNbDistanceCritique(distanceArretCourse,false,distanceCritique5cmIn);


    Serial.print("Gauche "); Serial.println(positionEncodeurGauche);

    // GESTION DU TEMPS 2
    tpsFinBoucle = millis(); // Calcul du temps de fin d'exécution de la boucle principale
    //Serial.println(tpsExecProg(tpsDebutBoucle, tpsFinBoucle)); // Calcul du temps d'exécution de la boucle principale avec le temps du début et de fin. A NOTER QUE LE TEMPS D'EXECUTION NE PRENDS PAS EN COMPTE LE TEMPS DE CALCUL ET D'AFFICHAGE DE LA BOUCLE
    // FIN GESTION DU TEMPS 2

    // GESTION DE L'ARRET DE LA COURSE
    // Si le robot est soulevé ou si le robot est en course depuis plus de 5 min ou si le robot est a une distance d'un mur inférieure au seuil d'arrêt
    /*if (digitalRead(capteurInfrarouge)
     || millis() >= tpsDemarrageCourse + 300000
     || distanceCritique5cmIn)
     || !digitalRead(bumperGauche)
     || !digitalRead(bumperDroite){
      break;
    }*/
  }

  //*************************LE ROBOT A FINI SA COURSE************************//
  while (1) {
    Serial.println("course finie");
    delay(1000);
  }

}
