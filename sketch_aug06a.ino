//library
#include <WiFiClient.h>
#include <Esp32WifiManager.h>
#include <FB_HTTPClient32.h>
#include <FirebaseESP32.h>
#include <FirebaseJson.h>
#include <WiFi.h>
#include<Servo.h>
//pengaturan firebase
#define FIREBASE_HOST "https://ceban-f91c8-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "2ABAxEsB5Rieofjqghyehc946LQ5tFiSmse9u9uU"
//pengaturan jaringan
#define WIFI_SSID "Hemat kuota"
#define WIFI_PASSWORD "harson2227"
//pengaturan variabel pada firebase
FirebaseData firebaseData;
FirebaseJson air,bak,cuaca;
int jarak1firebase;
int jarak2firebase;
int hujanfirebase;
//pengaturan input
#define trigger1Pin 5
#define echo1Pin 4
#define trigger2Pin 18
#define echo2Pin 19
const int sensor_hujan = 22;
//pengaturan output
Servo arm;
int buzzpin = 21;
int LedMerah = 2;
int LedHijau = 23;
int Pump = 12;
//definisi variabel input
float duration1;
float duration2;
float jarak1;
float jarak2;
float sensor;
String servo;
void setup() {
//inisial input
pinMode(trigger1Pin, OUTPUT);
pinMode(echo1Pin, INPUT);
pinMode(trigger2Pin, OUTPUT);
pinMode(echo2Pin, INPUT);
pinMode (sensor_hujan, INPUT);
//inisial ouput
arm.attach(27);
pinMode(LedMerah, OUTPUT);
pinMode(LedHijau, OUTPUT);
pinMode(buzzpin, OUTPUT);
pinMode(Pump, OUTPUT);
//kecepatan serial
Serial.begin(9600);
//penyambungan wifi dan server
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("Connecting to Wi-Fi");
while (WiFi.status() !=WL_CONNECTED)
{
Serial.print(".");
delay(300);
}
Serial.println();
Serial.print("Connected with IP: ");
Serial.println(WiFi.localIP());
Serial.println();
//penyambungan firebase
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
Firebase.reconnectWiFi(true);
Firebase.setReadTimeout(firebaseData, 1000*60);
Firebase.setwriteSizeLimit(firebaseData, "tiny");
//Serial.println("----------------------------------------");
//Serial.println("Connected...");
}
void loop() {
//Hitung Ketinggian Air Sungai
long duration1, jarak1;
digitalWrite(trigger1Pin, LOW);
delayMicroseconds(2);
digitalWrite(trigger1Pin, HIGH);
delayMicroseconds(10);
digitalWrite(trigger1Pin, LOW);
duration1 = pulseIn(echo1Pin, HIGH);
jarak1 = (duration1/2) / 29.1;
Serial.println("Ketinggian Sungai :");
Serial.print(jarak1);
Serial.println(" cm");
delay(1000);
//Hitung Ketinggian Bak Penampung
long duration2, jarak2;
digitalWrite(trigger2Pin, LOW);
delayMicroseconds(2);
digitalWrite(trigger2Pin, HIGH);
delayMicroseconds(10);
digitalWrite(trigger2Pin, LOW);
duration2 = pulseIn(echo2Pin, HIGH);
jarak2 = (duration2/2) / 29.1;
Serial.println("Ketinggian bak :");
Serial.print(jarak2);
Serial.println(" cm");
delay(1000);
//Hitung curah hujan
int kondisi_sensor = digitalRead(sensor_hujan);
if (kondisi_sensor == 1)
{Serial.println("Tidak Hujan");
delay (1000);}
else if (kondisi_sensor==0)
{Serial.println("Hujan");
delay (1000);}
//Pembukaan palang air sungaiotomattis
if (jarak1<=5)
{
Serial.println("Ketinggian sungai melebihi batas, BUKA PALANG AIR!");
digitalWrite(LedMerah,HIGH);
digitalWrite(LedHijau,LOW);
digitalWrite(buzzpin,HIGH);
delay(500);
arm.write (90);
delay(1000);
}
else {
Serial.println("Ketinggian sungai normal");
digitalWrite(LedMerah,LOW);
digitalWrite(LedHijau,HIGH);
digitalWrite(buzzpin,LOW);
delay(500);
arm.write (0);
delay(1000);
}
//Kontrol air bak penampung
if (jarak2<=5)
{
Serial.println("Ketinggian air bak penampung melebihi batas");
digitalWrite(Pump,HIGH);
}
else {
Serial.println("Ketinggian air bak penampung normal");
digitalWrite(Pump,LOW);
}
//pengaturan data firebase
if(jarak1 != jarak1firebase ){
Firebase.setDouble(firebaseData, "/CEBAN/Ketinggian Air", jarak1);
jarak1firebase = jarak1;}
if(jarak2 != jarak2firebase ){
Firebase.setDouble(firebaseData, "/CEBAN/Ketinggian Bak Penampung", jarak2);
jarak2firebase = jarak2;}
if(kondisi_sensor != hujanfirebase ){
Firebase.setDouble(firebaseData, "/CEBAN/Cuaca", kondisi_sensor);
hujanfirebase = kondisi_sensor;}
//pengaturan button palang
if (Firebase.getString(firebaseData, "/CEBAN/servo")){
servo = firebaseData.stringData();
}
if ( servo == "1"){
arm.write (90);
return;
}
else{
arm.write (0);
}
}
