/*************************************************** 
  This is firmware code for Adafruit 24-channel PWM/LED driver
  An Adaptation of test code by Limor Fried/Ladyada (Adafruit Industries)

  This driver uses SPI to communicate, 3 pins are required to  
  interface: Data, Clock and Latch. The boards are chainable

  Datasheet: http://www.adafruit.com/datasheets/tlc5947.pdf
  Library: https://github.com/adafruit/Adafruit_TLC5947
  Product Page: https://www.adafruit.com/product/1429

  Written by Hamza Mohammed

  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_TLC5947.h"
#include "MCP45HVX1.h"
#include digipots.h


// TODO: Store maxCurrent value in flash
// #include <avr/pgmspace.h>
// const PROGMEM float maxCurrent_store[] = {maxCurrent_hardcode};
// const char *const string_table[] PROGMEM = {string_0};
// char buffer[30];

// How many boards do you have chained?
#define NUM_TLC5974 1

#define data   4
#define clock   5
#define latch   6
#define oe   -1  // set to -1 to not use the enable pin (its optional)


// TODO, see if you can use #DEFINE, and advantage of this
// // Make sure this is correct
// // 60 mA/30 mA for 12/24 channel hardcoded limit, based on datasheet
// #define maxCurrent_hardcode maxCurrent
// #define numChannels_hardcode numChannels
// #define bitResolution 12
// uint32_t numSteps_val = pow(2, bitResolution);
// #define numSteps numSteps_val
// // #define numSteps pow(2, bitResolution)

//float maxCurrent_hardcode = 60.000; // 60 mA hardcoded limit, based on datasheet for 12 channel
float maxCurrent_hardcode = 30.000; // 30 mA hardcoded limit, based on datasheet for 24 channel

// float maxVoltage_hardcode = 10.000; // 10v hardcoded limit
// uint16_t numDigipots_hardcode = 2;
// uint32_t bitResolution_D = 8;
// uint32_t numSteps_D = pow(2, bitResolution_D);

uint16_t numChannels_hardcode = 24 * NUM_TLC5974;
uint32_t bitResolution = 12;
uint32_t numSteps = pow(2, bitResolution);

String incomingString;
//float maxCurrent = maxCurrent_hardcode; // in mA
float maxCurrent; // in mA
// float maxVolt; // in v

uint16_t numChannels = numChannels_hardcode;
// uint16_t numDigipots = numDigipots_hardcode;

// float currentStep;

// Added for speedup in PWM updates
int semicolonIndex = -1;
// int semicolonIndex_D = -1;

// if units = 0, outputs raw PWM values (uint16_t)
// if units = 1, outputs values in mA (float)
// if units = 2, outputs values in frac (float)
uint8_t units;
// uint8_t units_D;

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, clock, data, latch);

MCP45HVX1 digiPot_course(60);
MCP45HVX1 digiPot_fine(61);
Digipots_MCP45HVX1 digipots = Digipots_MCP45HVX1(digiPot_course, digiPot_fine);

void setup() {
  Serial.begin(9600);
  
  Serial.println("RESET");
  tlc.begin();
  if (oe >= 0) {
    pinMode(oe, OUTPUT);
    digitalWrite(oe, LOW);
  }

  // ADD Improved test case?

  incomingString = "";
  units = 0;
  semicolonIndex = -1;

//   maxCurrent = maxCurrent_hardcode;
  numChannels = numChannels_hardcode;

  // currentStep = maxCurrent / float(numSteps);

  // Reset all channels to 0
  for (uint16_t i = 0; i < numChannels; ++i)
  {
    tlc.setPWM(i, 0);
    tlc.write();
  }
  
  Serial.println("Please enter MAX current value or hit [Enter] if you want to keep existing value.");
  Serial.println("Or type \"hardcode\" if you want to use hardcoded value.");
  while(1==1) {
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      if(input.equals("")){
        if (maxCurrent == NULL || maxCurrent == 0) {
          maxCurrent = maxCurrent_hardcode;
        }
        break;
      }
      
      if (input.equals("hardcode")) {
        maxCurrent = maxCurrent_hardcode;
        break;
      }
      set_maxCurrent(input);
      if (maxCurrent == 0) {
        maxCurrent = maxCurrent_hardcode;
      }
      break;
    }
  }
  Serial.println("maxCurrent = " + String(maxCurrent));
  
  readChannels(0);

  // Initializing digipots
  digipots.initialize_digipots();
}

void loop() {
  while(1==1) {
    if (Serial.available() > 0) {
      incomingString = Serial.readStringUntil('\n');
      incomingString.trim();
      if(incomingString.equals("")){
        readChannels(0);
        continue;
      }

      semicolonIndex = incomingString.indexOf(';');

      if(semicolonIndex == -1){
        Serial.println("No command prefix given!");
      } else {
        // command prefix given
        String command = incomingString.substring(0, semicolonIndex);
        String args = "";
        if (incomingString.length() > semicolonIndex + 1) {
          args = incomingString.substring(semicolonIndex + 1);
        }
       // int out = executeCommand(command.trim(), args.trim());
       // executeCommand(command.trim(), args.trim());

        if(command.equals("") || command.equals("read_pwm") || command.equals("read")){
          readChannels(0);
          continue;
        }
        if(command.equals("read_mA")){
          readChannels(1);
          continue;
        }
        if(command.equals("read_f")){
          readChannels(2);
          continue;
        }
        // Read Commands for Digipots
        if(command.equals("read_D_pwm") || command.equals("read_D")){
          digipots.readDigipots(0);
          continue;
        }
        if(command.equals("read_D_v")){
          digipots.readDigipots(1);
          continue;
        }
        if(command.equals("read_D_f")){
          digipots.readDigipots(2);
          continue;
        }
      
        // Write commands
        if(command.equals("write") || command.equals("write_pwm")){
          units = 0;
          writeChannels(args);
          continue;
        }
        if(command.equals("write_mA")){
          units = 1;
          writeChannels(args);
          continue;
        }
        if(command.equals("write_f")){
          units = 2;
          writeChannels(args);
          continue;
        }
        // Write commands for Digipots
        if(command.equals("write_D") || command.equals("write_D_pwm")){
          // units_D = 0;
          digipots.writeDigipots(args, 0);
          continue;
        }
        if(command.equals("write_D_v")){
          // units_D = 1;
          digipots.writeDigipots(args, 1);
          continue;
        }
        if(command.equals("write_D_f")){
          // units_D = 2;
          digipots.writeDigipots(args, 2);
          continue;
        }
      
        // Configuration Commands
        if(command.equals("reset")){
          resetValues();
          continue;
        }
        if(command.equals("reset_D")){
          digipots.resetDigipots();
          continue;
        }
        if(command.equals("dimmer")){
          dimmerloop();
          continue;
        }
        if(command.equals("maxCurrent")){
          set_maxCurrent(args);
          continue;
        }
        if(command.equals("maxVoltage")){
          digipots.set_maxVoltage(args);
          continue;
        }
        if(command.equals("numChannels")){
          set_numChannels(args);
          continue;
        } else {
          Serial.println("Invalid command given:" + String(command));
          continue;
        }

      }
    }
  }
}

void resetValues() {
  if (oe >= 0) {
    digitalWrite(oe, HIGH);
    // Serial.println(digitalRead(oe));
    readChannels(0);
    delay(200);
    digitalWrite(oe, LOW);
    // Serial.println(digitalRead(oe));
  } else {
    // Reset all channels to 0
    for (uint16_t i = 0; i < numChannels; ++i)
    {
      tlc.setPWM(i, 0);
      tlc.write();
    }
  }
  
  readChannels(0);
}

void set_maxCurrent(String argString) {
  float current_new = argString.toFloat();

  if (current_new <= maxCurrent_hardcode && current_new > 0) {
    maxCurrent = current_new;
    Serial.println("CONFIG UPDATE: Max Current = " + String(maxCurrent));
  } else {
    Serial.println("Error, current out of range! Input Current:" + String(current_new));
  }

}

// void set_maxVoltage(String argString) {
//   float volt_new = argString.toFloat();

//   if (volt_new <= maxVoltage_hardcode && volt_new > 0) {
//     maxVolt = volt_new;
//     Serial.println("CONFIG UPDATE: Max Voltage = " + String(maxVolt));
//   } else {
//     Serial.println("Error, voltage out of range! Input Current:" + String(volt_new));
//   }
// }

void set_numChannels(String argString) {
  int numChannels_new = argString.toInt();

  if (numChannels_new <= numChannels_hardcode && numChannels_new > 0) {
    numChannels = (uint16_t) numChannels_new;
    Serial.println("CONFIG UPDATE: numChannels = " + String(numChannels));
  } else {
    Serial.println("Error, numChannels out of range! Input numChannels:" + String(numChannels_new));
  }

}

// if unitsRead = 0, outputs raw PWM values (uint16_t)
// if unitsRead = 1, outputs values in mA (float)
// if unitsRead = 2, outputs values in frac (float)
void readChannels(int unitsRead) {
  String unit_suffix = "";
  for(uint16_t i=0; i < numChannels; i++){
    uint16_t valueRead = tlc.getPWM(i);
    float value;
    switch (unitsRead) {
        case 0:
          unit_suffix = "";
          value = valueRead;
          break;
        case 1:
          unit_suffix = "mA";
          value = (float(valueRead) / float(numSteps - 1)) * maxCurrent;
          break;
        case 2:
          unit_suffix = "%";
          value = float(valueRead) / float(numSteps - 1);
          break;
        default:
          Serial.println("Unknown units value! 0=pwm, 1=mA, 2=frac; Units:" + String(unitsRead));
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

void writeChannels(String argString) {
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
  switch (units) {
      case 0:
        value = value_string.toInt();
        if (value >= numSteps || value < 0) {
          Serial.println("Error, PWM value out of range! Value:" + String(value));
          return;
        }
        break;
      case 1:
        valueFloat = value_string.toFloat();
        if (valueFloat > maxCurrent || valueFloat < 0) {
          Serial.println("Error, PWM (mA) value out of range! Value:" + String(valueFloat));
          return;
        }
        value = int((valueFloat / maxCurrent) * float(numSteps - 1));
        break;
      case 2:
        valueFloat = value_string.toFloat();
        if (valueFloat > 1.0 || valueFloat < 0) {
          Serial.println("Error, PWM (frac) value out of range! Value:" + String(valueFloat));
          return;
        }
        value = int(valueFloat * float(numSteps -1));
        break;
      default:
        Serial.println("Error, invalid units! Units:" + String(units));
        return;
  }

  if (channel == -1) {
    for (uint16_t i = 0; i < numChannels; ++i) {
      setChannel(i, value);
    }
  } else {
    setChannel(channel, value);
  }
}
 
// sets channel, values are 0 indexed. i.e. for TLC5947, channel indices are from 0 to 23 
void setChannel(int channel, int value) {
  if(channel < 0 || channel >= numChannels) {
    Serial.println("Error, channel value not in range! Channel:" + String(channel));
    return;
  }
  
  // make sure value in range
  value = (value > numSteps - 1) ? (numSteps - 1) : value;
  value = (value < 0) ? 0 : value;

  // set PWM value of channel
  tlc.setPWM((uint16_t) channel, (uint16_t) value);
  tlc.write();

  // read PWM value of channel
  uint16_t valueRead = tlc.getPWM((uint16_t) channel);

  if(valueRead != value){
    Serial.print("Error, unable to update PWM! Channel:" + String(channel));
    Serial.println(", Value:" + String(value) + ", valueRead:" + String(valueRead));
    return;
  } else {
    Serial.println("Channel: " + String(channel) + ", PWM val: " + String(valueRead));
  }
}

void dimmerloop(){
  for (uint16_t i = 0; i < 4096/100; ++i) {
      for (uint16_t j = 0; j < numChannels; ++j) {
        setChannel(j, i*100);
      }
      delay(100);
  }
}
