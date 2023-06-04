//KSCDC 2023 Snoo.py
//Writen by Kairos.hk
//2023.06.04

#include <SPI.h>
#include <SD.h>
#include "DHT.h"
#include "RF24.h"
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

const int chipSelect = 4;
DHT dht(8, DHT22);
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
  Serial.println("BMP180 Initialized!")

  radio.begin();
  radio.setPALevel(RF24_PA_HIGH); 
  radio.openWritingPipe(address);
  radio.openReadingPipe(0, address);
  radio.stopListening();
  pinMode(gasSensor, INPUT);
  uart_gps.begin(GPSBAUD);
  dht.begin();
}

void getgps(TinyGPS &gps)
{
  float latitude, longitude, speed;
  gps.f_get_position(&latitude, &longitude);
  String gpsdata = string(latitude) + "," + string(longitute) + "\n";
  gps.f_speed_kmph(&speed);
  String speeddata = string(speed) + "\n";
  
  unsigned long chars;
  unsigned short sentences, failed_checksum;
  gps.stats(&chars, &sentences, &failed_checksum);
}

void loop() {
  File dataFile = SD.open("sensor.txt", FILE_WRITE);

  if (dataFile) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    String dhtdata = "Temperature:" String(t) + " Humidity:" + String(h) + "\n";
    
    if(uart_gps.available()){
     int c = uart_gps.read();
     if(gps.encode(c))
         getgps(gps); 
    }

    sensors_event_t event;
    bmp.getEvent(&event);
    if (event.pressure){
      float hpa = event.pressure;
      String hpadata = float(hpa) + " hpa" + "\n";
 
      float seaLevelPressure = 1008.1;
      float alt = bmp.pressureToAltitude(seaLevelPressure,
                                        event.pressure)); 
      String altdata = float(alt) + " m" + "\n";
      }
      else{
        Serial.println("BMP180 error");
      }

    float gasdt = analogRead(gasSensor);
    String gasdata = "H2: " + float(gasdt) + "\n";

    int nrfdata[6];
    nrfdata[0] = dhtdata;
    nrfdata[1] = gpsdata;
    nrfdata[2] = speeddata;
    nrfdata[3] = hpadata;
    nrfdata[4] = altdata;
    nrfdata[5] = gasdata;

    sendmode();
    radio.write(num,sizeof(num));
  
    dataFile.println(dhtdata);
    dataFile.println(gpsdata);
    dataFile.println(speeddata);
    dataFile.println(hpadata);
    dataFile.println(altdata);
    dataFile.printlb(gasdata);

    dataFile.close();
    Serial.println(data);
  }
  else {
    Serial.println("File Write Error!");
  }

  delay(1000);
}
