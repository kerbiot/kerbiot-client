#include <Wire.h>

struct TemperatureAndHumidity {
    float temperature;
    int humidity;
    int error;
};

TwoWire temperatureAndHumidityWire;
int wireAddress;

int writeCommand(int command) {
    temperatureAndHumidityWire.beginTransmission(wireAddress);
    temperatureAndHumidityWire.write(command >> 8);
    temperatureAndHumidityWire.write(command & 0xFF);
    return (-10 * temperatureAndHumidityWire.endTransmission());
}

int initializeTemperatureHumidity(int address) {
    wireAddress = address;

    return writeCommand(10039);
}

int calculateCrc(uint8_t data[]) {
    uint8_t bit;
    uint8_t crc = 0xFF;
    uint8_t dataCounter = 0;

    for (; dataCounter < 2; dataCounter++) {
        crc ^= (data[dataCounter]);
        for (bit = 8; bit > 0; --bit) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x131;
            else
                crc = (crc << 1);
        }
    }

    return crc;
}

int checkCrc(uint8_t data[], uint8_t checksum) {
    return calculateCrc(data) != checksum;
}

int readRawTempH(uint16_t *data, int numOfPair) {
    uint8_t buf[2];
    uint8_t checksum;

    const int numOfBytes = numOfPair * 3;
    temperatureAndHumidityWire.requestFrom(wireAddress, numOfBytes);

    int counter = 0;

    for (counter = 0; counter < numOfPair; counter++) {
        temperatureAndHumidityWire.readBytes(buf, (uint8_t)2);
        checksum = temperatureAndHumidityWire.read();

        if (checkCrc(buf, checksum) != 0)
            return -1;

        data[counter] = (buf[0] << 8) | buf[1];
    }

    return 0;
}

float calculateTemperature(uint16_t rawValue) {
    float value = 175.0f * (float)rawValue / 65535.0f - 45.0f;
    return (value * 10) / 10;
}

float calculateHumidity(uint16_t rawValue) {
    return 100.0f * rawValue / 65535.0f;
}

TemperatureAndHumidity readTemperatureAndHumidity() {
    TemperatureAndHumidity result;

    result.temperature = 0;
    result.humidity = 0;

    uint16_t buf[2];
    int error = readRawTempH(buf, 2);

    if (error == 0) {
        result.temperature = calculateTemperature(buf[0]);
        result.humidity = calculateHumidity(buf[1]);
    }
    result.error = error;

    return result;
}