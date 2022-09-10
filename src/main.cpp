#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <config.h>
#include <log.h>
#include <secrets.h>
#include <string>

#include <senseair.h>
#include <sht31sensor.h>

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
        delay(100);
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

    SenseAirS8 *senseAirS8 = new SenseAirS8(CO2_RX_PIN, CO2_TX_PIN, CO2_SENSOR_WARMUP_TIME);
    SHT31Sensor *sht31 = new SHT31Sensor(SHT31_ADDRESS);

    connectToWifi();
    connectToMqtt();

    TemperatureAndHumidity tempH = sht31->read();
    log("Temperature: ");
    log(tempH.temperature);
    logln("°C");

    log("Humidity: ");
    log(tempH.humidity);
    logln("%");

    int co2 = senseAirS8->read(5);
    log("co2 in ppm: ");
    logln(co2);

    waitForMqtt();

    if (co2 > 0) {
        publish("CO2 in ppm", co2);
    }
    publish("Temperature in °C", tempH.temperature);
    publish("Humidity in %", tempH.humidity);
    publish("ProcessingTime in ms", millis());

    log("going to deep sleep after ");
    log(millis());
    logln("ms since reset");
    ESP.deepSleep(DEEP_SLEEP_DELAY * 1000 - millis() * 1000); // TODO: make sure not negative
}

void loop() {}