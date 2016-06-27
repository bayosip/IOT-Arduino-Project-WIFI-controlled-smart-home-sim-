#include "thingspeak.h"
#include "http-request.h"

// forward declaration of helper functions
void getFullResponse(Adafruit_CC3000_Client *client);
int getResponseCode(char *response);
char* getContent(char *response);		// return prointer to body inside response

// define the global object first
class ThingSpeak ThingSpeak;

#define 	BUF_LENGTH	512
char buf[BUF_LENGTH];	// for receiving response

int ThingSpeak::init(Adafruit_CC3000 * cc3000, const char * host, const int port, const char *channelApiKey, const int channelId, const char *talkbackApiKey, const int talkbackId)
{
	_cc3000 = cc3000;
	_host = host;
	_port = port;
	
	_channelApiKey = channelApiKey;
	_channelId = channelId;

	_talkbackApiKey = talkbackApiKey;
	_talkbackId = talkbackId;

	// Try looking up the website's IP address
  	_ip = 0;
  	Serial.print(F("Resolving host : "));Serial.print(_host); Serial.print(F(" -> "));
  	while (_ip == 0) {
    	if (! _cc3000->getHostByName((char*)_host, &_ip)) {
      		Serial.println(F("Couldn't resolve!"));
    	}
    	delay(500);
  	}
  	_cc3000->printIPdotsRev(_ip);
  	Serial.println();
  	
  	return 0;
}

void ThingSpeak::channelInit()
{
	buf[0]=0;
}

int ThingSpeak::channelSetField(const char *field, const char *value)
{
	if (strlen(buf) + strlen(field) + strlen(value) + 2 > BUF_LENGTH) {
		Serial.println(F("ERROR: buffer overflow in channelSetField()"));
		for(;;);
	}
	
	strcat(buf, "&");
	strcat(buf,field);
	strcat(buf, "=");
	strcat(buf, value);

	return strlen(buf);
}

int ThingSpeak::channelSetField(const char *field, int value)
{
	char b[10];
	return channelSetField(field, itoa(value, b, 10));
}

int ThingSpeak::channelSetField(const char *field, long value)
{
	char b[10];
	return channelSetField(field, ltoa(value, b, 10));
}

int ThingSpeak::channelSetField(const char *field, double value)
{
	char temp[20];
	return channelSetField(field, dtostrf(value, 1, 3, temp));
}


int ThingSpeak::channelUpdate()
{
	char temp[10];
	int contentLength=strlen(buf);
	static long lastTime = 0;
	
	Serial.println(F("Updating channel ...")); Serial.println(buf);

	/* Try connecting to the website */
  	Adafruit_CC3000_Client www = _cc3000->connectTCP(_ip, _port);
  	for(int i=0; !www.connected() && i < 1000; i++) { 
  		www = _cc3000->connectTCP(_ip, 80);
  	}  // this seems to be a bug of driver. It needs to connect twice 
  	if (!www.connected()) {
  		Serial.println(F("Connection failed"));
  		return NULL;
  	}

	/*
    Serial.print(F(CMD_UPDATE_1));
    Serial.print(_channelApiKey);
    Serial.println(buf);
    Serial.print(F(CMD_UPDATE_2));
	*/
	
    www.fastrprint(F(CMD_UPDATE_1));
    www.fastrprint(_channelApiKey);
    www.fastrprint(buf);
    www.fastrprint(F(CMD_UPDATE_2));

    getFullResponse(&www);
    
    int code= getResponseCode(buf);
	char * content=getContent(buf);
	
	Serial.print(F("Response code : "));
	Serial.println(code);
	
	Serial.print(F("Response : "));
	Serial.println(content);
	
	www.close();
	Serial.print((millis()-lastTime)/1000);
	Serial.println("Sec");
	lastTime = millis();
	return 0;
}

char* ThingSpeak::commandExecute() {
	static long lastTime = 0;
	Serial.println(F("Fetch next command ..."));
	
	/* Try connecting to the website */
  	Adafruit_CC3000_Client www = _cc3000->connectTCP(_ip, _port);
  	for(int i=0; !www.connected() && i < 1000; i++) { 
  		www = _cc3000->connectTCP(_ip, 80);
  	}  // this seems to be a bug of driver. It needs to connect twice 
  	if (!www.connected()) {
  		Serial.println(F("Connection failed"));
  		return NULL;
  	}

    www.fastrprint(F(CMD_EXECUTE_1));
    www.fastrprint(itoa(_talkbackId, buf, 10));
    www.fastrprint(F(CMD_EXECUTE_2));
    www.fastrprint(_talkbackApiKey);
    www.fastrprint(F("\n\n"));

	getFullResponse(&www);

	int code= getResponseCode(buf);
	char * content=getContent(buf);
	
	Serial.print(F("Response code : "));
	Serial.println(code);
	
	Serial.print(F("Response : "));
	Serial.println(content);
	
	www.close();
	Serial.print((millis()-lastTime)/1000);
	Serial.println("Sec");
	lastTime = millis();
	return content;
}


// ============ helpers ==============
void getFullResponse(Adafruit_CC3000_Client *client) 
{
	char c;
	int i=0;
    while(client->connected() && !client->available()) delay(1);
    while (client->available()) {
        c = client->read();
        // Serial.print(c);
        buf[i++] = c;
        if (i==BUF_LENGTH) {
        	Serial.println(F("buffer overflow in getFullResponse()"));
        	for(;;);
        }
    }
    buf[i]=0;
}

int getResponseCode(char *buf) {
   	return atoi(buf+9);
}

char* getContent(char *buf) {
	char *p=strstr(buf,"\r\n\r\n");
	if (p) {
		return p+4;
	} else {
		return "";
	}
}
