#include "SHT31.h"
#include "Wire.h"

struct TemperatureAndHumidity {
    float temperature;
    int humidity;
};

class SHT31Sensor {
private:
    SHT31 sht;

public:
    SHT31Sensor(int address) {
        Wire.begin();
        sht.begin(address);
        Wire.setClock(100000);
    }

    TemperatureAndHumidity read() {
        sht.read();
        TemperatureAndHumidity result;
        result.temperature = sht.getTemperature();
        result.humidity = sht.getHumidity();
        return result;
    }
};