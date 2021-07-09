#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_BME280.h>


//Define
#define MSG_BUFFER_SIZE	(50)
#define BME_ID 0x76

//Main loop frecuency 30sg
constexpr int main_loop_frecuency = 0.1 * 60 * 1000;

//Wifi connection variables
const char* ssid = "***";
const char* password = "***";
const char* mqtt_server = "***";

WiFiClient espClient;

PubSubClient mqtt_client(espClient);

Adafruit_BME280 bme;

char mqtt_msg[MSG_BUFFER_SIZE];

void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt_client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  
  //Configure Serial Port for debugging.
  Serial.begin(115200);
  Serial.setTimeout(2000);

  while(!Serial) 
  {
  }

  //Configure Wifi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  Serial.print("Connecting to WiFi...");
  while( WiFi.status()!=WL_CONNECTED ) 
  {
    Serial.print('.');
    delay(100);
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  //Configure mqtt server & callback
  mqtt_client.setServer(mqtt_server,1883);
  mqtt_client.setCallback(mqtt_callback);

  Wire.begin (21, 22);   // sda= GPIO_21 /scl= GPIO_22

  if (!bme.begin(BME_ID)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
}

void loop() {

  //Serial.println("Weather Station!");

  if (!mqtt_client.connected()) 
  {
    mqtt_reconnect();
  }

  double bme_temperature= bme.readTemperature();
  double bme_humidity= bme.readHumidity();
  double bme_pressure= bme.readPressure()/100.0F;;
  

  snprintf (mqtt_msg, MSG_BUFFER_SIZE, "T|%f|H|%f|P|%f",bme_temperature,bme_humidity,bme_pressure);
  mqtt_client.publish("/4jggokgpepnvsb2uv4s40d59ov/motion001/attrs", mqtt_msg);

  delay(main_loop_frecuency);
}
