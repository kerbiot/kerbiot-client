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

#if LOGGING
        if (retry >= retries) {
            this->diagnose();
        }
#endif

        return result;
    }

    int read() {
        return sensor_S8->get_co2();
    }

#if LOGGING
    void diagnose() {
        logln("Performing SenseAir S8 CO2 sensor diagnosis ...")
            S8_sensor sensor;

        sensor_S8->get_firmware_version(sensor.firm_version);
        int len = strlen(sensor.firm_version);
        if (len == 0) {
            logln("SenseAir S8 CO2 sensor not found! (no firmware version)");
            return;
        }
        logln(">>> SenseAir S8 NDIR CO2 sensor <<<");
        logln("Firmware version: ");
        logln(sensor.firm_version);
        log("Sensor ID: ");
        logln(sensor_S8->get_sensor_ID());
        log("ABC period: ");
        logln(sensor_S8->get_ABC_period());

        logln("Performing health check ...");
        sensor.meter_status = sensor_S8->get_meter_status();

        if (sensor.meter_status & S8_MASK_METER_ANY_ERROR) {
            logln("One or more errors detected!");
            if (sensor.meter_status & S8_MASK_METER_FATAL_ERROR) {
                logln("Fatal error in sensor!");
            }
            if (sensor.meter_status & S8_MASK_METER_OFFSET_REGULATION_ERROR) {
                logln("Offset regulation error in sensor!");
            }
            if (sensor.meter_status & S8_MASK_METER_ALGORITHM_ERROR) {
                logln("Algorithm error in sensor!");
            }
            if (sensor.meter_status & S8_MASK_METER_OUTPUT_ERROR) {
                logln("Output error in sensor!");
            }
            if (sensor.meter_status & S8_MASK_METER_SELF_DIAG_ERROR) {
                logln("Self diagnostics error in sensor!");
            }
            if (sensor.meter_status & S8_MASK_METER_OUT_OF_RANGE) {
                logln("Out of range in sensor!");
            }
            if (sensor.meter_status & S8_MASK_METER_MEMORY_ERROR) {
                logln("Memory error in sensor!");
            }
        } else {
            logln("The sensor is OK.");
        }

        logln("SenseAir S8 CO2 sensor done");
    }
#endif
};
