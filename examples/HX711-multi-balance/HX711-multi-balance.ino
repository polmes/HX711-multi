#include "HX711-multi.h"
 
#define CLK A0 // clock pin to the load cell
 
#define N 3 // # of channels
#define DOUT1 A1 // data pin to the 1st load cell lift 1
#define DOUT2 A2 // data pin to the 2nd load cell lift 2
#define DOUT3 A3 // data pin to the 3rd load cell drag
 
#define TARE_TIMEOUT_SECONDS 4
 

// INSERT THE VALUES CORRESPONDING TO YOUR MODEL
 
// Inputs 
double x01 = 0.02; // distance between the leading edge and the first load cell [m]
double v = 10; // speed of the wind [m/s]
double rho = 1.225; // density [kg/m^3]
double S =  0.04; // wing surface [m^2]
double C = 0.2; // chord length [m]
 


//CALLIBRATION OF THE BALANCE

// To re-calibrate the balance (if a load cell is replaced or one of the existing changes its behavior) write a 1.0 in the corresponding cell of measurements[] 
// vector and tare the balance (this can be done sending any character to the Arduino through Serial Monitor).
// Then read the value of 'WEIGHT DATA BELOW' corresponding to the load cell at the program when a known weight is placed at the load cell. 
// Substitute the 1.0 by  (value obtained)/(known weight [g]).  

float measurements[N] = {(-10000/21.2), -8600/21.2, 40750/21.2}; // measured SCALES parameters





// MAIN CODE, DO NOT SAVE THE CHANGES MADE HERE

// Init
byte DOUTS[N] = {DOUT1, DOUT2, DOUT3};
long int results[N];
double weights[N];
double go = 9.80662;
double lift;
double drag;
double Mle;
double F1;
double F2;
double x02 = 0.06;
double x12 = x01 + x02;
double xcp;
double Cl;
double Cd;
double Cm;
 
HX711MULTI scales(N, DOUTS, CLK);
 
void setup() {
    Serial.begin(115200);
    Serial.flush();
    
    tare();
    scales.set_scales(measurements);
}
 
void tare() {
    bool tareSuccessful = false;
 
    unsigned long tareStartTime = millis();
    while (!tareSuccessful && millis() < (tareStartTime + TARE_TIMEOUT_SECONDS * 1000)) {
        tareSuccessful = scales.tare(20, 10000); //reject 'tare' if still ringing
    }
}
 
// void sendRawData() {
//    scales.read(results);
//    for (int i = 0; i < scales.get_count(); ++i) {
//        Serial.print(-results[i]);  
//        Serial.print((i != scales.get_count() - 1) ? "\t" : "\n");
//    }
//    delay(1000);
//}
 
void sendWeightData() {
    scales.get_units(weights);
    for (int i = 0; i < scales.get_count(); ++i) {
        Serial.print(-weights[i]);
        Serial.print((i != scales.get_count() - 1) ? "\t" : "\n");
    }
    delay(1000);
}
 
void loop() {
//    Serial.println("RAW DATA BELOW");
//    sendRawData(); // this is for sending raw data, for where everything else is done in processing
//    Serial.println();
 
    Serial.println("WEIGHT DATA BELOW");
    sendWeightData();
    Serial.println();
 
 
    //calculate forces in N
    
F1 = weights[0]*go/1000; //[N]
F2 = weights[1]*go/1000; //[N]
lift = F1 + F2;
drag = (weights[2])*go/1000; //[N]
Mle = x01 * F1 + x02 *F2; // [Nm]
xcp = (x02 * F2 + x01 * F2) / (F1 + F2)*1000; 

   //aerodynamic coefficients 
   
Cl = (lift*2)/(rho*v*v*S);
Cd = (drag*2)/(rho*v*v*S);
Cm = (Mle*2)/(rho*v*v*S*C);
 
Serial.print("lift = ");
Serial.print(lift);
Serial.println(" [N]");
Serial.print("drag = ");
Serial.print(drag);
Serial.println(" [N]");  // REPRESENT IT IN MILINEWTONS FOR MORE PRECISSION
Serial.print("momentum at the leading edge = ");
Serial.print(Mle);
Serial.println(" [Nm]");
Serial.print("position of the centre of pressure: ");
Serial.print(xcp);
Serial.println(" [mm]");
Serial.println();
Serial.println("Aerodynamic coefficients");
Serial.print("Cl = ");
Serial.println(Cl);
Serial.print("Cd = ");
Serial.println(Cd);
Serial.print("Cm = ");
Serial.println(Cm);
Serial.println();
 
 
  // On serial data (any data), re-tare
  if (Serial.available() > 0) {
    while (Serial.available()) {
      Serial.read();
    }
    tare();
  }
 
}


