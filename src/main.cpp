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

float tempSensor() {
    OneWire oneWire(PIN_TEMP_SENSOR);       // Setup a oneWire instance to communicate with any OneWire devices
    DallasTemperature tempsensor(&oneWire); // Pass our oneWire reference to Dallas Temperature sensor
    tempsensor.begin();
    tempsensor.requestTemperatures();
    float temperatureC = tempsensor.getTempCByIndex(0);
    log(temperatureC);
    logln("°C");
    return temperatureC;
}

float batterySensor() {
    int value = analogRead(A0);
    float batteryVoltage = ((float)value / 1024) * BATTERY_MAX_VOLTAGE; // TODO: map to 0 - 100
    log(batteryVoltage);
    logln("V");
    return batteryVoltage;
}

WiFiClient wiFiClient;
WiFiClientSecure secure;
PubSubClient client(secure);

void publish(const char *sensor, float payload) {
    std::string topic(MQTT_DEVICE_ID);
    topic.append("/");
    topic.append(sensor);

    if (!client.publish(topic.c_str(), std::to_string(payload).c_str())) {
        logln("failed to publish"); // TODO: better error message
    }
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
    logln("Connecting to MQTT...");
    unsigned long timeLimit = millis() + TIME_TO_CONNECT;
    while (!client.connected() && millis() < timeLimit) {

        if (WiFi.status() == WL_CONNECTED && client.connect(MQTT_DEVICE_ID, MQTT_USER, MQTT_PASSWORD)) {
            logln("connected");
            return;
        }
        delay(200); // TODO: test power consumption
    }
    logln("couldn't connect to MQTT broker");
    log("WiFi status: ");
    logln(WiFi.status());
    log("MQTT status: ");
    logln(client.state());
    ESP.deepSleep(DEEP_SLEEP_AFTER_NO_CONNECTION * 1000);
}

void setup() {
    setupLogger(BAUD_RATE);
    logln();

    connectToWifi();
    connectToMqtt();

    float temperature = tempSensor();
    float batteryVoltage = batterySensor();

    waitForMqtt();

    publish("Temperature", temperature);
    publish("Battery", batteryVoltage);

    log("going to deep sleep after ");
    log(millis());
    logln("ms since reset");
    ESP.deepSleep(DEEP_SLEEP_DELAY * 1000 - millis() * 1000);
}

void loop() {}