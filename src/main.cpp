#include <Arduino.h>
#include <kerbiotclient.h>
#include <log.h>
#include <senseair.h>
#include <sht31sensor.h>
#include <string>

void setup() {
    setupLogger(BAUD_RATE);
    logln();

    SenseAirS8 *senseAirS8 = new SenseAirS8(CO2_RX_PIN, CO2_TX_PIN);
    SHT31Sensor *sht31 = new SHT31Sensor(SHT31_ADDRESS);

    KerbiotClient *kerbiot = new KerbiotClient();

    TemperatureAndHumidity tempH = sht31->read();

    int16_t co2 = senseAirS8->read(5);

    kerbiot->wait();

    if (co2 > 0) {
        kerbiot->publish("CO2 in ppm", co2);
    }
    kerbiot->publish("Temperature in °C", tempH.temperature);
    kerbiot->publish("Humidity in %", tempH.humidity);
    kerbiot->publish("ProcessingTime in ms", millis());

    kerbiot->sleep();
}

void loop() {}