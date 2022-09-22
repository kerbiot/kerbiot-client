#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#include <config.h>
#include <log.h>
#include <secrets.h>

class KerbiotClient {
private:
    WiFiClient wiFiClient;
    WiFiClientSecure secure;
    PubSubClient mqtt;

    void startWiFi() {
        log("Connecting to: ");
        logln(WIFI_SSID);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }

    void startMqtt() {
        secure.setTrustAnchors(&MQTT_CERTIFICATES);
        mqtt.setClient(secure);
        mqtt.setKeepAlive(65535); 
        mqtt.setServer(MQTT_BROKER, MQTT_PORT);
        mqtt.connect(MQTT_DEVICE_ID, MQTT_USER, MQTT_PASSWORD);
    }

public:
    KerbiotClient() {
        startWiFi();
        startMqtt();
    }

    void wait() {
        log("Connecting to MQTT ... ");
        unsigned long timeLimit = millis() + TIME_TO_CONNECT;
        while (!mqtt.connected() && millis() < timeLimit) {
            if (WiFi.status() == WL_CONNECTED && mqtt.connect(MQTT_DEVICE_ID, MQTT_USER, MQTT_PASSWORD)) {
                logln("connected");
                return;
            }
            delay(100);
        }
        logln();
        logln("couldn't connect to MQTT broker");
        log("WiFi status: ");
        logln(WiFi.status());
        log("MQTT status: ");
        logln(mqtt.state());
        ESP.deepSleep(DEEP_SLEEP_AFTER_NO_CONNECTION * 1000);
    }

    void publish(const char *sensor, float payload) {
        std::string topic(MQTT_DEVICE_ID);
        topic.append("/");
        topic.append(sensor);

        if (!mqtt.publish(topic.c_str(), std::to_string(payload).c_str())) {
            logln("failed to publish"); // TODO: better error message
        }
    }

    void sleep() {
        log("going to deep sleep after ");
        log(millis());
        logln("ms since reset");
        ESP.deepSleep(DEEP_SLEEP_DELAY * 1000 - millis() * 1000); // TODO: make sure not negative
    }
};