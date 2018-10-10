#ifndef __TcpClientManager__
#define __TcpClientManager__

#include <stdio.h>
#include "ofMain.h"

#include "ofxNetwork.h"
#include "ofxJSON.h"
#include "ofxGui.h"


#define RECONNECT_TIME 4000

class TcpClientManager : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);
		void exit();

		bool isSetup = false;
	//======================== Network ==========================
		ofxTCPClient tcpClient;
		string msgTx, msgRx;

		ofTrueTypeFont  mono;
		ofTrueTypeFont  monosm;

		float counter;
		int connectTime;
		int deltaTime;

		int size;

		ofParameter<string> ip;
		ofParameter<string> port;
		string prevIp;
		string prevPort;

		string delimiter;

		vector<string> videoCommandsName;
		vector<string> videoCommandsString;

		void sendTCPMessage(string s);

	//======================== JSON =============================
		ofxJSONElement settings;
		void saveSettings();
		void loadSettings();

	//======================== Gui =========================
		vector<ofxButton>	videoCommandBtn;
		ofxButton playBtn;
		ofxButton prevBtn;
		ofxButton nextBtn;
		ofxButton stopBtn;
		ofxButton volumeVal;
		ofxButton volumeUp;
		ofxButton volumeDn;

		ofxLabel ipInput;
		ofxLabel status;

		string tcpMessageToSend;

		ofxPanel tcpGui;

		void playButtonPressed();
		void prevButtonPressed();
		void nextButtonPressed();
		void stopButtonPressed();
		void volumeUpButtonPressed();
		void volumeDnButtonPressed();

};

#endif