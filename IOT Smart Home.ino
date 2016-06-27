#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <math.h>
#include <MemoryFree.h>

#include "thingspeak.h"

extern Adafruit_CC3000 cc3000;
extern void wifi_init();

#define LED 			 	7
#define LED_R 				A2
#define LED_G 				9
#define LED_B 				6
#define BUFSIZE     		128
#define PWM 				255
#define BTTN_ONOFF  		2
#define LED_STATUS_ON		"On"
#define LED_STATUS_OFF		"Off"
#define LDR					A0
#define TEMPERATURE			A1
#define BUZZER				A3

int fadeAmount = 		5;    // how many points to fade the LED by
int brightness = 		0; // how bright the LED is
volatile int ledState = 0;
// variable to hold sensor value
long sensorValue;
// variable to calibrate low value
int sensorLow = 1023;
// variable to calibrate high value
int sensorHigh = 0;
float temperature;

uint32_t ip;

void cloudWifiSetup();
void peripheralSetup();
void fadeOff(int );
void lightUpLed( int br);
void turnOnOffLed (int state);
void checkButtonOnOFF (void);
void fadeOn (int);
void calibrateLDR(void);
char* updateLedStatus(int);
float tempControl (void);

/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/

// A small Flash print helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void calibrateLDR(void){
	
	// calibrate for the first six seconds after program runs
	Serial.println(F("Please Caliberate for 6 seconds now ..."));
  while (millis() < 6000) {
    // record the maximum sensor value
    sensorValue = analogRead(LDR);
    if (sensorValue > sensorHigh) {
      sensorHigh = sensorValue;
    }
    // record the minimum sensor value
    if (sensorValue < sensorLow) {
      sensorLow = sensorValue;
    }
  }
  		Serial.print("Low Threshold: ");
  		Serial.println(sensorLow);
  		Serial.print("High Threshold: ");
		Serial.println(sensorHigh);
		digitalWrite(LED, LOW);
}

void setup(void){
  	Serial.begin(115200);
  	Serial.print("Free RAM: "); Serial.println(freeMemory(), DEC);
  	peripheralSetup();
  	calibrateLDR();
	cloudWifiSetup();
	
	// Interrupt call
		attachInterrupt(0, checkButtonOnOFF, RISING);
}

void peripheralSetup(){
		pinMode(BTTN_ONOFF, INPUT);
		pinMode(LED, OUTPUT);
		pinMode (LDR, INPUT);
		pinMode (TEMPERATURE, INPUT);
		pinMode(LED_R, OUTPUT);
		pinMode(LED_G, OUTPUT);
		pinMode(LED_B, OUTPUT);
}

void cloudWifiSetup(){
	wifi_init();
	ThingSpeak.init(&cc3000, "api.thingspeak.com", 80, "V2YG4Z7MUTN69QQR", 99922, "5V0A6OCWICLAPHT8", 7791);
	//ThingSpeak.channelInit();
}

void loop(void){
	int ldr = ldrControl();
	Serial.println(ldr);
	
	ThingSpeak.channelInit();
  	ThingSpeak.channelSetField("field1", ldr);
  	ThingSpeak.channelSetField("field2", tempControl ());
  	ThingSpeak.channelSetField("field4", ledState);
	ThingSpeak.channelUpdate();
	
	char *cmd=ThingSpeak.commandExecute();
	
	if (strlen(cmd)==0) {
		Serial.println(F("No command to execute"));
	} 
	else {
		Serial.print(F("Got command to execute: ")); 
		Serial.println(cmd);
		
		if ((strcmp(cmd, "on")== 0)){
			ledState = 1;
		}
		else if((strcmp(cmd, "off")== 0)){
			ledState = 0;
		}
		turnOnOffLed(ledState);
	}
}

void checkButtonOnOFF (void){
	static long lastTime = 0; 
	if (millis() - lastTime < 200) return; 
	lastTime = millis(); 
	ledState = ! ledState; 
	turnOnOffLed(ledState);
}

void turnOnOffLed (int state){
	ThingSpeak.channelInit();
	if (state == 1){
		digitalWrite(LED, state);
	}
	else {
		digitalWrite(LED, state);
	}
}

long ldrControl(void){
	static int temp = ledState;
  sensorValue = analogRead(LDR);
  if ( sensorValue <=sensorLow){
  	ledState = 1;
	}
  if (sensorValue >= sensorHigh){
	ledState =0;
	}
	turnOnOffLed(ledState);
	
  if (temp != ledState){
	ThingSpeak.channelSetField("field4", ledState);
	ThingSpeak.channelSetField("field3", updateLedStatus(ledState));
	ThingSpeak.channelUpdate();
	}
	temp=ledState;
	return sensorValue;
}

float tempControl (void){
	
	//read the input from A1 and store it in a variable
  int currentTemp = analogRead(TEMPERATURE);
  temperature =  (currentTemp * 0.702) + 32;
  Serial.print(temperature);
  Serial.println(" F");
  
  if (temperature > 67){
  	for(int i =0; i<= 6; i++){
  		digitalWrite(LED_G, HIGH);
  		delay (100);
  		digitalWrite(LED_G, LOW);
  		delay (100);
  		digitalWrite(LED_B, HIGH);
  		delay (100);
  		digitalWrite(LED_B, LOW);
		tone(BUZZER, 100, 1000);
		delay (500);
  	}
  }
  return temperature;
}

char* updateLedStatus(int state){
	if (state ==1){
		return "On";
	}
	else{
		return "Off";
	}
}

