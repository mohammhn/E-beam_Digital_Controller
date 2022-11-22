/***************************************************
  This is firmware code for Digipots DS3502
 ****************************************************/

#include <Arduino.h>
#include <Adafruit_DS3502.h>

#define DS3502_I2CADDR_DEFAULT 0x28 ///< DS3502 default I2C address
#define DS3502_I2CADDR_SECONDARY 0x29 ///< DS3502 Second I2C address

class Digipots_DS3502 {
  public:
    Adafruit_DS3502 digiPot_course;
    Adafruit_DS3502 digiPot_fine;
    float maxVoltage_hardcode = 10.000; // 10v hardcoded limit
    uint16_t numDigipots_hardcode = 2;
    uint32_t bitResolution_D = 7;
    uint32_t numSteps_D = pow(2, bitResolution_D);
    uint16_t numDigipots = numDigipots_hardcode;

    int units_D = 0;
    // int semicolonIndex_D = -1;
    float maxVolt = maxVoltage_hardcode;


    Digipots_DS3502() {
      digiPot_course = Adafruit_DS3502();;
      digiPot_fine = Adafruit_DS3502();;
      maxVolt = maxVoltage_hardcode;
      units_D = 0;
      // semicolonIndex_D = -1;
    }

    void initialize_digipots() {
      maxVolt = maxVoltage_hardcode;
      units_D = 0;
      //// MAKE SURE DIGIPOTS RECOGNIZED ////
      if (!digiPot_course.begin(0x28)) {
        Serial.println("Couldn't find DS3502 coarse chip");
        while (1);
      }
      if (!digiPot_fine.begin(0x29)) {
        Serial.println("Couldn't find DS3502 fine chip");
        while (1);
      }
      // Serial.println("Found DS3502 chips");
      digiPot_course.setWiper(0);
      digiPot_fine.setWiper(0);
      Serial.println("Initialized DS3502 chips");
    }

    void resetDigipots() {
      digiPot_fine.setWiper(0);
      digiPot_course.setWiper(0);
      readDigipots(0);
    }

    void set_maxVoltage(String argString) {
      float volt_new = argString.toFloat();

      if (volt_new <= maxVoltage_hardcode && volt_new > 0) {
        maxVolt = volt_new;
        Serial.println("CONFIG UPDATE: Max Voltage = " + String(maxVolt));
      } else {
        Serial.println("Error, voltage out of range! Input Current:" + String(volt_new));
      }
    }

    // if unitsRead = 0, outputs raw PWM values (uint16_t)
    void readDigipots(int unitsRead) {
      units_D = unitsRead;
      uint8_t value_course = digiPot_course.getWiper();
      uint8_t value_fine = digiPot_fine.getWiper();
      uint8_t values[2] = {value_course, value_fine};
      Serial.println("0:" + String(values[0]) + ",1:" + String(values[1]));
    }

    // sets Digipot, values are 0 indexed. i.e. 0 is digipot course, 1 is digipot fine
    void setDigipot(int channel, int value_int) {
      if (channel < 0 || channel >= numDigipots) {
        Serial.println("Error, channel value not in range! Channel:" + String(channel));
        return;
      }

      // uint8_t value_int8 = (uint8_t) value;

      uint8_t value = (uint8_t) value_int;

      // make sure value in range
      value = (value > numSteps_D - 1) ? (numSteps_D - 1) : value;
      value = (value < 0) ? 0 : value;

      uint8_t valueRead = 0;
      // set PWM value of channel
      if (channel == 0) {
        digiPot_course.setWiper(value);
        valueRead = digiPot_course.getWiper();
      } else if (channel == 1) {
        digiPot_fine.setWiper(value);
        valueRead = digiPot_fine.getWiper();
      }

      if (valueRead != value) {
        Serial.print("Error, unable to update PWM! Digipot Channel:" + String(channel));
        Serial.println(", Value:" + String(value) + ", valueRead:" + String(valueRead));
        return;
      } else {
        Serial.println("Digipot Channel: " + String(channel) + ", PWM val: " + String(valueRead));
      }
    }

    void writeDigipots(String argString, int unitsRead) {
      units_D = unitsRead;
      argString.trim();
      if (argString.equals("") || argString.indexOf(':') == -1)
      {
        return;
      }

      int commaIndex = argString.indexOf(',');

      String substring_coarse = argString.substring(0, commaIndex);
      substring_coarse.trim();

      String substring_fine = argString.substring(commaIndex + 1);
      substring_fine.trim();

      ////// coarse
      int colonIndex = substring_coarse.indexOf(':');

      String channel_string = substring_coarse.substring(0, colonIndex);
      channel_string.trim();
      int channel_coarse = channel_string.toInt();

      String value_string = substring_coarse.substring(colonIndex + 1);
      value_string.trim();
      int value_coarse = value_string.toInt();

      ////// fine
      colonIndex = substring_fine.indexOf(':');

      channel_string = substring_fine.substring(0, colonIndex);
      channel_string.trim();
      int channel_fine = channel_string.toInt();

      value_string = substring_fine.substring(colonIndex + 1);
      value_string.trim();
      int value_fine = value_string.toInt();

      setDigipot(channel_coarse, value_coarse);
      setDigipot(channel_fine, value_fine);
    }

};
