/* Programme du robot M.A.R.K

    GE-A promo 2027

    Baptiste POIDVIN
    Pierre BOURIAUD
    Alexandre GUILBAUD
    Yann DONNARD

    Version 0.B.6 du 15/11/2024

    NETTOYAGE DU PROGRAMME ET GESTION DE LA FIN DE COURSE
*/

//*************************IMPLEMENTATION DES BIBLIOTHEQUES************************//

#include <Wire.h>  //bibliothèque nécessaire pour la communication I2C
#include "Ultrasonic.h" //bibliothèque nécessaire pour les capteurs ultrasons


//**************************INITIALISATION DES VARIABLES***************************//

// Déclaration des entrées/sorties
const int joystickX = A2;               // Axe X du joystick sur le pin A2
const int joystickY = A3;               // Axe Y du joystick sur le pin A3
const int ledInterne = 13;              // LED interne sur le pin TOR 12
const int capteurInfrarouge = 6;        // Capteur Infrarouge sur le pin TOR 6
const byte bumperGauche = 3;            // Capteur moustache de gauche sur le pin TOR 2
const byte bumperDroite = 2;            // Capteur moustache de droite sur le pin TOR 3
Ultrasonic capteurUltrasonsFrontal(8);  // Capteur ultrasons frontal
Ultrasonic capteurUltrasonsGauche(10);  // Capteur ultrasons de gauche
Ultrasonic capteurUltrasonsDroite(12);  // Capteur ultrasons de droite

// Variables de temps
unsigned long tpsDemarrageCourse;       // Variable recuperant la date de départ du robot
unsigned long tpsFinCourse;             // Variable recuperant la date d'arrivee du robot
unsigned long tpsDebutBoucle;           // Variable recuperant la date du debut de l'exécution d'une boucle
unsigned long tpsFinBoucle;             // Variable recuperant la date de fin d'exécution d'une boucle

// Variables du programme
int valeurCuFrontal, valeurCuGauche, valeurCuDroite; // Données des capteurs ultrasons
int compteurDistanceCritique = 0;                      // Valeur calculée du nombre de fois ou le robot est a une distace critique d'un mur
const int distanceCritiqueCourse = 20;                // Définition de la distance critique à 20 cm.
const int distanceArretCourse = 5;
bool distanceCritique5cmIn;
bool distanceCritique20cmIn;
//**************************INITIALISATION DES FONCTIONS***************************//

// fonction du calcul du temps d'execution de la boucle principale
unsigned long tpsExecProg(unsigned long tpsDebut, unsigned long tpsFin) { // Fonction calculant le temps d'exécution du programme : nécessite en entrée un temps 1 et 2 pour calculer le delta
  unsigned long resultat = tpsFin - tpsDebut;
  return resultat;  // renvoi du delta sur la variable de la fonction
}

// fonction de calcul du nombre de fois que le robot se trouve à une distance critique d'un mur
int calculNbDistanceCritique(int seuilDistanceCritique, bool calculSeuil, bool &distCritiqueOn) { // Condition de front montant lorsque le robot entre dans une zone ou il est a une distance critique d'un mur
  if ((valeurCuFrontal <= seuilDistanceCritique || valeurCuDroite <= seuilDistanceCritique || valeurCuGauche <= seuilDistanceCritique) && !distCritiqueOn) {
    if(calculSeuil == true){
    compteurDistanceCritique++;
    }
    distCritiqueOn = true;
  } // Condition de front descendant lorsque le robot quitte une zone de distance critique d'un mur
  else if (valeurCuFrontal > seuilDistanceCritique && valeurCuDroite > seuilDistanceCritique && valeurCuGauche > seuilDistanceCritique && distCritiqueOn) {
    distCritiqueOn = false;
  }
  return distCritiqueOn;
}

//*************************INITIALISATION DU CONTROLLEUR***************************//
void setup(void) {
  // Initialisation des pins (entrées / sorties)

  pinMode(ledInterne, OUTPUT); // Led interne (pin D13) en sortie
  pinMode(joystickX, INPUT); // Axe X du Joystick (pin A2) en entrée
  pinMode(joystickY, INPUT); // Axe Y du Joystick (pin A3) en entrée
  pinMode(bumperGauche, INPUT_PULLUP);
  pinMode(bumperDroite, INPUT_PULLUP);
  Serial.begin(9600); // Initialisation du moniteur série à 9600 bauds
}


//*********************************BOUCLE PRINCIPALE********************************//
void loop(void) {

  while (digitalRead(capteurInfrarouge)) {
    Serial.println("souleve"); // Attendre que le robot soit au sol
  }

  while (digitalRead(bumperGauche)) {
    Serial.println("btn relache"); // Attendre que le bouton soit appuyé
  }

  while (!digitalRead(bumperGauche)) {
    Serial.println("btn appuye"); // Attendre que le bouton soit relaché
  }

  Serial.println("GOGOGOGOGOGOOGO");
  delay(2000);

  //****************************LE ROBOT DEBUTE SA COURSE***************************//

  tpsDemarrageCourse = millis(); // Enregistrement du temps de départ de la course par rapport à l'horloge du microcontroleur
  while (1) {
    // GESTION DU TEMPS
    tpsDebutBoucle = millis(); // Enregistrement du temps du début de l'exécution de la boucle principale
    // FIN GESTION DU TEMPS

    // Acquisition des capteurs
    valeurCuFrontal = capteurUltrasonsFrontal.MeasureInCentimeters();
    valeurCuGauche = capteurUltrasonsGauche.MeasureInCentimeters();
    valeurCuDroite = capteurUltrasonsDroite.MeasureInCentimeters();

    calculNbDistanceCritique(distanceCritiqueCourse,true,distanceCritique20cmIn);
    calculNbDistanceCritique(distanceArretCourse,false,distanceCritique5cmIn);
    Serial.print("Distance critique 5m : ");
    Serial.println(distCritique5cm);
    Serial.print("Nb istance critique 20m : ");
    Serial.println(compteurDistanceCritique);
    // GESTION DU TEMPS 2
    tpsFinBoucle = millis(); // Calcul du temps de fin d'exécution de la boucle principale
    //Serial.println(tpsExecProg(tpsDebutBoucle, tpsFinBoucle)); // Calcul du temps d'exécution de la boucle principale avec le temps du début et de fin. A NOTER QUE LE TEMPS D'EXECUTION NE PRENDS PAS EN COMPTE LE TEMPS DE CALCUL ET D'AFFICHAGE DE LA BOUCLE
    // FIN GESTION DU TEMPS 2

    // GESTION DE L'ARRET DE LA COURSE
    // Si le robot est soulevé ou si le robot est en course depuis plus de 5 min ou si le robot est dans une zone inférieure à
    if (digitalRead(capteurInfrarouge) || millis() >= tpsDemarrageCourse + 300000) {
      break;
    }
  }

  //*************************LE ROBOT A FINI SA COURSE************************//
  while (1) {
    Serial.println("course finie");
  }

}
