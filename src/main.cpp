#include <Arduino.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <config.h>
#include <secrets.h>
#include <string>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature tempsensor(&oneWire);

int lightSensor() {
    int sensorValue = analogRead(A0);
    Serial.println(sensorValue);
    return sensorValue;
}

float tempSensor() {
    tempsensor.begin();
    tempsensor.requestTemperatures();
    float temperatureC = tempsensor.getTempCByIndex(0);
    Serial.print(temperatureC);
    Serial.println("°C");
    return temperatureC;
}

WiFiClient wiFiClient;
WiFiClientSecure secure;
PubSubClient client(secure);

void publish(const char *sensor, float payload) {
    std::string topic(MQTT_DEVICE_ID);
    topic.append("/");
    topic.append(sensor);

    client.publish(topic.c_str(), std::to_string(payload).c_str());
}

void connectToWifi() {
    Serial.print("Connecting to: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
    secure.setTrustAnchors(&MQTT_CERTIFICATES);
    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.connect(MQTT_DEVICE_ID, MQTT_USER, MQTT_PASSWORD);
}

void waitForMqtt() {
    // TODO: error handling

    Serial.println("Connecting to MQTT...");
    while (!client.connected()) {
        if (WiFi.status() == WL_CONNECTED && client.connect(MQTT_DEVICE_ID, MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("connected");
        }
        delay(250);
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    Serial.println();
    connectToWifi();
    connectToMqtt();
    int lightValue = lightSensor();
    float tempValue = tempSensor();

    waitForMqtt();
    publish("Light", lightValue);
    publish("Temperature", tempValue);

    Serial.print("going to deep sleep after ");
    Serial.print(millis());
    Serial.println("ms since reset");
    ESP.deepSleep(DEEP_SLEEP_DELAY - millis() * 1000);
}

void loop() {}