/* Programme du robot M.A.R.K

    GE-A promo 2027

    Baptiste POIDVIN
    Pierre BOURIAUD
    Alexandre GUILBAUD
    Yann DONNARD

    Version 0.3 du 25/10/2024
*/

/* Initialisation des variables */

int joystickX = A2; // Axe X du joystick sur le pin A2
int joystickY = A3; // Axe Y du joystick sur le pin A3
int ledInterne = 13; // LED interne sur le pin TOR 12
int capteurInfrarouge = 6; // Capteur Infrarouge sur le pin TOR 6

// Variables de temps
unsigned long tpsDemarrageProg;  // Variable recuperant la date de départ du robot
unsigned long tpsArretProg; // Variable recuperant la date d'arrivee du robot
unsigned long tpsDebutBoucle; // Variable recuperant la date du debut de l'exécution d'une boucle
unsigned long tpsFinBoucle; // Variable recuperant la date de fin d'exécution d'une boucle
unsigned long tpsExecProg2;

// Fonctions
unsigned long tpsExecProg(unsigned long tpsDebut, unsigned long tpsFin) { // Fonction calculant le temps d'exécution du programme
  unsigned long resultat = tpsFin - tpsDebut;
  return resultat;
}

bool boutonDemarrage(void) {            // Variable du bouton de démarrage converti avec le joystick
  if (analogRead(joystickX) == 1023) {  // Mise à 1 lorsque l'axe X est à 1203 cf. fiche de tests
    return (true);
  }
  else {                                // Sinon bouton à 0
    return (false);
  }
}

void setup(void) {
  // Initialisation des pins (entrées / sorties)

  pinMode(ledInterne, OUTPUT);
  pinMode(joystickX, INPUT);
  pinMode(joystickY, INPUT);
  Serial.begin(9600);
}

void loop(void) { // Démarrage du programme

  while (boutonDemarrage() == false) {
    Serial.println("en attente");
  }

  while (digitalRead(capteurInfrarouge) == HIGH) {
    Serial.println("en attente 2");
  }

  while (1) {
    tpsDebutBoucle = millis();
    digitalWrite(ledInterne, HIGH);
    delay(100);
    digitalWrite(ledInterne, LOW);
    delay(100);
    tpsFinBoucle = millis();
    Serial.println(tpsExecProg(tpsDebutBoucle, tpsFinBoucle));
  }
}
