#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#define SS_PIN 10
#define RST_PIN 9
#define LCH_PIN 8
#define SPI_FREQ 1000000

#define DAC0 0b10000000
#define DAC1 0b10100000

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

uint8_t writeRegister(uint8_t address, uint8_t data0, uint8_t data1){
  // start SPI
  SPI.beginTransaction(SPISettings(SPI_FREQ, MSBFIRST, SPI_MODE1));

  // toggle SS pin
  digitalWrite(SS_PIN, HIGH);
  delay(100);
  digitalWrite(SS_PIN, LOW);

  // write data
  SPI.transfer(address);   
  SPI.transfer(data0);
  SPI.transfer(data1);
  uint8_t temp[] = {address, data0, data1};
  SPI.transfer(CalculateCRC(temp));

  digitalWrite(SS_PIN, HIGH); // disable SS
  SPI.endTransaction(); // end SPI com

  return 0;
}

void setup() {
  // init pins
  digitalWrite(RST_PIN, HIGH);
  digitalWrite(LCH_PIN, HIGH);
  digitalWrite(SS_PIN, HIGH);

  // set up serial
  Serial.begin(9600);

  // set up SPI
  SPI.begin();
  
  /* 
    set up DAC
  */ 

  // reset DACs
  digitalWrite(RST_PIN, HIGH);
  delay(50);
  digitalWrite(RST_PIN, LOW);
  delay(50);

  // calibration memory refresh
  writeRegister(DAC0 | 0x08, 0xFC, 0xBA); // write 0xFCBA to key register
  delay(50);

  // clear reset
  writeRegister(DAC0 | 0x15, 0x20, 0x00); 
  delay(50);

  // enable internal buffers and set range (0xA: 4mA to 20mA, 0x1 = 0V to 10V)
  writeRegister(DAC0 | 0x06, 0x00, 0x1A); 
  delay(50);

  //write zero scale to dac
  writeRegister(DAC0 | 0x01, 0x00, 0x00); 
  delay(50);

  // latch outputs
  digitalWrite(LCH_PIN, LOW);
  delay(50);
  digitalWrite(LCH_PIN, HIGH);

  // enable DAC output
  writeRegister(DAC0 | 0x06, 0x00, 0x30); 
  delay(50);
}

void loop() {
  // continuously change the output
  for(int i=0; i<16; i++){
    writeRegister(DAC0 | 0x01, i*0x10, 0x00); 
    digitalWrite(LCH_PIN, LOW);
    delay(50);
    digitalWrite(LCH_PIN, HIGH);
  }
}