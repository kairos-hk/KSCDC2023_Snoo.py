#include <SPI.h>
#include "RF24.h"

RF24 radio(9, 10);

uint8_t address[6] = "34517";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW); 
  radio.openWritingPipe(address);
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void loop() {
  if (radio.available()){
    String nrfdata[6];
    radio.read(nrfdata,sizeof(nrfdata));
    Serial.println(nrfdata)
    receivemode();
  }
}

void receivemode(){
  radio.startListening();
}
