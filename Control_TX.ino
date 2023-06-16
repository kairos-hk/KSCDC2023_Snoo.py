//KSCDC 2023 Snoo.py
//Writen by Kairos.hk
//2023.06.16

#include <SPI.h>
#include <SD.h>
#include "RF24.h"
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

const int chipSelect = 4;
RF24 radio(9, 10);
uint8_t address[6] = "34517";
#define RXPIN 6
#define TXPIN 5
#define GPSBAUD 9600
int gasSensor = A1;

TinyGPS gps;
SoftwareSerial uart_gps(RXPIN, TXPIN);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void getgps(TinyGPS &gps);

void sendmode(){
  radio.stopListening();
}

void setup() {
  Serial.begin(9600);
  Serial.print("Checking SD card!");

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card Error!");
    while (1);
  }
  Serial.println("card initialized!");

  if(!bmp.begin())
  {
    Serial.print("BMP180 Error!");
    while(1);
  }
  Serial.println("BMP180 Initialized!");

  radio.begin();
  radio.setPALevel(RF24_PA_LOW); 
  radio.openWritingPipe(address);
  radio.openReadingPipe(0, address);
  radio.stopListening();
  pinMode(gasSensor, INPUT);
  uart_gps.begin(GPSBAUD);
}





void loop() {
  File dataFile = SD.open("sensor.txt", FILE_WRITE);

  if (dataFile) {
    float latitude, longitude;
    float speedd;
    gps.f_get_position(&latitude, &longitude);
    String gpsdata = String(latitude) + "," + String(longitude) + "\n";
    

    sensors_event_t event;
    bmp.getEvent(&event);
    
    float hpa = event.pressure;
    String hpadata = String(hpa) + " hpa" + "\n";
 
    float seaLevelPressure = 1008.1;
    float alt = bmp.pressureToAltitude(seaLevelPressure, event.pressure); 
    String altdata = String(alt) + " m" + "\n";

    float gasdt = analogRead(gasSensor);
    String gasdata = "H2: " + String(gasdt) + "\n";

    String nrfdata[4];
    nrfdata[0] = gpsdata;
    nrfdata[1] = hpadata;
    nrfdata[2] = altdata;
    nrfdata[3] = gasdata;

    sendmode();
    radio.write(nrfdata,sizeof(nrfdata));

    dataFile.println(gpsdata);
    dataFile.println(hpadata);
    dataFile.println(altdata);
    dataFile.println(gasdata);

    dataFile.close();
  }
  else {
    Serial.println("File Write Error!");
  }

  delay(1000);
}
