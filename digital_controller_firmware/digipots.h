/*************************************************** 
  This is firmware code for Digipots MAX45HVX1
 ****************************************************/

#include <Arduino.h>
#include "MCP45HVX1.h"

#define POT1_SW 2
#define POT2_SW 3
#define PWR_ON 4
#define Vcont A1
#define Pot_out_1 A0

class Digipots_MCP45HVX1 {
  public:
    MCP45HVX1 digiPot_course;
    MCP45HVX1 digiPot_fine;
    float maxVoltage_hardcode = 10.000; // 10v hardcoded limit
    uint16_t numDigipots_hardcode = 2;
    uint32_t bitResolution_D = 8;
    uint32_t numSteps_D = pow(2, bitResolution_D);
    uint16_t numDigipots = numDigipots_hardcode;
    
    int units_D = 0;
    // int semicolonIndex_D = -1;
    float maxVolt = maxVoltage_hardcode;
    

    Digipots_MCP45HVX1(MCP45HVX1 digiPot_c, MCP45HVX1 digiPot_f) {
      digiPot_course = digiPot_c;
      digiPot_fine = digiPot_f;
      maxVolt = maxVoltage_hardcode;
      units_D = 0;
      // semicolonIndex_D = -1;
    }

    void initialize_digipots() {
      maxVolt = maxVoltage_hardcode;
      units_D = 0;
      /* Turn on digital pins.......................... */
      pinMode(POT1_SW, OUTPUT);
      digitalWrite(POT1_SW, HIGH);

      pinMode(POT2_SW, OUTPUT);
      digitalWrite(POT2_SW, HIGH);

      //  pinMode(PWR_ON, OUTPUT);
      //  digitalWrite(PWR_ON, HIGH);

      /* ............................................... */
      // Initializing Digipot Course
      Serial.println("Initializing Digipot Fine");
      digiPot_course.begin();
      digiPot_course.startup();
      Serial.print("startup: ");
      Serial.println(digiPot_course.readTCON());
      digiPot_course.startup();

      digiPot_course.defaultTCON();
      Serial.print("default: ");
      Serial.println(digiPot_course.readTCON());

      /* ............................................... */
      // Initializing Digipot Fine
      Serial.println("Initializing Digipot Course");
      digiPot_fine.begin();
      digiPot_fine.startup();
      Serial.print("startup: ");
      Serial.println(digiPot_fine.readTCON());
      digiPot_fine.startup();

      digiPot_fine.defaultTCON();
      Serial.print("default: ");
      Serial.println(digiPot_fine.readTCON());
      Serial.println("Initialization of Digipot Course Complete");
    }

    void resetDigipots() {
      digiPot_fine.writeWiper(0);
      digiPot_course.writeWiper(0);
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
    // if unitsRead = 1, outputs values in v (float)
    // if unitsRead = 2, outputs values in frac (float)
    void readDigipots(int unitsRead) {
      uint16_t value_fine = digiPot_fine.readWiper();
      uint16_t value_course = digiPot_fine.readWiper();
      uint16_t values[2] = {value_fine, value_course}

      String unit_suffix = "";
      for(uint16_t i=0; i < numDigipots; i++){
        uint16_t valueRead = values[i];
        float value;
        switch (unitsRead) {
            case 0:
              unit_suffix = "";
              value = valueRead;
              break;
            case 1:
              unit_suffix = "v";
              value = (float(valueRead) / float(numSteps - 1)) * maxCurrent;
              break;
            case 2:
              unit_suffix = "%";
              value = float(valueRead) / float(numSteps - 1);
              break;
            default:
              Serial.println("Unknown units value! 0=pwm, 1=v, 2=frac; Units:" + String(unitsRead));
              return;
        }
        //outputting each channel value per line, /n delimited
        Serial.println(String(i) + ":" + String(value) + unit_suffix);

        // outputting channel values in single line, comma delimited
        /*
        if (i == numChannels - 1) {
          Serial.println(String(i) + ":" + String(value) + unit_suffix);
        } else {
          Serial.print(String(i) + ":" + String(value) + unit_suffix + ",");
        }
        */
        
      }
    }

    void writeDigipots(String argString, int unitsRead) {
      units_D = unitsRead;
      argString.trim();
      if (argString.equals("") || argString.indexOf(':') == -1) 
      {
        Serial.println("Incorrect PWM input, no colons! Input:" + argString);
        // return -1;
        return;     
      }

      int commaIndex = argString.indexOf(',');
      int commaIndex_prev = 0;

      while (1==1) {
        if(commaIndex == -1){
          String substring = argString.substring(commaIndex_prev);
          substring.trim();
          if(!substring.equals("")){
            parseColonSubstring(substring);
          }
          break;
        }

        String substring = argString.substring(commaIndex_prev, commaIndex);
        substring.trim();
        if(!substring.equals("")){
          parseColonSubstring(substring);
        }

        // commaIndex_prev = ++commaIndex;
        // commaIndex = argString.indexOf(',', commaIndex);
        if(argString.length() > commaIndex + 1){
          commaIndex_prev = commaIndex + 1;
          commaIndex = argString.indexOf(',', commaIndex + 1);
        } else {
          commaIndex = -1;
          commaIndex_prev = 0;
          break;
        }
        
      }
      return;
    }

    void parseColonSubstring (String substring) {
      int colonIndex = substring.indexOf(':');
      if(colonIndex == -1){
        Serial.println("Error, no colon! Substring:" + substring);
        return;
      }

      String channel_string = substring.substring(0, colonIndex);
      channel_string.trim();
      int channel;

      // to change value for all channels
      if (channel_string.equals("all")) {
        channel = -1;
      } else {
        channel = channel_string.toInt();
      
        // to catch toInt() errors which return 0
        if (channel == 0 && !(String(channel).equals(channel_string))) {
          Serial.println("Error, incorrect channel input:" + channel_string);
          return;
        }
      }

      

      if(substring.length() <= colonIndex + 1){
        Serial.println("Error, no value given! Substring:" + substring);
        return;
      }

      String value_string = substring.substring(colonIndex + 1);
      int value;
      float valueFloat;
      switch (units_D) {
          case 0:
            value = value_string.toInt();
            if (value >= numSteps_D || value < 0) {
              Serial.println("Error, PWM value out of range! Value:" + String(value));
              return;
            }
            break;
          case 1:
            valueFloat = value_string.toFloat();
            if (valueFloat > maxVolt || valueFloat < 0) {
              Serial.println("Error, PWM (v) value out of range! Value:" + String(valueFloat));
              return;
            }
            value = int((valueFloat / maxVolt) * float(numSteps_D - 1));
            break;
          case 2:
            valueFloat = value_string.toFloat();
            if (valueFloat > 1.0 || valueFloat < 0) {
              Serial.println("Error, PWM (frac) value out of range! Value:" + String(valueFloat));
              return;
            }
            value = int(valueFloat * float(numSteps_D -1));
            break;
          default:
            Serial.println("Error, invalid units! Units:" + String(units_D));
            return;
      }

      if (channel == -1) {
        for (uint16_t i = 0; i < numDigipots; ++i) {
          setDigipot(i, value);
        }
      } else {
        setDigipot(channel, value);
      }
    }
    
    // sets Digipot, values are 0 indexed. i.e. 0 is digipot course, 1 is digipot fine
    void setDigipot(int channel, int value) {
      if(channel < 0 || channel >= numDigipots) {
        Serial.println("Error, channel value not in range! Channel:" + String(channel));
        return;
      }
      
      // make sure value in range
      value = (value > numSteps_D - 1) ? (numSteps_D - 1) : value;
      value = (value < 0) ? 0 : value;

      uint16_t valueRead = 0;
      // set PWM value of channel
      if (channel == 0) {
        digiPot_course.writeWiper(value);
        valueRead = digiPot_course.readWiper();
      } else if (channel == 1) {
        digiPot_fine.writeWiper(value);
        valueRead = digiPot_fine.readWiper();
      }

      if(valueRead != value){
        Serial.print("Error, unable to update PWM! Digipot Channel:" + String(channel));
        Serial.println(", Value:" + String(value) + ", valueRead:" + String(valueRead));
        return;
      } else {
        Serial.println("Digipot Channel: " + String(channel) + ", PWM val: " + String(valueRead));
      }
    }
}

