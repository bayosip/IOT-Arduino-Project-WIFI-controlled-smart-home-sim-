#ifndef _thingspeak_h_
#define _thingspeak_h_

#include <Arduino.h>
#include <Adafruit_CC3000.h>

class ThingSpeak {
	public:
		int init(Adafruit_CC3000 * cc3000, const char * host, const int port, const char *channelApiKey, const int channelId, const char *talkbackApiKey, const int talkbackId);

		void channelInit();
		int channelSetField(const char *field, int value);
		int channelSetField(const char *field, long value);
		int channelSetField(const char *field, double value);
		int channelSetField(const char *field, const char * value);
		int channelSetField(const char *field, String value);
		int channelUpdate();
		
		char* commandExecute();		// return command string in *cmd
		
	private:
		Adafruit_CC3000 * _cc3000;
		const char * _host;
		int _port;
		uint32_t _ip;
		
		const char * _channelApiKey;
		int _channelId;
		
		const char * _talkbackApiKey;
		int _talkbackId;
};

extern ThingSpeak ThingSpeak;

#endif