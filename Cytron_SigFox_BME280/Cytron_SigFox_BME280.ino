/***************************************************************************
Cyton Sigfox Shield Demo - BME280 + SigFox WISOL
 ***************************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "SIGFOX.h"

#define UINT16_t_MAX  65536
#define INT16_t_MAX   UINT16_t_MAX/2

Adafruit_BME280 bme; // I2C BME280 sensor 0x60

unsigned long delayTime; //set delayTime for next transmission

static const String device = "CytronSigFox";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_SG;  //  Set this to your country to configure the SIGFOX transmission frequencies.
static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield V2S Dev Kit

//Create a data packet to send data
typedef struct __attribute__ ((packed)) sigfox_message {
  int16_t bmeTemp;
  uint16_t bmePres;
  uint16_t bmeHumi;
} SigfoxMessage;

// Stub for message which will be sent
SigfoxMessage msg;


void setup() {
    Serial.begin(9600);
    while(!Serial);    // time to get serial running

    if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));

//    Send a raw 12-byte message payload to SIGFOX. (Uncomment this to test transmission)
//    transceiver.sendMessage("0102030405060708090a0b0c");
  
    //  Delay 10 seconds before sending next message.
    Serial.println(F("Waiting 10 seconds..."));
    delay(10000);

    unsigned status;
    Serial.println(F("BME280 Sensor Detection"));
    status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1);
    }

/*  // BME280 Default sampling mode
    Serial.println("-- Default Test --");
    Serial.println("normal mode, 16x oversampling for all, filter off,");
    Serial.println("0.5ms standby period");
    delayTime = 5000;
*/    
    // weather monitoring sampling mode
    Serial.println("-- Weather Station Scenario 10 Min--");
    Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
    Serial.println("filter off");
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF   );
                      
    // suggested rate is 1/600Hz (10m)
    delayTime = 600000; // in milliseconds

    Serial.println();
}

void loop() {
    // Only needed in forced mode! In normal mode, you can remove the next line.
    bme.takeForcedMeasurement(); // has no effect in normal mode
    
    printValues();
    delay(delayTime);
}

int16_t convertoFloatToInt16(float value, long max, long min) {
  float conversionFactor = (float) (INT16_t_MAX) / (float)(max - min);
  return (int16_t)(value * conversionFactor);
}

uint16_t convertoFloatToUInt16(float value, long max, long min = 0) {
  float conversionFactor = (float) (UINT16_t_MAX) / (float)(max - min);
  return (uint16_t)(value * conversionFactor);
}

void printValues() { 
    float BMETemp = bme.readTemperature();
    msg.bmeTemp = convertoFloatToInt16(BMETemp, 60, -60);
    float BMEPres = bme.readPressure();
    msg.bmePres = convertoFloatToUInt16(BMEPres, 200000);
    float BMEHumi = bme.readHumidity();
    msg.bmeHumi = convertoFloatToUInt16(BMEHumi, 110);
    
    String encodeMsg = transceiver.toHex((uint8_t*)&msg, 12);
    
    Serial.print("Temperature = ");
    Serial.print(BMETemp);
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(BMEPres);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(BMEHumi);
    Serial.println(" %");
    Serial.println();

    Serial.print("Encoded Message = ");
    Serial.print(encodeMsg);
    Serial.println();

    //  Send the message.
    Serial.println(F("\n>> Device sending message "));
    transceiver.sendMessage(encodeMsg);
}
