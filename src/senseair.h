#include "s8_uart.h"
#include <Softwareserial.h>

class SenseAirS8 {
private:
    SoftwareSerial *S8_serial;
    S8_UART *sensor_S8;

public:
    SenseAirS8(int rx_pin, int tx_pin) {
        S8_serial = new SoftwareSerial(rx_pin, tx_pin);
        S8_serial->begin(S8_BAUDRATE);
        sensor_S8 = new S8_UART(*S8_serial);
    }

    int read(int retries) {
        int retry = 0;
        int result = 0;

        while (retry <= retries && (result < 1 || result > 9999)) {
            result = read();
            retry++;
        }

        return result;
    }

    int read() {
        return sensor_S8->get_co2();
    }
};
