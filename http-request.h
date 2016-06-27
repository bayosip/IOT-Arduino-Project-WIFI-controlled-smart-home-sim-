/*
A command execute request:

GET /talkbacks/4033/commands/execute?headers=false HTTP/1.0
Host: api.thingspeak.com
X-THINGSPEAKAPIKEY: TGBUQ5BNI0JEK9R9

We will break it into 2 parts
*/
#define CMD_EXECUTE_1 \
	"GET /talkbacks/"
	
#define CMD_EXECUTE_2 \
	"/commands/execute?headers=false HTTP/1.0\nHost: api.thingspeak.com\nX-THINGSPEAKAPIKEY: "
	

/*
  GET command to update channel:

GET /update?headers=false&key=7SJS0VOUUMUJSESA&field1=555 HTTP/1.0
Host: api.thingspeak.com

We will break header into two parts as well

Note: the POST version does not work as the doc page described!!!
*/
#define CMD_UPDATE_1 \
	"GET /update?headers=false&key="
	
#define CMD_UPDATE_2 \
	" HTTP/1.0\nHost: api.thingspeak.com\n\n"