/* Programme du robot M.A.R.K
 *  
 *  GE-A promo 2027
 * 
 *  Baptiste POIDVIN
 *  Pierre BOURIAUD
 *  Alexandre GUILBAUD
 *  Yann DONNARD
 *  
 *  Version 0.2 du 23/10/2024
 */

/* Initialisation des variables */

int joystickX = A2; // Axe X du joystick sur le pin A2
int joystickY = A3; // Axe Y du joystick sur le pin A3
int ledInterne = 13; // LED interne sur le pin TOR 12
int capteurInfrarouge = 6; // Capteur Infrarouge sur le pin TOR 6

bool boutonDemarrage(void){         // Variable du bouton de démarrage converti avec le joystick
  if(analogRead(joystickX) == 1023){ // Mise à 1 lorsque l'axe X est à 1203 cf. fiche de tests
    return(true);
  }
  else {                            // Sinon bouton à 0
    return(false);
    }
}

void setup(void) {
  // Initialisation des pins (entrées / sorties)

  pinMode(ledInterne, OUTPUT);
  pinMode(joystickX, INPUT);
  pinMode(joystickY, INPUT);
  Serial.begin(9600);

  
}

void loop(void) {
  
while(boutonDemarrage()==false){
  Serial.println("en attente");
}

while(digitalRead(capteurInfrarouge) == HIGH){
  Serial.println("en attente 2");
}
  
while(1) {
  digitalWrite(ledInterne, HIGH);
  delay(100);
  digitalWrite(ledInterne, LOW);
  delay(100);
  }
}
