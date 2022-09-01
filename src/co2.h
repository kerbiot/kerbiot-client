#include <Softwareserial.h>

const byte COMMAND_READ_CO2[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};
const int CO2_SENSOR_BAUD_RATE = 9600;

SoftwareSerial *serialCO2;
unsigned long started = 0;

void initializeCO2(int rx_pin, int tx_pin) {
    serialCO2 = new SoftwareSerial(rx_pin, tx_pin);
    serialCO2->begin(CO2_SENSOR_BAUD_RATE);

    started = millis();
}

void waitForCO2Heating(unsigned long warmUpTime) {
    while (millis() - started < warmUpTime) {
        delay(100);
    }
}

int readCO2inPpm() {
    byte CO2Response[] = {0, 0, 0, 0, 0, 0, 0};

    serialCO2->write(COMMAND_READ_CO2, 7);
    delay(100); // give the sensor a bit of time to respond

    if (serialCO2->available()) {
        for (int i = 0; i < 7; i++) {
            int byte = serialCO2->read();
            CO2Response[i] = byte;
            if (CO2Response[0] != 254) {
                return -1;
            }
        }
        unsigned long val = CO2Response[3] * 256 + CO2Response[4];
        return val;
    } else {
        return -2;
    }
}
