#include <Softwareserial.h>

const byte COMMAND_READ_CO2[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};
const int CO2_SENSOR_BAUD_RATE = 9600;

class SenseAirS8 {
private:
    unsigned long startedAt;
    unsigned long warmUpTime;
    SoftwareSerial *serialCO2;

public:
    SenseAirS8(int rx_pin, int tx_pin, unsigned long warmUpTime) {
        serialCO2 = new SoftwareSerial(rx_pin, tx_pin);
        serialCO2->begin(CO2_SENSOR_BAUD_RATE);

        this->warmUpTime = warmUpTime;

        startedAt = millis();
    }

    void wait() {
        while (millis() - startedAt < warmUpTime) {
            delay(100);
        }
    }

    int read(int retries) {
        int result = read();
        if (result > 0)
            return result;

        int retry = 0;

        while (retry <= retries) {
            int result = read();
            if (result > 0)
                return result;
            retry++;
        }

        return result;
    }

    int read() {
        wait();

        byte CO2Response[] = {0, 0, 0, 0, 0, 0, 0};

        serialCO2->write(COMMAND_READ_CO2, 7);
        delay(100);

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
};
