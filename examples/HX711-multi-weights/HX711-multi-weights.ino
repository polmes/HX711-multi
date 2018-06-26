#include "HX711-multi.h"

#define CLK A0 // clock pin to the load cell

#define N 3 // # of channels
#define DOUT1 A1 // data pin to the 1st load cell
#define DOUT2 A2 // data pin to the 2nd load cell
#define DOUT3 A3 // data pin to the 3rd load cell

#define TARE_TIMEOUT_SECONDS 4

// Init
byte DOUTS[N] = {DOUT1, DOUT2, DOUT3};
long int results[N];
double weights[N];
float measurements[N] = {1.0, 1.0, 1.0}; // measured SCALES parameters

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

void sendRawData() {
	scales.read(results);
	for (int i = 0; i < scales.get_count(); ++i) {
		Serial.print(-results[i]);  
		Serial.print((i != scales.get_count() - 1) ? "\t" : "\n");
	}
	delay(10);
}

void sendWeightData() {
	scales.get_units(weights);
	// scales.get_units(weights, 5) // to average 5 measurements
	for (int i = 0; i < scales.get_count(); ++i) {
		Serial.print(-weights[i]);
		Serial.print((i != scales.get_count() - 1) ? "\t" : "\n");
	}
	delay(10);
}

void loop() {
	Serial.println("RAW DATA BELOW");
	sendRawData(); // this is for sending raw data, for where everything else is done in processing
	Serial.println();

	Serial.println("WEIGHT DATA BELOW");
	sendWeightData();
	Serial.println();

	// On serial data (any data), re-tare
	if (Serial.available() > 0) {
		while (Serial.available()) {
			Serial.read();
		}
		tare();
	}
}
