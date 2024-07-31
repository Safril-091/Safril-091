//include library Wifi pada esp8266 dan library sensor dht11
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ThingESP.h>
#include <DHT.h>

//konfigurasi WiFi
const char* ssid = "BsB"; // Nama Wifi
const char* password = "33333333"; // Password Wifi

//Variabel pin out pada esp8266
#define pinSensor 5   //D1 = GPIO 5
#define pinGas 4      //D2 = GPIO 4
#define pinLED 0      //D3 = GPIO 0
#define pinBuzzer 16  //D0 = GPIO 16
#define DHTPIN 2      //D4 = GPIO 2 
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

ThingESP8266 thing("DHT11", "sensorsuhu", "sensorsuhu123");

unsigned long previousMillis = 0;
const long INTERVAL = 6000;  

//Variabel URL
String url;

//Variable WiFi Client
WiFiClient client;

void setup() {
  Serial.begin(9600);
  pinMode(pinSensor, INPUT);  //sebagai input data api
  pinMode(pinGas, INPUT);     //sebagai input data gas atau asap
  pinMode(pinLED, OUTPUT);    //sebagai output data jika internet sudah terhubung
  pinMode(pinBuzzer, OUTPUT); //sebagai output data dari sensor api dan gas

  thing.SetWiFi("BsB", "33333333"); //SSID dan Password Wifi agar terhubung ke thingesp

  thing.initDevice();

  //konek ke wifi
  WiFi.hostname("NodeMCU8622");
  WiFi.begin(ssid, password);
  //uji wifi
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(pinLED, LOW);
    delay(100);
  }

  //apabila terkoneksi
  digitalWrite(pinLED, HIGH);
}

String HandleResponse(String query)
{

 float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  Serial.println("Temperature :");
  Serial.print(t);
  Serial.println("Humidity");
  Serial.print(h);
  String humid = "Humidity: " + String(h) + " % \n";
  String temp = "Temperature: " + String(t) + "°C, " + String(f) + "°F \n";

  if (query == "temperature") {
   
    return temp;
  }

  else if (query == "humidity") {
  
    return humid;
  }
 
  else return "Your query was invalid..";

}

void loop() 
{
  float suhu = dht.readTemperature(); //menyimpan nilai suhu pada variabel suhu
  int api = digitalRead(pinSensor); //baca nilai sensor api
  int gas = digitalRead(pinGas);    //baca nilai sensor gas
  if (api == 1) {
    //tidak ada api, wa tidak muncul
    Serial.println("Tidak Ada Api");
    digitalWrite(pinBuzzer, LOW);
  } else {
    //terdeteksi api, wa muncul, buzzer bunyi
    Serial.println("Ada Titik Api");
    digitalWrite(pinBuzzer, HIGH);

    //kirim ke wa
    kirim_wa("TERDETEKSI APII !!!");
  }

  if (gas == 1) {
    //tidak ada gas, wa tidak muncul
    Serial.println("Tidak Ada gas");
    digitalWrite(pinBuzzer, LOW);
  } else {
    //terdeteksi gas, wa muncul, buzzer bunyi
    Serial.println("Ada Titik gas");
    digitalWrite(pinBuzzer, HIGH);

    //kirim ke wa
    kirim_wa("TERDETEKSI GAS ATAUPUN ASAP !!!");
  }

  if (suhu > 35.00) {
    digitalWrite(pinBuzzer, HIGH);

    Serial.print(suhu);

    kirim_wa("SUHU TINGGI MELEBIHI 40°C");

    delay(10000);
  } else {
    digitalWrite(pinBuzzer, LOW);

    Serial.print(suhu);
  }

  delay(500);
  thing.Handle();
}

void kirim_wa(String pesan)   //memasukan apikey bot yang telah dibuat pada whatsapps
{
  url = "http://api.callmebot.com/whatsapp.php?phone=6289513259986&text="+ urlencode(pesan) +"&apikey=9309991";
  postData();
}

void postData() {
  int httpCode;
  HTTPClient http;
  http.begin(client, url);
  httpCode = http.POST(url);
  if (httpCode == 200) {
    Serial.println("Notif Terkirim");
  } else {
    Serial.println("Notif Gagal");
  }
  http.end();
}

String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0, code1, code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
    yield();
  }
  Serial.println(encodedString);
  return encodedString;
}