#include <WiFiClientSecure.h>
#include <string>

const char *WIFI_SSID = "******";
const char *WIFI_PASSWORD = "******";

const char *MQTT_BROKER = "7b7b7b7b.nip.io";
const int MQTT_PORT = 8883;
const char *MQTT_USER = "******";
const char *MQTT_PASSWORD = "******";
const char *MQTT_DEVICE_ID = "DeviceID";

const char MQTT_CERTIFCATE[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)CERT";

X509List MQTT_CERTIFICATES(MQTT_CERTIFCATE);