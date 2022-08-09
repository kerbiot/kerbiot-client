#include <Arduino.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <config.h>
#include <log.h>
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

float batterieSensor() {
    int value = analogRead(A0);
    int mappedValue = map(value, 0, 1024, 0, 4500);
    log(mappedValue);
    logln("mV");
    // float batterieVoltage = mappedValue / 1000;
    // log(batterieVoltage);
    // logln("V");
    return mappedValue;
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

    // int lightValue = lightSensor();
    float tempValue = tempSensor();
    float batterieValue = batterieSensor();

    waitForMqtt();

    // publish("Light", lightValue);
    publish("Temperature", tempValue);
    publish("batterie", batterieValue);

    log("going to deep sleep after ");
    log(millis());
    logln("ms since reset");
    ESP.deepSleep(DEEP_SLEEP_DELAY - millis() * 1000);
}

void loop() {}