/***Library*******************************************************************************************************/
#include <FirebaseArduino.h> //firebase library
#include <ESP8266WiFi.h>  //ESP8266 library
#include <DHT.h>  //DHT11 library
#include <WiFiUdp.h>
#include <String.h>
/***Wifi-and-Firebase*******************************************************************************************************************************/
#define ssid  "" //WIFI name
#define pass  ""  //WIFI passwords
#define FIREBASE_HOST "" //Firebase Host
#define FIREBASE_AUTH "" //Firebase Auth key
#define time2SendData 1 //time to send sensor data to firebase, "1" = 10seconds
/**********************************************************************************************************************************/
#define PIR_PIN D5
#define LIGHT_PIN D7
#define DOOR_PIN D6          
#define DHTTYPE DHT22
#define dht_dpin 2 //D4
const int relay = 5;
DHT dht(dht_dpin, DHTTYPE); 
float t;
float h;
//PIR mặc định = 0
//cảm biến ánh sáng mặc định = 1, có ánh sáng = 0
//cảm biến cửa cắm 1 chân vào nguồn 3v3, 1 chân vào D6, dùng 1 điện trở 220 nối từ D6 xuống Gnd, mặc định = 0, khi tiếp xúc = 1
void readPIR();
void readLight();
void dth();

void setup() 
{
    Serial.begin(9600);
    pinMode(relay, OUTPUT);
    dht.begin();
      /**********************************************************************************************************************************/       
    Serial.println("Connecting to ");
    Serial.println(ssid); 
    WiFi.begin(ssid, pass); 
    while (WiFi.status() != WL_CONNECTED) //wait for wifi t be connected
      {
        delay(500);
        Serial.print(".");
      }
    Serial.println("");
    Serial.println("WiFi connected"); 
    /**********************************************************************************************************************************/
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //initialize firebase communication
    Serial.println(Firebase.error());
    /**********************************************************************************************************************************/
}

void loop() 
{
  readPIR();
  readDoor();
  dth();
}

void readPIR()
{
  int PIR = digitalRead(PIR_PIN);
  //Serial.print("PIR = ");
  //Serial.println(PIR);
  if (PIR == 1)
  {
    Serial.print("Dang Gan Cua");
    Firebase.setInt("/Danger/DoorPassed", 1); //ra gần cửa, gửi về firebase
  } else {
    Serial.print("Dang Khong Gan Cua");
    Firebase.setInt("/Danger/DoorPassed", 0); //ra xa cửa, gửi về firebase
  }
  Serial.println();
}

void readLight()
{
  int LIGHT = digitalRead(LIGHT_PIN);
  if (LIGHT == 0)
  {
    Serial.print("TV dang sang");
    Firebase.setInt("/DeviceState/TVLight", 1); //TV on, gửi về firebase
  } else {
    Serial.print("TV dang toi");
    Firebase.setInt("/DeviceState/TVLight", 0); //TV off, gửi về firebase
  }
  Serial.println();
}

void readDoor()
{
  int DOOR = digitalRead(DOOR_PIN);
  //Serial.print("Door = ");
  //Serial.println(DOOR);
  if (DOOR == 1)
  {
    Firebase.setInt("/Sensor/Door", DOOR); //Tín hiệu cửa mở
    Firebase.setInt("/Danger/DoorOpen", 0); //Tín hiệu cảnh báo nguy hiểm
    Serial.print("Cua dang dong");
  } else {
    Firebase.setInt("/Sensor/Door", DOOR); //Tín hiệu cửa mở
    Firebase.setInt("/Danger/DoorOpen", 1); //Tín hiệu cảnh báo không nguy hiểm
    Serial.print("Cua dang mo");
  }
  Serial.println();
}

void relaycontrol()
{
  int TVstate = Firebase.getInt("DeviceState/TV"); 
//  Serial.print("Relay:");
  //Serial.print(digitalRead(relay));
  //Serial.print("\n");
  //Serial.print(TVstate);
  if (1 == TVstate){
    Serial.print("TV duoc phep bat");
    digitalWrite(relay, HIGH);
  }
  if (0 == TVstate){
    Serial.print("TV dang tat");
    digitalWrite(relay, LOW);
  }
  Serial.println();

}

void dth()
{
  delay(600);
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t); 
  Serial.println();
  String fireHumid = String(h);                                         //convert integer humidity to string humidity 
  String fireTemp = String(t);   
  Firebase.setString("/Sensor/Humidity", fireHumid);                                  //setup path and send readings
  Firebase.setString("/Sensor/Temperature", fireTemp);
}
