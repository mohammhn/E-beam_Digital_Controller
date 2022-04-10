# E-beam_Digital_Controller
 
## Setup Instructions:

 1. Install Arduino IDE from https://www.arduino.cc/en/software
 	- Refer to the Getting Started guide on the page for assistance on installation process

 2. Add the two Adafruit libraries:

 	Method A:
	 a. Copy the folders inside the libraries folder in this repository over to your Arduino libraries folder
	 	- Arduino libraries folder is generally located at:
	 		- Windows: User\Documents\Arduino\libraries\
	 		- Mac: User\Documents\Arduino\libraries\
	 		- Linux: home\user\Arduino\sketchbook\libraries\

	 Method B:
	 b. Go to Tools->Manage Libraries. A dialog box will open up. Type in and install libraries Adafruit TLC5947, and Adafruit TLC59711
 3. Restart Arduino IDE if already open, and verify the two Adafruit libraries are shown under File->Examples menu
 4. Open the firmware code in Arduino IDE, and click verify to make sure it compiles.

 Optional: 	to work on alternative IDE (e.g. VScode or Sublime) see: 
		 	a. https://maker.pro/arduino/tutorial/how-to-use-visual-studio-code-for-arduino 
		 	b. https://www.crazyengineers.com/threads/use-sublime-text-3-editor-as-arduino-ide-to-edit-upload-arduino-sketch-into-arduinos.71159


# Commands for Current Driver (ends with semicolon):

	read; 
		reads and outputs raw pwm values for all channels to Serial

	read_pwm;
		reads and outputs raw pwm values for all channels to Serial

	read_mA;
		reads pwm values and converts to mA and outputs for all channels to Serial

	read_f;
		reads pwm values and converts to fraction value and outputs for all channels to Serial


	write;
		writes input values (in raw pwm) to respective channels.

	write_pwm;
		writes input values (in raw pwm) to respective channels.

	write_mA;
		writes input values (float value in mA) to respective channels.

	write_f;
		writes input values (fractional float value) to respective channels.

	reset;
		sets all channel values to 0;

	maxCurrent;
		takes input current value as a float (in mA), and sets maxCurrent to it (refers to the maximum current output per channel)

	numChannels;
		takes input int value, and set numChannels to it (refers to the number of channels on the board to control)

# Example Inputs for Current Driver:

## read examples:
	read;
	read_f;
	read_mA;

## write examples:
### max current on all channels:
	write; all:4095
	write_pwm; all:4095
	write_mA; all:60
	write_f; all:1

	or

	write; 0:4095, 1:4095, ... , 22:4095, 23:4095

#### Note: you can use the "all" channel selector along with individual channels in the same command as well. 
	E.g. to set max current on all channels except channel 5:
	write; all:4095, 5:0

### half current on channels 3-6:
	write; 3:2047, 4:2047, 5:2047,6: 2047 
	write_pwm; 3:2047, 4:2047, 5:2047, 6:2047 
	write_mA; 3:30, 4:30, 5:30, 6:30 
	write_f; 3:0.5, 4:0.5, 5:0.5, 6:0.5

### reset values to 0 on all channels:
	reset;
	write; all:0
	(etc.)

## Some config update examples:
### changing max current value to 5.5 mA:
	maxCurrent; 5.5

### changing number of channels to 48:
	numChannels; 48

### Note: spacing between punctuation is ignored automatically as Strings and substrings are trimmed before parsing.



# Commands for Voltage Driver (Digipots) (ends with semicolon):

	read_D; 
		reads and outputs raw pwm values for both digipots

	read_D_pwm;
		reads and outputs raw pwm values for both digipots

	read_D_v;
		reads pwm values and converts to voltage and outputs for both digipots

	read_D_f;
		reads pwm values and converts to fraction value and outputs for both digipots


	write_D;
		writes input values (in raw pwm) to respective channels. *channel 0 is digipot course, 1 is digipot fine

	write_D_pwm;
		writes input values (in raw pwm) to respective channels.

	write_D_v;
		writes input values (float value in mA) to respective channels.

	write_D_f;
		writes input values (fractional float value) to respective channels.

	reset_D;
		sets both digipot wipers to 0;

	maxVoltage;
		takes input current value as a float (in v), and sets maxVoltage to it (refers to the maximum voltage output for digipots)


