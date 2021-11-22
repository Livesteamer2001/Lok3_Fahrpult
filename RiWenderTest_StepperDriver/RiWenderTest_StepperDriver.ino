#include <Arduino.h>

//#include <BasicStepperDriver.h>
#include "A4988.h"

#define RPM 400
// 1=full step, 2=half step etc.
#define MICROSTEPS 1
// All the wires needed for full functionality
#define MOTOR_STEPS 64   // Mit Getriebe: 2048 /Umdrehung
#define DIR 0
#define STEP 1
#define SLEEP 2
#define STOPPER_PIN 7
A4988 stepper(MOTOR_STEPS, DIR, STEP);




#include <ELECHOUSE_CC1101_SRC_DRV.h>





// this constant won't change:
const int RiWend_V = A6;    // Richtungswender für Vorwärts PD4
const int RiWend_R = A5;   // Richtungswender für Rückwärts PF2
const int Licht_V = A9;
const int Licht_H = A11;
const int Licht_F = 11;   // Fernlicht, digital

// this variables will change
int Li_V = 0;
int Schluss_V = 0;
int Li_H = 0;
int Schluss_H = 0;
int Li_F = 0;
char RiWend = 'N';
int tVal = 0;
int tFst = 0;                 // Variablen für Fahrschalterwerte
char Fahrstufe = 'F';
int L = 0;                    // Läuten
int P = 0;                    // Pfeifen
int stepPosition = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(11,INPUT_PULLUP);   // Fernlicht
  pinMode(2, OUTPUT);         // Enable für Stepper und LED-Fahrschalter
  pinMode(5, OUTPUT);         // Voltmeter, PWM  22V 24V -> 110, , 22V max 245
  pinMode(6, OUTPUT);         // Amperemeter, PWM 10A -> 45, 20A -> 90, 30A 
  pinMode(13, OUTPUT);        // Störmelder
  pinMode(10, OUTPUT);        // SS für CC1101
  pinMode(0, OUTPUT);         // Stepper dir
  pinMode(1, OUTPUT);         // Stepper step
  pinMode(STOPPER_PIN, INPUT_PULLUP);   // Nullpunkt Tacho

  stepper.begin(RPM, MICROSTEPS);
  stepper.startMove(2048);
  stepper.enable();
  
  /*
  ELECHOUSE_cc1101.setSpiPin(15,14,16,10);  //SCL, MISO, MOSI, SS
  if (ELECHOUSE_cc1101.getCC1101()){         // Check the CC1101 Spi connection.
    Serial.println("Connection OK");
    }else{
    Serial.println("Connection Error");
    }

    ELECHOUSE_cc1101.Init();              // must be set to initialize the cc1101!
    ELECHOUSE_cc1101.setCCMode(1);       // set config for internal transmission mode.
    ELECHOUSE_cc1101.setModulation(0);  // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
    ELECHOUSE_cc1101.setMHZ(868);   // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
    ELECHOUSE_cc1101.setSyncMode(2);  // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
    ELECHOUSE_cc1101.setCrc(1);      // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
    
    Serial.println("Rx Mode");
    */
    
}

void loop() {
  
  // put your main code here, to run repeatedly:
  if (digitalRead(STOPPER_PIN) == LOW){
        stepper.stop();
        stepPosition = 0;
    }
  // motor control loop - send pulse and return how long to wait until next pulse
  unsigned wait_time_micros = stepper.nextAction();
   if (wait_time_micros <= 0) {
        Do_things();
    }
   // execute other code if we have enough time
    if (wait_time_micros > 100){
        Do_things();
    }
    Serial.println(wait_time_micros);
}

void Do_things(){
          int tVal5 = analogRead(A5);
          int tVal6 = analogRead(A6);
        
          // Richtungswender bestimmen
          if ((tVal5 < 25) && (tVal6 < 25)) {
            RiWend = 'N'; 
            digitalWrite(2, HIGH);
          }
          else if (tVal5 > 25) {
            RiWend = 'R';
            digitalWrite(2, LOW);
          }
          else if (tVal6 > 25) {
            RiWend = 'V';
            digitalWrite(2, LOW);
          }
        
          // Fahrschalter auslesen, wenn Richtungswender nicht N
          if (RiWend != 'N') {
            read_Fahrschalter();       
          }
          
          read_Licht();
        
          read_LP();
        
          write_Voltmeter();
          write_Amperemeter();
        
          
          
          String stringout = "RiWend: ";
          //stringout = stringout + RiWend + "  F: " + tVal4 + " " +tVal3 + " " + tVal2 + " " + tVal1 + " " + tVal0;
          stringout = stringout + RiWend + "  F: " + Fahrstufe;
          Serial.println(stringout);
          
}

void read_Fahrschalter(){
    tFst = 0;
    tVal = analogRead(A0);
    if (tVal > 25) {
        tFst = 1;
    }  
    tVal = analogRead(A1);
    if (tVal > 25) {
        tFst = tFst + 2;
    }  
    tVal = analogRead(A2);
    if (tVal > 25) {
        tFst = tFst + 4;
    } 
    tVal = analogRead(A3);
    if (tVal > 25) {
        tFst = tFst + 8;
    } 
    tVal = analogRead(A4);
    if (tVal > 25) {
        tFst = tFst + 16;
    }    
    
    // Neutral
    if (tFst == 0) {
      Fahrstufe = 'F';
    }
    // Fahrstufen
    else if (tFst == 1){
      Fahrstufe = 'G';
    }
    else if (tFst == 3){
      Fahrstufe = 'H';
    }
    else if (tFst == 2){
      Fahrstufe = 'I';
    }
    else if (tFst == 6){
      Fahrstufe = 'J';
    }
    else if (tFst == 7){
      Fahrstufe = 'K';
    }
    else if (tFst == 5){
      Fahrstufe = 'L';
    }
    else if (tFst == 4){
      Fahrstufe = 'M';
    }
     else if (tFst == 12){
      Fahrstufe = 'N';
    }
    else if (tFst == 13){
      Fahrstufe = 'O';
    }
    else if (tFst == 15){
      Fahrstufe = 'P';
    }
    else if (tFst == 14){
      Fahrstufe = 'Q';
    }
    
    // Bremsstufen
    else if (tFst == 16){
      Fahrstufe = 'E';
    }
    else if (tFst == 17){
      Fahrstufe = 'D';
    }
    else if (tFst == 19){
      Fahrstufe = 'C';
    }
    else if (tFst == 18){
      Fahrstufe = 'B';
    }
    else if (tFst == 22){
      Fahrstufe = 'A';
    }
    else { 
      Fahrstufe = 'F';
    }
}

void read_Licht(){
  // Lichtschalter auswerten und Werte setzen
  tVal = 0;
  tVal = analogRead(A9);
  if (tVal < 10) {
    Li_V = 0;
    Schluss_V = 0;
  }
  else if (tVal > 400) {
    Li_V = 1;
    Schluss_V = 0;
  }
  else {
    Li_V = 0;
    Schluss_V = 1;
  }
  
  tVal = analogRead(A11);
  if (tVal < 10) {
    Li_H = 0;
    Schluss_H = 0;
  }
  else if (tVal > 400) {
    Li_H = 0;
    Schluss_H = 1;
  }
  else {
    Li_H = 1;
    Schluss_H = 0;
  }
  
  tVal = digitalRead(11);  
  if (tVal = LOW) {
    Li_F = 0;
  }
  else {
    Li_F = 1;
  }
  /*
  String stringout1 = "Li_V: ";
  stringout1 = stringout1 + Li_V + "  Schluss_V: " + Schluss_V + "   Li_H: " + Li_H + "   Schluss_H: " + Schluss_H + "   Li_F: " + Li_F;
  Serial.println(stringout1);
  */
}  

void read_LP(){
  tVal = 0;
  tVal = analogRead(A8);
  if (tVal < 10){
    L = 0;
    P = 0;
  }
  else if (tVal < 310) {
    L = 0;
    P = 1;
  }
  else if (tVal < 530) {
    L = 1;
    P = 0;
  }
  else {
    L = 1;
    P = 1;
  }

 // Serial.println(tVal);
  
  //Serial.println( L + P);
}

void write_Voltmeter() {
  //Voltmeter, PWM  22V -> 75, 24V -> 110, , 26V -> 155 max 245
  analogWrite(5,110);
}

void write_Amperemeter() {
  //Amperemeter, PWM 10A -> 45, 20A -> 95, 30A -> 150, 40A -> 240
  analogWrite(6,95);
}
