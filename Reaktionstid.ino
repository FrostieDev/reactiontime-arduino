#include <stdio.h>
#include <string.h>

// Struct med informationer.
struct navne
{
  String Navn;
  int Alder;
  int deltagerNummer;
  int rVaerdier[9]; // Array med reaktions måleværdier. 10 værdier
  float middelVaerdi; // Gennemsnit
  char gnmsntCase; // Case
  bool vinder = 0; // Vinder - FALSE/TRUE
};
struct navne pr[5];

int prAntal;
float vSamlet;

int ledState = LOW;
const int ledPin = 9;


void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(8, INPUT_PULLUP);

  personAntal();
  deltagerDeclare();
  startSpil();
  vinder();
  samletVaerdier();
  hurtigeEndGnmst();

}

void loop() {
  printDeltager();
}

void reaktion() {
  // Vores variabler
  unsigned long tStart, tStop, tReak;
  float rSamlet;
  for (int i = 0; i < prAntal; i++ ) { // Kører efter hvor mange personer der er, altså prAntal
    // Starter program fra 0, og tæller op ad.
    pr[i].middelVaerdi = 0; // øhmm
    digitalWrite(9, HIGH);
    delay(200);
    Serial.print(pr[i].Navn);
    Serial.println(" start tur ved at trykke på knappen");
    while (digitalRead(8) == HIGH);
    digitalWrite(9, LOW);
    rSamlet = 0; // Resetter gennemsnittet per person. Hvis ikke ville den bare lægge på den forrige persons tal.

    for (int k = 0; k < 10; k++) {

      delay(random(2000, 4000)); // Tilfældig delay, Husk ændre til højere værdier.
      digitalWrite(9, HIGH); // Sætter LED High
      tStart = millis(); //Starter nedtælling
      // Når pin Knappen bliver trykket.
      while (digitalRead(8) == HIGH);
      {}
      digitalWrite(9, LOW); // Sætter LED off
      tStop = millis();
      tReak = tStop - tStart; // Måler vi forskellen mellem millis der er startet før delay, med millis der er startet efter delay, som blev aktiveret af knappen.
      pr[i].rVaerdier[k] = tReak; // Sætter værdien ind i den pågældende persons array af reaktioner
      Serial.println(pr[i].rVaerdier[k]);
      // Hvis under 100, print SNYD! i monitor.
      if (tReak < 100) {
        Serial.println("SNYD!");
        k--; // Starter turen forfra.
      }
    }

    for (int o = 0; o < 10; o++) { // Array udprint
      // Print reaktionstiden
      Serial.print("Reaktions Vaerdi[");
      Serial.print(o + 1);
      Serial.print("]:");
      Serial.println(pr[i].rVaerdier[o]);
      rSamlet += pr[i].rVaerdier[o];

    }

    vSamlet += rSamlet;
    float gennemsnit = rSamlet / 10;
    pr[i].middelVaerdi = gennemsnit;
    Serial.print("Reaktiontids gennemsnit : ");
    Serial.println(pr[i].middelVaerdi);
    Serial.println(" ");

    if (gennemsnit < 215) {
      pr[i].gnmsntCase = 'A';
    }
    else if (gennemsnit < 315) {
      pr[i].gnmsntCase = 'B';
    }
    else {
      pr[i].gnmsntCase = 'C';
    }
    pr[i].deltagerNummer = i + 1;
  }
}




void personAntal() { // Antal af personer
  for (int i = 0; i < 1; i++) {
    Serial.println("Hvor mange personer skal spille? (Maks. 5 personer)");
    Serial.print("Indtast antal : ");
    pinMode(9, OUTPUT);
    while (Serial.available() == 0)
    {
      digitalWrite(9, HIGH); //Blink
      delay(500);
      digitalWrite(9, LOW);
      delay(500);
    } //Læser ikke 0, kører uendeligt mens der ikke er en værdi skrevet.
    prAntal = Serial.parseInt(); //Sørger for at læse en Integer, og sætte den i variablen prAntal.
    Serial.println(prAntal); // Printer Interger/variabel ud
    emptyReceiveBuf();
    // ERROR ved for mange spillere.
    if (prAntal > 5 || prAntal == 0) {
      Serial.println("FEJL! - REVURDER");
      i--;
    }
  }
  emptyReceiveBuf();
}

void deltagerDeclare() {
  //Person 1 //Navn

  for (int i = 0; i < prAntal; i++) {
    Serial.print("Deltager ");
    Serial.print(i + 1);
    Serial.println(", indtast dine informationer, tak!");
    Serial.print("Indtast navn : ");
    while (Serial.available() == 0) {} //Læser ikke 0
    pr[i].Navn = Serial.readString(); //Sørger for at læse en string
    Serial.print(pr[i].Navn); // Printer string ud
    //Alder
    Serial.print("Indtast alder : ");
    while (Serial.available() == 0) {} //Tjekker om der bliver skrevet
    pr[i].Alder = Serial.parseInt(); //Sørger for at læse en integer
    Serial.println(pr[i].Alder); // Printer integer ud
    if (pr[i].Alder == 0) {
      Serial.println("FEJL! - REVURDER");
      i--;

    }
    emptyReceiveBuf();
  }
}
void emptyReceiveBuf()
{
  int x;
  delay(500);  // vent lige halvt sekund på at alt er kommet ove
  while (0 < Serial.available() ) {
    x = Serial.read();
  }
}

void startSpil() {

  reaktion();
}

void vinder() {
  float dmin = pr[0].middelVaerdi; //Sætter den første deltager som dmin.
  float dmax = pr[0].middelVaerdi;
  float forskel;
  for (int i = 0; i < prAntal; i++) { // Kører igennem alle deltagere
    if (pr[i].middelVaerdi < dmin) { //Finder ud af om den næste værdi i rækken er mindre.
      dmin = pr[i].middelVaerdi; // Sætter den mindste værdi som dmin
    }
    if (pr[i].middelVaerdi > dmax) {
      dmax = pr[i].middelVaerdi;
    }
  }
  Serial.println("Deltageren med laveste gennemsnit, og dermed VINDEREN! var:");
  for (int i = 0; i < prAntal; i++) {
    if (pr[i].middelVaerdi == dmin) {
      Serial.print(pr[i].Navn);
      pr[i].vinder = 1;
    }
  }
  Serial.println("Med et gennemsnit på:");
  Serial.print(dmin);
  Serial.println(" milisekunder");

  Serial.println();

  forskel = dmax - dmin;
  Serial.print("Forskellen mellem vinderen og den absolutte taber(");
  for (int i = 0; i < prAntal; i++) {
    if (pr[i].middelVaerdi == dmax) {
      Serial.print(pr[i].Navn);
    }
  }
  Serial.print(")");
  Serial.print(", var:");
  Serial.println(forskel);
}


void samletVaerdier() {
  float *ptrVSamlet = NULL;
  float gnmstSamlet;
  /*int samletV = 0;
  float samletV2 = 0;
  int *ptrA = &samletV;
  float b = samletV2;
  Serial.println(samletV);
  Serial.println(samletV2);
  for ( int i = 0; i < prAntal; i++) {
    for (int k = 0; k < 10; k++) {
      *ptrA += pr[i].rVaerdier[k];
      b += pr[i].rVaerdier[k];
    }
  }
  Serial.println(*ptrA);
  Serial.println(b); */
  ptrVSamlet = &vSamlet;
  Serial.println("Den samlede reaktionstid, i sekunder, for alle spillere blev:");
  Serial.println(*ptrVSamlet / 1000);
  gnmstSamlet = *ptrVSamlet / (prAntal * 10);
  Serial.println("Det samlede gennemsnit er saa: ");
  Serial.println(gnmstSamlet);
  /* Serial.println("The lValue for ptrVSamlet is: ");
   Serial.println((long) &ptrVSamlet, DEC);
     Serial.println("The lValue for vSamlet is: ");
   Serial.println((long) &vSamlet, DEC); */
}
void hurtigeEndGnmst() { // Case for hurtigere end gennemsnittet som taget fra  http://www.humanbenchmark.com/tests/reactiontime
  Serial.println(" ");
  for (int i = 0; i < prAntal; i++) {
    switch (pr[i].gnmsntCase) {
      case 'A' :
        Serial.print(pr[i].Navn);
        Serial.println("HURTIGERE END DET GLOBALE GENNEMSNIT!");
        break;
      case 'B' :
        Serial.print(pr[i].Navn);
        Serial.println("Langsommere end det globale gennemsnit...");
        break;
      case 'C' :
        Serial.print(pr[i].Navn);
        Serial.println("Sover du?");
    }
  }
}

void printDeltager() { // Print deltager ved at skrive navn.
  Serial.println(" ");
  Serial.println("Skriv navn for at se resultater");
  String printD;
  
  while (Serial.available() == 0) {}
  printD = Serial.readString();
  for (int i = 0; i < prAntal; i++) {
    if (pr[i].Navn == printD) {
      Serial.println(" ");
      Serial.print("Navn: ");
      Serial.print(pr[i].Navn);
      Serial.print("Alder:");
      Serial.println(pr[i].Alder);
      for (int k = 0; k < 10; k++) {
        Serial.print("Reaktions Vaerdi[");
        Serial.print(k + 1);
        Serial.print("]:");
        Serial.println(pr[i].rVaerdier[k]);
      }
      Serial.print("Gennemsnit:");
      Serial.println(pr[i].middelVaerdi);
      if (pr[i].vinder == 1) {
        Serial.println("Vinder!");
      }
      else
      {
        Serial.println("Taber");
      }
      
      emptyReceiveBuf();
      return;
    }
    /*    else
    {

    }*/
  }
        Serial.println("FEJL");
      Serial.print("Folgende navne er valide:");
       for (int o = 0; o < prAntal; o++) {
        Serial.println(pr[o].Navn);
       }
      return;
}


// Gennemsnitlig reaktionstid http://www.humanbenchmark.com/tests/reactiontime 215 milisekunder - Kan ikke helt sammenlignes men alligevel.
