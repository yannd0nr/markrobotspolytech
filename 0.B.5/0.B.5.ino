/* Programme du robot M.A.R.K

    GE-A promo 2027

    Baptiste POIDVIN
    Pierre BOURIAUD
    Alexandre GUILBAUD
    Yann DONNARD

    Version 0.5 du 14/11/2024

    AJOUT DES CAPTEURS ULTRASONS ET COMPTAGE DU NOMBRE DE FOIS A UNE DISTANCE INFERIEURE DE 20 CM
*/

//*************************IMPLEMENTATION DES BIBLIOTHEQUES************************//

#include <Wire.h>  //bibliothèque nécessaire pour la communication I2C
#include "Ultrasonic.h" //bibliothèque nécessaire pour les capteurs ultrasons


//**************************INITIALISATION DES VARIABLES***************************//

// Déclaration des entrées/sorties
const int joystickX = A2;                     // Axe X du joystick sur le pin A2
const int joystickY = A3;                     // Axe Y du joystick sur le pin A3
const int ledInterne = 13;                    // LED interne sur le pin TOR 12
const int capteurInfrarouge = 6;              // Capteur Infrarouge sur le pin TOR 6
const byte bumperGauche = 3;                  // Capteur moustache de gauche sur le pin TOR 2
const byte bumperDroite = 2;                  // Capteur moustache de droite sur le pin TOR 3
Ultrasonic capteurUltrasonsFrontal(8);  // Capteur ultrasons frontal
Ultrasonic capteurUltrasonsGauche(10);  // Capteur ultrasons de gauche
Ultrasonic capteurUltrasonsDroite(12);  // Capteur ultrasons de droite


// Variables de temps
unsigned long tpsDemarrageCourse;       // Variable recuperant la date de départ du robot
unsigned long tpsFinCourse;             // Variable recuperant la date d'arrivee du robot
unsigned long tpsDebutBoucle;           // Variable recuperant la date du debut de l'exécution d'une boucle
unsigned long tpsFinBoucle;             // Variable recuperant la date de fin d'exécution d'une boucle

// Variables du programme
int valeurCuFrontal,valeurCuGauche,valeurCuDroite;
int nombreDistanceCritique = 0;
bool distCritiqueOn = false;


//**************************INITIALISATION DES FONCTIONS***************************//

// fonction du calcul du temps d'execution de la boucle principale
unsigned long tpsExecProg(unsigned long tpsDebut, unsigned long tpsFin) { // Fonction calculant le temps d'exécution du programme : nécessite en entrée un temps 1 et 2 pour calculer le delta
  unsigned long resultat = tpsFin - tpsDebut;
  return resultat;  // renvoi du delta sur la variable de la fonction
}

//fonction de calcul du nombre de fois que le robot se trouve a moins de 20 cm d'un mur
int calculNbDistanceCritique(){
  if ((valeurCuFrontal<=20 || valeurCuDroite<=20 || valeurCuGauche<=20) && !distCritiqueOn){
    nombreDistanceCritique++;
    distCritiqueOn=true;
  }
  else if(valeurCuFrontal>20 && valeurCuDroite>20 && valeurCuGauche>20 && distCritiqueOn){
    distCritiqueOn=false;
  }
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


  while (digitalRead(capteurInfrarouge) == true) {Serial.println("souleve");}  // Attendre que le robot soit au sol

  while (digitalRead(bumperGauche) == true) {Serial.println("btn relache");}  // Attendre que le bouton soit appuyé

  while (digitalRead(bumperGauche) == false) {Serial.println("btn appuye");}  // Attendre que le bouton soit relaché

  Serial.println("GOGOGOGOGOGOOGO");
  delay(2000);

  //****************************LE ROBOT DEBUTE SA COURSE***************************//

  while (1) { // Boucle infinie
    // GESTION DU TEMPS
    tpsDemarrageCourse = millis(); // Enregistrement du temps de départ de la course par rapport à l'horloge du microcontroleur
    tpsDebutBoucle = millis(); // Enregistrement du temps du début de l'exécution de la boucle principale
   // FIN GESTION DU TEMPS

    // Acquisition des capteurs
    valeurCuFrontal=capteurUltrasonsFrontal.MeasureInCentimeters();
    valeurCuGauche=capteurUltrasonsGauche.MeasureInCentimeters();
    valeurCuDroite=capteurUltrasonsDroite.MeasureInCentimeters();

    calculNbDistanceCritique();

    Serial.println(nombreDistanceCritique);
    // GESTION DU TEMPS 2
    tpsFinBoucle = millis(); // Calcul du temps de fin d'exécution de la boucle principale
    //Serial.println(tpsExecProg(tpsDebutBoucle, tpsFinBoucle)); // Calcul du temps d'exécution de la boucle principale avec le temps du début et de fin. A NOTER QUE LE TEMPS D'EXECUTION NE PRENDS PAS EN COMPTE LE TEMPS DE CALCUL ET D'AFFICHAGE DE LA BOUCLE
    // FIN GESTION DU TEMPS 2
  }

}

void demarrageArretRobot(){

}
