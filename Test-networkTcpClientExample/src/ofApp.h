#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxJSON.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(ofKeyEventArgs & key);
		void exit();

		void keyReleased(int key) {};
		void mouseMoved(int x, int y) {};
		void mouseDragged(int x, int y, int button) {};
		void mousePressed(int x, int y, int button) {};
		void mouseReleased(int x, int y, int button) {};
		void mouseEntered(int x, int y) {};
		void mouseExited(int x, int y) {};
		void windowResized(int w, int h) {};
		void dragEvent(ofDragInfo dragInfo) {};
		void gotMessage(ofMessage msg) {};

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

