#include <Arduino.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <config.h>
#include <secrets.h>
#include <log.h>
#include <string>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature tempsensor(&oneWire);

int lightSensor() {
    int sensorValue = analogRead(A0);
    logln(sensorValue);
    return sensorValue;
}

float tempSensor() {
    tempsensor.begin();
    tempsensor.requestTemperatures();
    float temperatureC = tempsensor.getTempCByIndex(0);
    log(temperatureC);
    logln("°C");
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
    log("Connecting to: ");
    logln(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
    secure.setTrustAnchors(&MQTT_CERTIFICATES);
    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.connect(MQTT_DEVICE_ID, MQTT_USER, MQTT_PASSWORD);
}

void waitForMqtt() {
    // TODO: error handling

    logln("Connecting to MQTT...");
    while (!client.connected()) {
        if (WiFi.status() == WL_CONNECTED && client.connect(MQTT_DEVICE_ID, MQTT_USER, MQTT_PASSWORD)) {
            logln("connected");
        }
        delay(250);
    }
}

void setup() {
    setupLogger(BAUD_RATE);
    logln();
    connectToWifi();
    connectToMqtt();
    int lightValue = lightSensor();
    float tempValue = tempSensor();

    waitForMqtt();
    publish("Light", lightValue);
    publish("Temperature", tempValue);

    log("going to deep sleep after ");
    log(millis());
    logln("ms since reset");
    ESP.deepSleep(DEEP_SLEEP_DELAY - millis() * 1000);
}

void loop() {}