#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#define SS_PIN 10
#define SPI_FREQ 1000000

// continously checks both 1 voltage channel or current channel
// by changing VOLTAGE to 1, a Vin0 will be selected
// by chanign VOLTAGE to 0, Iin0 will be selected
#define VOLTAGE 1

uint8_t data[3] = {0, 0, 0}; //data buffer

uint8_t CalculateCRC(uint8_t data[]){
  uint8_t i;
	uint8_t crc = 0;
  uint8_t data_size = 3;

	while (data_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			if (((crc & 0x80) != 0) != ((*data & i) != 0)) {
				crc <<= 1;
				crc ^= 0x07; // x^8 + x^2 + x^1 + x^0
			} else {
				crc <<= 1;
			}
		}
		data++;
		data_size--;
	}

	return crc;
}

uint8_t writeRegister(uint8_t data[]){
  // start SPI
  SPI.beginTransaction(SPISettings(SPI_FREQ, MSBFIRST, SPI_MODE0));

  // toggle SS pin
  digitalWrite(SS_PIN, HIGH);
  delay(100);
  digitalWrite(SS_PIN, LOW);

  for(int i = 0; i<3 ; i++){
    data[i] = SPI.transfer(data[i]); // then, write data to register
  }
    
  digitalWrite(SS_PIN, HIGH); // disable SS
  SPI.endTransaction(); // end SPI com

  return 0;
}

void setup() {
  // set up serial
  Serial.begin(9600);

  // set up SPI
  SPI.begin();
  
  /* 
    set up DAC
  */ 
}

void loop() {

}