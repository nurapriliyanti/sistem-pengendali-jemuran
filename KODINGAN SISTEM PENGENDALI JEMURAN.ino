#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <FirebaseESP8266.h>

int ENA = D8;
int IN1 = D4;
int IN2 = D5;
int ENB = D9;
int IN3 = D6;
int IN4 = D7;
int rainsense= A0; // analog sensor pin A0 untuk input raindrop
int countval= 0; // counter value mulai 0 
int x = 0;
int Jemuran = 1 ;
int status = WL_IDLE_STATUS;
char ssid [] = "NURIZ";
char password [] = "cahayailahi";

WiFiServer server(80);
FirebaseData firebaseData;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(rainsense, INPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  analogWrite (ENA, 255);
  analogWrite (ENB, 255);
  Wifi ();
  Firebase.begin("https://c-remote.firebaseio.com/", "bKCQq9D9CuC7o42D1lVutZ9JzEi9JQmRDCTBVBjm");
}

void loop() {
  // put your main code here, to run repeatedly:
  Firebase.getInt(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Otomatis");
  if (firebaseData.intData() == 1){
    Serial.println("Mode Otomatis");
    ModeOtomatis();
  }
  else if (firebaseData.intData() == 2){
    Serial.println("Mode Manual");
    ModeManual();
  }
}

void Wifi () {
  WiFi.begin(ssid, password);
  Serial.println("Koneksi ke jaringan dilakukan . . .");
  Serial.println("SSID: ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay (500);
    Serial.print (". ");
  }
  server.begin();
  IPAddress ip = WiFi.localIP();
  Serial.print ("\nAlamat IP: ");
  Serial.println (ip);
  Serial.print ("Siap Melayani Anda");
}

void ModeOtomatis(){
   int membacaSensorHujan = analogRead(rainsense);
  Serial.println(membacaSensorHujan);
  Firebase.setInt(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Sensor", membacaSensorHujan);
  delay (250);
  if (countval == 0){
    sensorHujan();
  }
  else if (countval == 1){
    Hujan();
  }
  else if (countval == 2){
    Cerah();
  }
}

void sensorHujan(){
  int membacaSensorHujan = analogRead(rainsense);
  Serial.println(membacaSensorHujan);
  Firebase.setInt(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Sensor", membacaSensorHujan);
  delay (250);
  //Kondisi Hujan
  if (membacaSensorHujan <= 600 && x == 0){
    countval = 1;
  }
  //Kondisi Cerah
  else if (membacaSensorHujan >= 601 && x == 0){
    countval = 2;
  }
  //Kondisi Hujan Alat Sudah Bekerja
  else if (membacaSensorHujan >= 601 && x == 1){
    x = 0; //reset alat
    countval = 0;
  }
  //Alat sudah bekerja 1x dan Alat hanya mengambil data Hujan
  else if (membacaSensorHujan <= 600 && x == 1){
    countval = 0;
  }
  //Kondisi Cerah Alat Sudah Bekerja
  else if (membacaSensorHujan <= 600 && x == 2){
    x = 0; //reset alat
    countval = 0;
  }
  //Kondisi Cerah Alat hanya mengambil data Hujan
  else if (membacaSensorHujan >= 601 && x == 2){
    countval = 0;
  }
}
void Hujan(){
  Firebase.setString(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Kondisi", "DI DALAM");
  Serial.println("Hujan");
  Serial.println("Jemuran berada di dalam");
  //Set Motor A backward
  Jemuran = 0;
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(3000);
  Firebase.setInt(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Jemuran", Jemuran);
  delay(1000);
  Jemuran = 1;
  Firebase.setInt(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Jemuran", Jemuran);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  x = 1;
  countval = 0;
}

void Cerah(){
  Firebase.setString(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Kondisi", "DI LUAR");
  Serial.println("Cerah");
  Serial.println("Jemuran berada di luar");
  //Set Motor A forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(3000);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);  
  x = 2;
  countval = 0;
}

void ModeManual(){
  int membacaSensorHujan = analogRead(rainsense);
  Serial.println(membacaSensorHujan);
  Firebase.setInt(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Sensor", membacaSensorHujan);
  delay (250);
  //Mengatur Fungsi Tombol Melalui Angka
  Firebase.getInt(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Manual");
  if (firebaseData.intData() == 1){
    Serial.println("Diluar Cerah, Alat Terbuka");
    CuacaCerah();
  }
  else if (firebaseData.intData() == 2){
    Serial.println("Diluar Hujan, Alat Tertutup");
    CuacaHujan();
  }
  else if (firebaseData.intData() == 0){
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW);

  }
}

void CuacaCerah(){
  Firebase.setString(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Kondisi", "DI LUAR");
  Serial.println("Cerah");
  Serial.println("Jemuran berada di luar");
  //Set Motor A forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(3000);
  Firebase.setInt(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Manual", 0);
}

void CuacaHujan(){
  Firebase.setString(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Kondisi", "DI DALAM");
  Serial.println("Hujan");
  Serial.println("Jemuran berada di dalam");
  //Set Motor A backward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(3000);
  Firebase.setInt(firebaseData, "/Data/-M9E1yzOFtaKOIa4XeHs/Manual", 0);
}
