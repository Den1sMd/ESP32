#include <dummy.h>

/*
  Developpement Projet TSN3 section 2024-2025
  Fichier:      VariationVitesseMoteurL298N
  Description:  Adaptation de code Arduino, permettant de faire varier la vitesse de 2 moteurs à courant continu (accélération, puis décélération)
                avec inversion de sens de rotation, le tout piloté par un module L298N raccordé à un ESP32
  
*/

//*******************************************************************************//
// Association des entrées du L298N, aux sorties utilisées sur notre ESP32 //
//*******************************************************************************//

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

#define borneENA        23      // On associe la borne "ENA" du L298N à la pin D23 de l'Arduino (PWM possible sur cette broche)
#define borneIN1        22       // On associe la borne "IN1" du L298N à la pin D22 de l'Arduino
#define borneIN2        19       // On associe la borne "IN2" du L298N à la pin D19 de l'Arduino
#define borneIN3        2       // On associe la borne "IN3" du L298N à la pin D2 de l'Arduino
#define borneIN4        4       // On associe la borne "IN4" du L298N à la pin D4 de l'Arduino
#define borneENB        5


// On associe la borne "ENB" du L298N à la pin D5 de l'Arduino (PWM possible sur cette broche)

//*************************//
// Constantes du programme //
//*************************//
#define delaiChangementVitesse     20       // Défini un petit délai, exprimé en millisecondes, avant tout autre changement de vitesse
#define vitesseMinimale            60       // Rapport cylique minimal du signal PWM, pour faire tourner le moteur au minimum de sa vitesse (en pratique, on évitera de trop approcher la valeur  0)
#define vitesseMaximale            255 
#define vitessestop101             0     // Rapport cylique maximal du signal PWM, pour faire tourner le moteur au maximum de sa vitesse




const char MARCHE_AVANT   = 'V';            // Défini une constante pour la "marche avant" (peu importe la valeur)
const char MARCHE_ARRIERE = 'R';
const char MARCHE_DROITE = 'D';
const char MARCHE_GAUCHE = 'G';       // Défini une constante pour la "marche arrière" (peu importe la valeur)

//************************//
// Variables du programme //
//************************//
int vitesse;                    // Spécifie la vitesse de rotation du moteur, entre son minimum (0) et son maximum (255) <= signal PWM

//*******//
// SETUP //
//*******//
void setup() {
  
  // Configuration de toutes les pins de l'Arduino en "sortie" (car elles attaquent les entrées du module L298N)

  Serial.begin(115200);
  SerialBT.begin("ESP32 Bluetooth (Credit : Denis)");

  pinMode(borneENA, OUTPUT);
  pinMode(borneIN1, OUTPUT);
  pinMode(borneIN2, OUTPUT);
  pinMode(borneIN3, OUTPUT);
  pinMode(borneIN4, OUTPUT);
  pinMode(borneENB, OUTPUT);

  String credit = "Made by Denis";
  Serial.println(credit);
  
}

//**************************//
// Boucle principale : LOOP //
//**************************//
void loop() {


  if (SerialBT.available()) {
    char commande = SerialBT.read();

    if (commande == 'g' || commande == 'G') {
      configurerSensDeRotationPontA(MARCHE_GAUCHE);
      configurerSensDeRotationPontB(MARCHE_GAUCHE);
      changeVitesseMoteurPontA(200);  // Exemple vitesse
      changeVitesseMoteurPontB(200);
    } 
    else if (commande == 'd' || commande == 'D') {
      configurerSensDeRotationPontA(MARCHE_DROITE);
      configurerSensDeRotationPontB(MARCHE_DROITE);
      changeVitesseMoteurPontA(200);
      changeVitesseMoteurPontB(200);
    }
    else if (commande == 'a' || commande == 'A') {
      configurerSensDeRotationPontA(MARCHE_AVANT);
      configurerSensDeRotationPontB(MARCHE_AVANT);
      changeVitesseMoteurPontA(200);
      changeVitesseMoteurPontB(200);
    }
    else if (commande == 'r' || commande == 'R') {
      configurerSensDeRotationPontA(MARCHE_ARRIERE);
      configurerSensDeRotationPontB(MARCHE_ARRIERE);
      changeVitesseMoteurPontA(200);
      changeVitesseMoteurPontB(200);
    }
    else if (commande == 's' || commande == 'S') {
      stop101(0);
    }
  }
}



//************************************************************************************//
// Fonction : configurerSensDeRotationPontA()                                         //
// But :      Permet de définir le sens de rotation du moteu branché sur le pont A    //
//************************************************************************************//
void configurerSensDeRotationPontA(char sensDeRotation) {

  if(sensDeRotation == MARCHE_AVANT) {
    // Configuration du L298N en "marche avant", pour le moteur connecté au pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN1, HIGH);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneIN2, LOW);                  // L'entrée IN2 doit être au niveau bas    
  }
  
  if(sensDeRotation == MARCHE_ARRIERE) {
    // Configuration du L298N en "marche arrière", pour le moteur câblé sur le pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN1, LOW);                  // L'entrée IN1 doit être au niveau bas
    digitalWrite(borneIN2, HIGH);                 // L'entrée IN2 doit être au niveau haut
  }
  if(sensDeRotation == MARCHE_DROITE) {
    digitalWrite(borneIN1, LOW);
    digitalWrite(borneIN2, HIGH);
  }
  if(sensDeRotation == MARCHE_GAUCHE) {
    digitalWrite(borneIN1, HIGH);
    digitalWrite(borneIN2, LOW);
  }
  
}

//************************************************************************************//
// Fonction : configurerSensDeRotationPontB()                                         //
// But :      Permet de définir le sens de rotation du moteu branché sur le pont B    //
//************************************************************************************//
void configurerSensDeRotationPontB(char sensDeRotation) 
{

  if(sensDeRotation == MARCHE_AVANT) {
    // Configuration du L298N en "marche avant", pour le moteur connecté au pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN3, LOW);                 // L'entrée IN3 doit être au niveau haut
    digitalWrite(borneIN4, HIGH);                  // L'entrée IN4 doit être au niveau bas    
  }
  
  if(sensDeRotation == MARCHE_ARRIERE) {
    // Configuration du L298N en "marche arrière", pour le moteur câblé sur le pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN3, HIGH);                  // L'entrée IN3 doit être au niveau bas
    digitalWrite(borneIN4, LOW);                 // L'entrée IN4 doit être au niveau haut
  }
    if(sensDeRotation == MARCHE_DROITE) {
    digitalWrite(borneIN3, HIGH);
    digitalWrite(borneIN4, LOW);
  }
  if(sensDeRotation == MARCHE_GAUCHE) {
    digitalWrite(borneIN3, LOW);
    digitalWrite(borneIN4, HIGH);
  }

 
}

//************************************************************************************//
// Fonction : changeVitesseMoteurPontA()                                              //
// But :      Change la vitesse de rotation du moteur branché sur le pont A           //
//            (cette valeur doit être comprise entre 0 et 255)                        //
//************************************************************************************//
void changeVitesseMoteurPontA(int nouvelleVitesse) 
{
  
  // Génère un signal PWM permanent, de rapport cyclique égal à "nouvelleVitesse" (valeur comprise entre 0 et 255)
  analogWrite(borneENA, nouvelleVitesse);
}

//************************************************************************************//
// Fonction : changeVitesseMoteurPontB()                                              //
// But :      Change la vitesse de rotation du moteur branché sur le pont A           //
//            (cette valeur doit être comprise entre 0 et 255)                        //
//************************************************************************************//
void changeVitesseMoteurPontB(int nouvelleVitesse) 
{
  
  // Génère un signal PWM permanent, de rapport cyclique égal à "nouvelleVitesse" (valeur comprise entre 0 et 255)
  analogWrite(borneENB, nouvelleVitesse);
}

void stop101(int stopTout) {
  digitalWrite(borneIN1, LOW);                 // L'entrée IN3 doit être au niveau haut
  digitalWrite(borneIN2, LOW);                  // L'entrée IN4 doit être au niveau bas   
  digitalWrite(borneIN3, LOW);                 // L'entrée IN3 doit être au niveau haut
  digitalWrite(borneIN4, LOW);

 

  
  
      



}