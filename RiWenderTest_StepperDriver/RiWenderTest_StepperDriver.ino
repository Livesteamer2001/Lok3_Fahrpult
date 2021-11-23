#include <Arduino.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "A4988.h"

#define RPM 50
// 1=full step, 2=half step etc.
#define MICROSTEPS 1
// All the wires needed for full functionality
#define MOTOR_STEPS 64   // Mit Getriebe: 2048 /Umdrehung
#define DIR 0
#define STEP 1
#define SLEEP 2
#define STOPPER_PIN 7
#define RiWend_V = A6;    // Richtungswender für Vorwärts PD4
#define RiWend_R = A5;   // Richtungswender für Rückwärts PF2
#define Licht_V = A9;
#define Licht_H = A11;
#define Licht_F = 11;   // Fernlicht, digital

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

A4988 stepper(MOTOR_STEPS, DIR, STEP);

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
  
   ELECHOUSE_cc1101.setSpiPin(15, 14, 16, 10);
    if (ELECHOUSE_cc1101.getCC1101()){      // Check the CC1101 Spi connection.
       Serial.println("Connection OK");
       delay(500);
       digitalWrite(13, HIGH);
       delay(200);
       digitalWrite(13, LOW);
       delay(500);
       digitalWrite(13, HIGH);
       delay(500);
       digitalWrite(13, LOW);
    }else{
      Serial.println("Connection Error");
    } 
      ELECHOUSE_cc1101.Init();                // must be set to initialize the cc1101!
      ELECHOUSE_cc1101.setCCMode(1);          // set config for internal transmission mode.
      ELECHOUSE_cc1101.setModulation(0);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
      ELECHOUSE_cc1101.setMHZ(868);        // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
      ELECHOUSE_cc1101.setDeviation(47.60);   // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
      ELECHOUSE_cc1101.setChannel(0);         // Set the Channelnumber from 0 to 255. Default is cahnnel 0.
      ELECHOUSE_cc1101.setChsp(199.95);       // The channel spacing is multiplied by the channel number CHAN and added to the base frequency in kHz. Value from 25.39 to 405.45. Default is 199.95 kHz.
      ELECHOUSE_cc1101.setRxBW(812.50);       // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
      ELECHOUSE_cc1101.setDRate(99.97);       // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
      ELECHOUSE_cc1101.setPA(10);             // Set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!
      ELECHOUSE_cc1101.setSyncMode(2);        // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
      ELECHOUSE_cc1101.setSyncWord(211, 145); // Set sync word. Must be the same for the transmitter and receiver. (Syncword high, Syncword low)
      ELECHOUSE_cc1101.setAdrChk(0);          // Controls address check configuration of received packages. 0 = No address check. 1 = Address check, no broadcast. 2 = Address check and 0 (0x00) broadcast. 3 = Address check and 0 (0x00) and 255 (0xFF) broadcast.
      ELECHOUSE_cc1101.setAddr(0);            // Address used for packet filtration. Optional broadcast addresses are 0 (0x00) and 255 (0xFF).
      ELECHOUSE_cc1101.setWhiteData(0);       // Turn data whitening on / off. 0 = Whitening off. 1 = Whitening on.
      ELECHOUSE_cc1101.setPktFormat(0);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
      ELECHOUSE_cc1101.setLengthConfig(1);    // 0 = Fixed packet length mode. 1 = Variable packet length mode. 2 = Infinite packet length mode. 3 = Reserved
      ELECHOUSE_cc1101.setPacketLength(0);    // Indicates the packet length when fixed packet length mode is enabled. If variable packet length mode is used, this value indicates the maximum packet length allowed.
      ELECHOUSE_cc1101.setCrc(1);             // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
      ELECHOUSE_cc1101.setCRC_AF(0);          // Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size.
      ELECHOUSE_cc1101.setDcFilterOff(0);     // Disable digital DC blocking filter before demodulator. Only for data rates ≤ 250 kBaud The recommended IF frequency changes when the DC blocking is disabled. 1 = Disable (current optimized). 0 = Enable (better sensitivity).
      ELECHOUSE_cc1101.setManchester(0);      // Enables Manchester encoding/decoding. 0 = Disable. 1 = Enable.
      ELECHOUSE_cc1101.setFEC(0);             // Enable Forward Error Correction (FEC) with interleaving for packet payload (Only supported for fixed packet length mode. 0 = Disable. 1 = Enable.
      ELECHOUSE_cc1101.setPRE(0);             // Sets the minimum number of preamble bytes to be transmitted. Values: 0 : 2, 1 : 3, 2 : 4, 3 : 6, 4 : 8, 5 : 12, 6 : 16, 7 : 24
      ELECHOUSE_cc1101.setPQT(0);             // Preamble quality estimator threshold. The preamble quality estimator increases an internal counter by one each time a bit is received that is different from the previous bit, and decreases the counter by 8 each time a bit is received that is the same as the last bit. A threshold of 4∙PQT for this counter is used to gate sync word detection. When PQT=0 a sync word is always accepted.
      ELECHOUSE_cc1101.setAppendStatus(0);    // When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK.
  
   // Home-Position für Tacho finden
  digitalWrite(SLEEP, LOW);
  stepper.begin(RPM, MICROSTEPS);
  stepper.startMove(2048);
  stepper.enable();
  
    
}

void loop() {
  
  // put your main code here, to run repeatedly:
  if (digitalRead(STOPPER_PIN) == LOW){
        stepper.stop();
        stepper.startMove(-10);
        stepPosition = 0;
        digitalWrite(SLEEP, HIGH);
        stepper.setRPM(400);
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
