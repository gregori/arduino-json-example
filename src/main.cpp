#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

EthernetClient client;

void connect() {
  Serial.println(F("Connecting..."));

  // Connect to HTTP server
  client.setTimeout(10000);
  if (!client.connect("192.168.0.1", 80)) {
    Serial.println(F("Connection failed"));
    return;
  }

  Serial.println(F("Connected!"));
}

void getData(char data[], size_t size) {
  // Send HTTP request
  client.println(F("GET /example.json HTTP/1.0"));
  client.println(F("Host: 192.168.0.1"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  client.readBytesUntil('\r', data, size);
  /*if (strcmp(data, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(data);
    return;
  }*/

   // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }
}

DynamicJsonDocument parseDocument() {
  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  Serial.println(F("Response:"));
  Serial.println(doc["sensor"].as<char*>());
  Serial.println(doc["time"].as<long>());
  Serial.println(doc["data"][0].as<float>(), 6);
  Serial.println(doc["data"][1].as<float>(), 6);

  if (doc["led"].as<int>() == 1) {
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }
  return doc;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) continue;

  // Initialize Ethernet library
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  byte ip[] = {192, 168, 0, 10};

  Ethernet.begin(mac, ip);

  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  connect();

  size_t dataSize = 32;
  char data[dataSize];

  getData(data, dataSize);
  DynamicJsonDocument doc = parseDocument();

  
  // Extract values

  delay(1000);
}