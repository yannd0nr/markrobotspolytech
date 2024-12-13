/* Programme du robot M.A.R.K

    GE-A promo 2027

    Baptiste POIDVIN
    Pierre BOURIAUD
    Alexandre GUILBAUD
    Yann DONNARD

    Version 0.B.4 du 14/11/2024
*/

/* Initialisation des variables */

int joystickX = A2; // Axe X du joystick sur le pin A2
int joystickY = A3; // Axe Y du joystick sur le pin A3
int ledInterne = 13; // LED interne sur le pin TOR 12
int capteurInfrarouge = 6; // Capteur Infrarouge sur le pin TOR 6

// Variables de temps
unsigned long tpsDemarrageCourse;  // Variable recuperant la date de départ du robot
unsigned long tpsFinCourse; // Variable recuperant la date d'arrivee du robot
unsigned long tpsDebutBoucle; // Variable recuperant la date du debut de l'exécution d'une boucle
unsigned long tpsFinBoucle; // Variable recuperant la date de fin d'exécution d'une boucle
unsigned long tpsExecProg2;

// Fonctions

// fonction du calcul du temps d'execution de la boucle principale
unsigned long tpsExecProg(unsigned long tpsDebut, unsigned long tpsFin) { // Fonction calculant le temps d'exécution du programme : nécessite en entrée un temps 1 et 2 pour calculer le delta
  unsigned long resultat = tpsFin - tpsDebut;
  return resultat;  // renvoi du delta sur la variable de la fonction
}

// fonction permettant de créer un bouton virtuel à l'aide du joystick
bool boutonDemarrage(void) {            // Variable du bouton de démarrage converti avec le joystick
  if (analogRead(joystickX) == 1023) {  // Mise à 1 lorsque l'axe X est à 1203 : cf. fiche de tests
    return (true);
  }
  else {                                // Sinon bouton à 0
    return (false);
  }
}


// Initialisation du controleur, des pins et de la communication
void setup(void) {
  // Initialisation des pins (entrées / sorties)

  pinMode(ledInterne, OUTPUT); // Led interne (pin D13) en sortie
  pinMode(joystickX, INPUT); // Axe X du Joystick (pin A2) en entrée
  pinMode(joystickY, INPUT); // Axe Y du Joystick (pin A3) en entrée
  Serial.begin(9600); // Initialisation du moniteur série à 9600 bauds
}

//**************************** Boucle principale****************************//
void loop(void) {


  while (digitalRead(capteurInfrarouge) == true) {  // Attendre que le robot soit au sol
    Serial.println("en attente 2");
  }

    while (boutonDemarrage() == false) {            // Attendre un appui sur le bouton
    Serial.println("en attente");
  }

    while (boutonDemarrage() == true) {             // Attendre le relachement du bouton
    Serial.println("en attente");
  }


  while (1) { // Le robot commence sa course
    tpsDemarrageCourse = millis(); // Enregistrement du temps de départ de la course par rapport à l'horloge du microcontroleur
    tpsDebutBoucle = millis(); // Enregistrement du temps du début de l'exécution de la boucle principale
    digitalWrite(ledInterne, HIGH);
    delay(100);
    digitalWrite(ledInterne, LOW);
    delay(100);
    tpsFinBoucle = millis(); // Calcul du temps de fin d'exécution de la boucle principale
    Serial.println(tpsExecProg(tpsDebutBoucle, tpsFinBoucle)); // Calcul du temps d'exécution de la boucle principale avec le temps du début et de fin. A NOTER QUE LE TEMPS D'EXECUTION NE PRENDS PAS EN COMPTE LE TEMPS DE CALCUL ET D'AFFICHAGE DE LA BOUCLE
  }

}
