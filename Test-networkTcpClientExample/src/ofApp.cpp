#include "ofApp.h"

#define RECONNECT_TIME 400

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(230);

	//======================== Network ==========================

	// our send and recieve strings
	msgTx	= "";
	msgRx	= "";

	connectTime = 0;
	deltaTime = 0;

	//======================== JSON =============================
	loadSettings();

	//======================== GUI ==============================

	
	tcpGui.setup(); // most of the time you don't need a name
	
	//tcpGui.add(playBtn.setup(videoCommandsName[0]));
	tcpGui.add(playBtn.setup(videoCommandsName[1]));
	tcpGui.add(prevBtn.setup(videoCommandsName[2]));
	tcpGui.add(nextBtn.setup(videoCommandsName[3]));
	tcpGui.add(stopBtn.setup(videoCommandsName[6]));
	tcpGui.add(volumeUp.setup(videoCommandsName[8]));
	tcpGui.add(volumeDn.setup(videoCommandsName[9]));

	tcpGui.setPosition(ofGetWidth() / 2, 10);
}

//--------------------------------------------------------------
void ofApp::update(){

	//======================== Network ==========================
	if(tcpClient.isConnected()){
		// we are connected - lets try to receive from the server
		string str = tcpClient.receive();
		if( str.length() > 0 ){
			msgRx = str;
		}
	}else{
		msgTx = "";
		// if we are not connected lets try and reconnect every 5 seconds
		deltaTime = ofGetElapsedTimeMillis() - connectTime;

		if( deltaTime > 5000 ){
			tcpClient.setup(ip, port);
			tcpClient.setMessageDelimiter(delimiter);
			connectTime = ofGetElapsedTimeMillis();
		}

	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(20);
	ofDrawBitmapString("openFrameworks TCP Send Example", 15, 30);

	//======================== Network ==========================

	if(tcpClient.isConnected()){
		if(!msgTx.empty()){
			ofDrawBitmapString("sending:", 15, 55);
			ofDrawBitmapString(msgTx, 85, 55);
		}else{
			ofDrawBitmapString("status: type something to send data to port 11999", 15, 55);
		}
		ofDrawBitmapString("from server: \n" + msgRx, 15, 270);
	}else{
		ofDrawBitmapString("status: server not found. launch server app and check ports!\n\nreconnecting in "+ofToString( (5000 - deltaTime) / 1000 )+" seconds", 15, 55);
	}

	//======================== JSON =============================

	//======================== GUI ==============================
	tcpGui.draw();
}


//--------------------------------------------------------------
void ofApp::keyPressed(ofKeyEventArgs & key){
	// you can only type if you're connected
	// we accumulate 1 line of text and send every typed character
	// on the next character after a breakline we clear the buffer
	if(tcpClient.isConnected()){
		if(key.key == OF_KEY_BACKSPACE || key.key == OF_KEY_DEL){
			if( !msgTx.empty() ){
				msgTx = msgTx.substr(0, msgTx.size()-1);
			}
		}else if (key.codepoint != 0){
			ofUTF8Append(msgTx, key.codepoint);
		}
		tcpClient.send(msgTx);
		if (!msgTx.empty() && msgTx.back() == '\n') {
			msgTx.clear();
		}
	}
}

//======================== JSON =============================
void ofApp::saveSettings() {
	// now write pretty print
	if (!settings.save("settings.json", true))
	{
		ofLogNotice("ofApp::setup") << "example_output_pretty.json written unsuccessfully.";
	}
	else
	{
		ofLogNotice("ofApp::setup") << "example_output_pretty.json written successfully.";
	}
}

void ofApp::loadSettings() {

	std::string file = "settings.json";
	// Now parse the JSON
	bool parsingSuccessful = settings.open(file);

	if (parsingSuccessful)
	{
		ofLogNotice("ofApp::setup") << settings.getRawString();

		ip = settings["ip"].asString();
		port = settings["port"].asDouble();

		ofxTCPSettings IPsettings(ip, port);

		// set other options:
		//settings.blocking = false;
		//settings.messageDelimiter = "\n";

		// connect to the server - if this fails or disconnects
		// we'll check every few seconds to see if the server exists
		tcpClient.setup(IPsettings);

		// optionally set the delimiter to something else.  The delimiter in the client and the server have to be the same
		delimiter = settings["message-delimiter"].asString();
		tcpClient.setMessageDelimiter(delimiter);
		
		videoCommandsName.clear();
		videoCommandsString.clear();

		ofLog() << " settings['video-commands'].size(); "<< settings["video-commands"].size();

		for (int i = 0; i < settings["video-commands"].size(); i++) {
			videoCommandsName.push_back(settings["video-commands"][i]["name"].asString());
			videoCommandsString.push_back(settings["video-commands"][i]["string"].asString());
		}
	
	}
	else
	{
		ofLogError("ofApp::setup") << "Failed to parse JSON" << endl;
	}

	
}
