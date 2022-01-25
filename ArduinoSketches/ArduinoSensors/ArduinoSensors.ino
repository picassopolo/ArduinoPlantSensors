#include <RTCZero.h>
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Adafruit_seesaw.h"

//Soil sensor global variables
Adafruit_seesaw soilSensor1; //First soil sensor (0x36)
Adafruit_seesaw soilSensor2; //Second soil sensonr (0x37)
Adafruit_seesaw soilSensor3; //Third soil sensor (0x38)

//Temperature sensor global variables
#define DHTPIN 2 //Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 //Type of DHT sensor
DHT_Unified dht(DHTPIN, DHTTYPE); //Temperature & Humidity sensor
DHT dht2(DHTPIN, DHTTYPE);
sensors_event_t event1;
sensors_event_t event2;

//Light sensor
int photocellPin = 3; //the cell and 10K pulldown are connected to a3

//Wifi connection
char ssid[] = WIFI_SSID_NAME;
char pass[] = WIFI_PASSWORD;

//HTTPClient connection
const char serverName[] = SERVER_ADDRESS;
WiFiClient wifi;
HttpClient client = HttpClient( wifi, serverName);
int status = WL_IDLE_STATUS;

RTCZero rtc;

void setup()
{
  Serial.begin(9600);
  delay(1500); //Allows the console to open before outputing


  //Wait for the card to find a network
  while ( status != WL_CONNECTED )
  {
    Serial.print( "Attempting to connect to Network named: " );
    Serial.println( ssid );
    status = WiFi.begin( ssid, pass );
  }

  //Prints connection infos
  Serial.print( "SSID: " );
  Serial.println( WiFi.SSID() );
  IPAddress ip = WiFi.localIP();
  Serial.print( "IP Address: " );
  Serial.println( ip );

  //Starts soil sensor 1
  if (soilSensor1.begin(0x36))
  {
    Serial.println("Soil sensor 1 started!");
  }
  else
  {
    Serial.println("ERROR: SOIL SENSOR 1 NOT FOUND!");
    while (1);
  }

  //Starts soil sensor 2
  if (soilSensor2.begin(0x37))
  {
    Serial.println("Soil sensor 2 started!");
  }
  else
  {
    Serial.println("ERROR: SOIL SENSOR 2 NOT FOUND!");
  }

  //Starts soil sensor 3
  if (soilSensor3.begin(0x38))
  {
    Serial.println("Soil sensor 3 started!");
  }
  else
  {
    Serial.println("ERROR: SOIL SENSOR 3 NOT FOUND!");
  }

  //Temperature sensor setup
  dht.begin();
  sensor_t sensor;

  //Waiting 10 seconds to make sure connection is established
  delay(10000);

  //Setting up time
  rtc.begin();
  unsigned long epoch = WiFi.getTime();
  epoch += -18000;
  rtc.setEpoch(epoch);
  
  PrintTime();

  //Setup alarm to trigger every hour
  rtc.setAlarmTime(0, 0, 0);
  rtc.enableAlarm(rtc.MATCH_MMSS);
  rtc.attachInterrupt(SensorReading);
}

void loop()
{
  //Prints temperature & humidity sensor read every five minutes
  dht.temperature().getEvent(&event1);
  dht.humidity().getEvent(&event2);
  delay(300000);
}

void SensorReading()
{
  //variables to convert readings to string
  String tempString = "";
  String humString = "";
  String lightString = "";
  String sensor1String = "";
  String sensor2String = "";
  String sensor3String = "";

  //Take reading from soil sensor
  uint16_t capread1 = soilSensor1.touchRead(0);
  uint16_t capread2 = soilSensor2.touchRead(0);
  uint16_t capread3 = soilSensor3.touchRead(0);

  //Displays soil sensor infos
  Serial.print("Sensor 1: "); Serial.println(capread1);
  Serial.print("Sensor 2: "); Serial.println(capread2);
  Serial.print("Sensor 3: "); Serial.println(capread3);
  
  //Reads and print the light sensor
  uint16_t lightread = analogRead(photocellPin);
  Serial.print("Light: ");
  Serial.println(lightread);

  //Converts the datas to String
  tempString.concat(event1.temperature);
  humString.concat(event2.relative_humidity);
  sensor1String.concat(capread1);
  sensor2String.concat(capread2);
  sensor3String.concat(capread3);
  lightString.concat(lightread);

  //POST the data to the server
  String contentType = "application/json";
  String postData = "{\"temperature\":" + tempString + ",\"humidity\":" + humString + ",\"light\":" + lightString + ",\"sensor1\":" + sensor1String + ",\"sensor2\":" + sensor2String + ",\"sensor3\":" + sensor3String + "}";
  client.post( "/SensorAPI/SensorData", contentType, postData );
  Serial.print("data sent :");
  Serial.println(postData);

  //Server response
  int statusCode = client.responseStatusCode();
  Serial.print( "Status code: " );
  Serial.println( statusCode );
  String response = client.responseBody();
  Serial.print( "Response: " );
  Serial.println( response );
  client.stop();
}

void PrintTime()
{
  Serial.print(rtc.getDay());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.print(rtc.getYear());
  Serial.print("\t");

  Serial.print(rtc.getHours());
  Serial.print(":");
  Serial.print(rtc.getMinutes());
  Serial.print(":");
  Serial.println(rtc.getSeconds());
}
