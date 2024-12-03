#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define rxPin 4
#define txPin 2
HardwareSerial sim800(1);
String response;
int lastStringLength = response.length();

#include <TinyGPS++.h>
#define RXD2 16
#define TXD2 17
HardwareSerial neogps(2);
TinyGPSPlus gps;
String Linkk;
double latitude, longitude;

//setup MP3Player
static const uint8_t PIN_MP3_TX = 12;
static const uint8_t PIN_MP3_RX = 13; 
SoftwareSerial MP3Player(PIN_MP3_RX, PIN_MP3_TX);
DFRobotDFPlayerMini player;

//setup HC-SR04
#define pinTrig1 14
#define pinEcho1 27
#define pinTrig2 26
#define pinEcho2 25
#define pinTrig3 32
#define pinEcho3 35
#define pinTrig4 33
#define pinEcho4 34
long duration1, duration2, duration3, duration4;
float distance1, distance2, distance3, distance4;
float sound_speed = 0.034;
String bawah, depan, kanan, kiri;
int bawahh, depann, kanann, kirii;

void setup() {
  Serial.begin(115200);
  Serial.println("esp32 serial initialize");

  MP3Player.begin(9600);
  Serial.println("MP3Player serial initialize");

  sim800.begin(9600, SERIAL_8N1, rxPin, txPin);
  Serial.println("sim800 serial initialize");
  sim800.println("AT+CMGF=1");
  Serial.println("sim800 started at 9600");
  delay(1000);
  Serial.println("Setup Complete! sim800 is Ready!");
  sim800.println("AT+CNMI=2,2,0,0,0");

  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("neogps serial initialize");

  pinMode(pinTrig1, OUTPUT);
  pinMode(pinTrig2, OUTPUT);
  pinMode(pinTrig3, OUTPUT);
  pinMode(pinTrig4, OUTPUT);

  pinMode(pinEcho1, INPUT);
  pinMode(pinEcho2, INPUT);
  pinMode(pinEcho3, INPUT);
  pinMode(pinEcho4, INPUT);
  
  if (player.begin(MP3Player)) {
   Serial.println("OK");

    // Set volume to maximum (0 to 30).
    player.volume(20);
    // Play the first MP3 file on the SD card
    player.play(1);
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  ultrasonic1(); //bawah
  ultrasonic2(); //depan
  ultrasonic3(); //kanan
  ultrasonic4(); //kiri

  playSuara();

  if(sim800.available()>0){
    response = sim800.readStringUntil('\n');
  }

  if(lastStringLength != response.length()){
    GPS();
    if(response.indexOf("Lokasi")!=-1){      
      sim800.println("AT+CMGF=1");   
      delay(1000); 
      sim800.println("AT+CMGS=\"08---------\"\r");
      delay(1000);
      sim800.println(Linkk);
      delay(100);
      sim800.println((char)26);
      delay(1000);
    }
  }

  Serial.println("----------------------");
  Serial.println("jarak1: " + String(distance1) + " | bool1: " + String(bawahh) + " | bawah: " + bawah);
  Serial.println("jarak2: " + String(distance2) + " | bool2: " + String(depann) + " | depan: " + depan);
  Serial.println("jarak3: " + String(distance3) + " | bool3: " + String(kanann) + " | kanan: " + kanan);
  Serial.println("jarak4: " + String(distance4) + " | bool4: " + String(kirii) + " | kiri: " + kiri);

  delay(1000);
}

void ultrasonic1(){
  digitalWrite(pinTrig1, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig1, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig1, LOW);

  duration1 = pulseIn(pinEcho1, HIGH);
  distance1 = duration1*sound_speed/2;

  if(distance1 <= 30){
    bawah = "terdeteksi";
    bawahh = 1;
  } else{
    bawah = "tidak terdeteksi";
    bawahh = 0;
  }
}
void ultrasonic2(){
  digitalWrite(pinTrig2, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig2, LOW);

  duration2 = pulseIn(pinEcho2, HIGH);
  distance2 = duration2*sound_speed/2;

  if(distance2 <= 30){
    depan = "terdeteksi";
    depann = 1;
  } else{
    depan = "tidak terdeteksi";
    depann = 0;
  }
}
void ultrasonic3(){
  digitalWrite(pinTrig3, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig3, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig3, LOW);

  duration3 = pulseIn(pinEcho3, HIGH);
  distance3 = duration3*sound_speed/2;

  if(distance3 <= 30){
    kanan = "terdeteksi";
    kanann = 1;
  } else{
    kanan = "tidak terdeteksi";
    kanann = 0;
  }
  
}
void ultrasonic4(){
  digitalWrite(pinTrig4, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig4, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig4, LOW);

  duration4 = pulseIn(pinEcho4, HIGH);
  distance4 = duration4*sound_speed/2;

  if(distance4 <= 30){
    kiri = "terdeteksi";
    kirii = 1;
  } else{
    kiri = "tidak terdeteksi";
    kirii = 0;
  }
}

void playSuara(){
  if(bawahh == 1){
    player.play(1);
    delay(4000);
  }

  if(depann == 1){
    player.play(2);
    delay(4000);
  }

  if(kanann == 1){
    player.play(3);
    delay(4000);
  }

  if(kirii == 1){
    player.play(4);
    delay(4000);
  }
}

void GPS(){
  if(neogps.available()){
    gps.encode(neogps.read());
  }
  if(gps.location.isUpdated()){
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    Linkk = "www.google.com/maps/place/" + String(latitude, 6) + "," + String(longitude, 6) ;
    Serial.println(Linkk);
  }
}

