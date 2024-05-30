#include <WiFi.h>
#include <HTTPClient.h>
#include <SimpleDHT.h>
const char* ssid = "24 LAU2";
const char* password = "chopuxinhdep";
const char* FIREBASE_HOST = "fir-smartfarm-1491e-default-rtdb.com";
const char* FIREBASE_AUTH = "5WA3WboiVC0qLd6Qwax5ApWVZRFYWb9EMVWmcbyi";
const char* databaseURL = "https://fir-smartfarm-1491e-default-rtdb.firebaseio.com/data.json";

#define MQ_PIN 35
#define baodong_PIN 14
#define pinDHT11 13

SimpleDHT11 dht11(pinDHT11);
void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connected to WiFi");
  pinMode(MQ_PIN, INPUT);

  pinMode(baodong_PIN, OUTPUT);

}
void loop() {
  delay(1000); // đợi cảm biến ổn định


  // đọc nhiệt độ và độ ẩm
  int percentage_air = map(analogRead(MQ_PIN), 0, 4095, 0, 100);
  int air_val = 100 - percentage_air;
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  } 
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  
  // DHT11 sampling rate is 1HZ.

  Serial.printf("Air: %d\n", air_val);

  // Send data to Firebase
  String data = String("{\"temperature\": ") + String(temperature) + 
  String(", \"humidity\": ") + String(humidity) + 
  String(",\"air\": ") + String(air_val)+ String("}");
  
  HTTPClient httpput;
  httpput.begin(databaseURL);
  httpput.addHeader("Content-Type", "application/json");
  httpput.addHeader("Authorization", FIREBASE_AUTH);

  int ResponseCode = httpput.PUT(data);

  if (ResponseCode == 200) {
    Serial.print("Data sent successfully, response code: ");
    Serial.println(ResponseCode);
  } else {
    Serial.print("Error sending data, response code: ");
    Serial.println(ResponseCode);
  }
  httpput.end();
  delay(500); // Gửi dữ liệu mỗi 0.5 giây
    if ((int)temperature > 40) 
    {
      digitalWrite(baodong_PIN, HIGH); 
    } else {
      digitalWrite(baodong_PIN, LOW); 
    }


}
