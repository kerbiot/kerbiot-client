#define LOGGING 1

const int BAUD_RATE = 115200;

const double DEEP_SLEEP_DELAY = 60000;
const double DEEP_SLEEP_AFTER_NO_CONNECTION = DEEP_SLEEP_DELAY * 15;

const int TIME_TO_CONNECT = 50000;

const int PIN_TEMP_SENSOR = 4;

const float BATTERY_MAX_VOLTAGE = 5;

const int CO2_RX_PIN = 12;
const int CO2_TX_PIN = 14;
const int CO2_SENSOR_WARMUP_TIME = 10000;
