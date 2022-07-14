// Libraries needed for DHT 11 sensor
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
//Libraries needed to connect to MQTT server
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//Libraries needed for json file
#include <ArduinoJson.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "LANCOMBEIA"; // your network SSID (name)
const char* password = "beialancom"; // your network password (use for WPA, or use as key for WEP)
const char* mqtt_server = "mqtt.beia-telemetrie.ro";
const char* topic = "training/esp8266/AgniaCodreanu"; //don't know where it is used

StaticJsonDocument<512> doc;// Use a StaticJsonDocument to store in the stack (recommended for documents smaller than 1KB)

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
String msgStr = "";
char mess[512];
int counter;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  //????

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "Reconnected!");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void mqtt_pub_sub(float i , char* k)
{
  doc["sennsor"] = k;
  doc["value"] = i;

  serializeJson(doc, mess);
  client.publish(topic, mess);
  client.subscribe(topic);
  msgStr = "";
  delay(50);

}
void sensor(){
  float h = dht.readHumidity(); //read humidity
  float t = dht.readTemperature(); // read temperature
  //check if any reads failed and exit to try again
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.println(F("------------------------------------"));
  
  // Print humidity sensor details.
  Serial.print(F(" Humidity: "));
  Serial.print(h);
 // mqtt_pub_sub(h, "Humidity");
  // Print temperature sensor details.
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
//  mqtt_pub_sub(t, "Temperature");
  doc["sensor"] ="Temperatura";
  doc["TC"] = t;
   doc["sensor"] = "Humidity";
  doc["HU"] = h;

  serializeJson(doc, mess);
  client.publish(topic, mess);
  client.subscribe(topic);
  msgStr = "";
  delay(50);
  Serial.println();
  delay(500);
  
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  sensor();
  delay(50);
 
}
