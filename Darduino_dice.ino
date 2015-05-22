
/* Charliplexing 7 LEDs
dice description : 
  1      5
  2  4   6
  3      7
--------------------------------------------------------------------------------
Row 1 (R1): Arduino Pin 10 or ATTINY Pin 1
Row 2 (R2): Arduino Pin 11 or ATTINY Pin 2
Row 3 (R3): Arduino Pin 12 or ATTINY Pin 3
Row 4 (R4): Arduino Pin 13 or ATTINY Pin 4
See pin_switch var Arduino vs Attiny
--------------------------------------------------------------------------------
variable    pinMode      state
 L           OUTPUT       LOW 
 H           OUTPUT       HIGH
 Z           INPUT        LOW
---------------------------------------------------------------------------------
*/

#define ATTINY_MODE 1     // Comment for ARDUINO
//#define TEST_MODE 1      // Comment for production version

//#define ATTINYfd_1  1      //Attiny à 1 mhz
#define ATTINYfd_8    1      //Attiny à 8 mhz
//#define ATTINYfd_20 1      //Attiny à 20 mhz

// Library 
#include <EEPROM.h>       // save data for random
#include <Entropy.h>      // GNU random

const int coef = 5;

#if !defined (ATTINY_MODE)  //ARDUINO
  const int temps_affiche =   500; //display refresh rate = 500 for arduino 150 for ATTINY
  const int temps_une_led =   30;  // display repet light per led = 30 for arduino and 5 for ATTINY
  const int pin_switch =      10;      // match pin between ATTINY and arduino 
#endif

#if defined(ATTINYfd_1)
  const int temps_affiche =   150; //display refresh rate = 500 for arduino 150 for ATTINY
  const int temps_une_led =   24;    // display repet light per led = 30 for arduino and 5 for ATTINY
  const int pin_switch =      1;      // match pin between ATTINY and arduino
#endif

#if defined(ATTINYfd_8)
  const int temps_affiche =   300; //display refresh rate = 500 for arduino 150 for ATTINY
  const int temps_une_led =   100;    // display repet light per led = 30 for arduino and 5 for ATTINY
  const int pin_switch =      1;      // match pin between ATTINY and arduino
#endif

// dice number To led matrix
byte dede[8][7] ={
 //3 4 5 6 7 8 9
  {0,0,0,0,0,0,0}, // 0 nothing
  {0,0,0,1,0,0,0}, // 1
  {0,0,1,0,1,0,0}, // 2
  {1,0,0,1,0,0,1}, // 3
  {1,0,1,0,1,0,1}, // 4
  {1,0,1,1,1,0,1}, // 5
  {1,1,1,0,1,1,1}, // 6
  {1,1,1,1,1,1,1}, // 7 all
}; 

// 1= output high
// 2= output low
// 3= input low
int tabled[7][4] ={
//row 1 2 3 4
  {1,2,3,3}, //led 1
  {2,1,3,3}, //led 2
  {1,3,2,3}, //led 3
  {3,1,2,3}, //led 4
  {3,3,1,2}, //led 5
  {3,3,2,1}, //led 6
  {3,2,3,1}, //led 7
};  

void setup() 
{
    uint8_t random_byte;
    unsigned int id = EEPROM.read(0);
  
    Entropy.Initialize();
    if (Entropy.available()) 
    {
      randomSeed(id);
      id = Entropy.random();
    }
    EEPROM.write(0, id);
  
#if !defined (ATTINY_MODE)    
    Serial.begin (115200);
    Serial.println ("setup");
    Serial.println ("id : ");
    Serial.print (id+1);
    Serial.println("");
#endif 
#if !defined (TEST_MODE)
  Lancedes();
#endif
}

void loop() 
{
#if defined (TEST_MODE)
    afficheled(0,temps_affiche*coef);
    afficheled(1,temps_affiche*coef);
    afficheled(2,temps_affiche*coef);
    afficheled(3,temps_affiche*coef);
    afficheled(4,temps_affiche*coef);
    afficheled(5,temps_affiche*coef);
    afficheled(6,temps_affiche*coef);
    affichede(1, temps_affiche);
    affichede(2, temps_affiche);
    affichede(3, temps_affiche);
    affichede(4, temps_affiche);
    affichede(5, temps_affiche);
    affichede(6, temps_affiche);
#endif
}

// ***** affiche led *****
void afficheled(int numled,int temps) {
int i,j;
//boucle de repetition nombre de temps
//par defaut 1
if (numled >6 || numled <0) affiche_erreur (1);

for(i=0;i<4;i++) // changement des pins arduino  
{
  switch (tabled[numled][i]) {
  case 1 :
    pinMode(pin_switch + i, OUTPUT);
    break;
  case 2 :
    pinMode(pin_switch + i, OUTPUT);
    break;
  case 3 : 
    pinMode(pin_switch + i, INPUT);
    break;
  default : 
    break;
  }
}

if (temps ==0) temps=1;
for (j=0;j<temps;j++)
{
  for(i=0;i<4;i++) // changement des pins arduino  
  {
    switch (tabled[numled][i]) {
    case 1 :
      digitalWrite(pin_switch + i, HIGH);
      break;
    case 2 :
      digitalWrite(pin_switch + i, LOW);
      break;
    case 3 : 
      digitalWrite(pin_switch + i, LOW);
      break;
    default : 
      break;
    }
  }
}
}

// ***** set up leds corresponding the number *****
void affichede(int n ,int temps)
{    int i,j;   
     int tp_led , tp_boucle;
     
     if (n >6 || n <1) affiche_erreur (2);
     
     tp_led = temps_une_led / n;
     tp_boucle= temps;

  //boucle de repetition nombre de temps
  //par defaut 1
  if (tp_boucle ==0) tp_boucle=1;
  for (j=0;j<tp_boucle;j++)
  {
    for(i=0;i<7;i++) if (dede[n][i]==1) afficheled(i,tp_led);
  }
}

void affiche_erreur(int erreur)
{
  int i,j;
  
    switch (erreur) {
      case 1 : // erreur affiche led
        for (i=0;i<10;i++) for (j=1;j<8;j++) afficheled(j,temps_affiche);
        break;
      case 2 : // erreur affichede
        for (i=0;i<10;i++) for (j=1;j<4;j++) afficheled(j,temps_affiche);
        break;
     default : 
        break;
    }
}
  

void Lancedes() 
{
  
 int n,i; 
 int loto1;                // résultat random du nombre de tour de dès avant résultat
 int maxi =15;             // maxi du random du nombre de tour de dès avant résultat
 int mini = 5;             // mini du random du nombre de tour de dès avant résultat
 int delay1;               // temps d'affichage entre deux tours avant final
 int resultat = 10;        // résultat intermediaire
 int ancienresultat = 10;  // empeche deux fois le meme nombre
 

  loto1 = Entropy.random(mini,maxi);

 for (n=0; n < loto1;n++)
  {
     while (resultat == ancienresultat)  resultat=Entropy.random(1,7);      
     ancienresultat=resultat;
     delay1=(temps_affiche)/(loto1-n);
     affichede(resultat,delay1);
   }

#if !defined (ATTINY_MODE)
    Serial.print ("resultat final :");
    Serial.print(resultat);
    Serial.print(" nb resul inter : ");
    Serial.println(loto1);
#endif
 affichede(resultat,temps_affiche*100);
 
}


