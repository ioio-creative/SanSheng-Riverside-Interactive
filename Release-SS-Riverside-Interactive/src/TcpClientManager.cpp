#include "TcpClientManager.h"



//--------------------------------------------------------------
void TcpClientManager::setup() {

	ofSetBackgroundColor(230);

	//======================== Network ==========================

	// our send and recieve strings
	msgTx = "";
	msgRx = "";

	tcpMessageToSend = "";

	connectTime = 0;
	deltaTime = 0;

	//======================== JSON =============================
	loadSettings();

	//======================== GUI ==============================


	tcpGui.setup(); // most of the time you don't need a name

	//tcpGui.add(playBtn.setup(videoCommandsName[0]));
	tcpGui.add(ip.set("IP : ", ip));
	tcpGui.add(port.set("Port : ", port));
	tcpGui.add(playBtn.setup(videoCommandsName[1]));
	tcpGui.add(prevBtn.setup(videoCommandsName[2]));
	tcpGui.add(nextBtn.setup(videoCommandsName[3]));
	tcpGui.add(stopBtn.setup(videoCommandsName[6]));
	tcpGui.add(volumeUp.setup(videoCommandsName[8]));
	tcpGui.add(volumeDn.setup(videoCommandsName[9]));
	tcpGui.add(status.setup("TCP Send", tcpMessageToSend));

	playBtn.addListener(this, &TcpClientManager::playButtonPressed);
	prevBtn.addListener(this, &TcpClientManager::prevButtonPressed);
	nextBtn.addListener(this, &TcpClientManager::nextButtonPressed);
	stopBtn.addListener(this, &TcpClientManager::stopButtonPressed);
	volumeUp.addListener(this, &TcpClientManager::volumeUpButtonPressed);
	volumeDn.addListener(this, &TcpClientManager::volumeDnButtonPressed);


	tcpGui.setPosition(ofGetWidth() - 410, 10);
	tcpGui.setWidthElements(400);

	prevIp = ip;
	prevPort = port;

	isSetup = true;
}


void TcpClientManager::playButtonPressed() {
	sendTCPMessage(videoCommandsString[1]);
};
void TcpClientManager::prevButtonPressed() {
	sendTCPMessage(videoCommandsString[2]);
};
void TcpClientManager::nextButtonPressed() {
	sendTCPMessage(videoCommandsString[3]);
};
void TcpClientManager::stopButtonPressed() {
	sendTCPMessage(videoCommandsString[6]);
};
void TcpClientManager::volumeUpButtonPressed() {
	sendTCPMessage(videoCommandsString[8]);
};
void TcpClientManager::volumeDnButtonPressed() {
	sendTCPMessage(videoCommandsString[9]);
};



//--------------------------------------------------------------
void TcpClientManager::update() {
	if (isSetup) {
		if (prevIp.compare(ip) || prevPort.compare(port)) {
			prevIp = ip;
			prevPort = port;
			tcpClient.close();
			saveSettings();
			tcpClient.setup(ip, ofToInt(port));
			tcpClient.setMessageDelimiter(delimiter);
			connectTime = ofGetElapsedTimeMillis();
		}

		//======================== Network ==========================
		if (tcpClient.isConnected()) {
			// we are connected - lets try to receive from the server
			string str = tcpClient.receive();
			if (str.length() > 0) {
				msgRx = str;
			}
		}
		else {
			msgTx = "";
			// if we are not connected lets try and reconnect every 5 seconds
			deltaTime = ofGetElapsedTimeMillis() - connectTime;

			if (deltaTime > 5000) {
				tcpClient.setup(ip, ofToInt(port));
				tcpClient.setMessageDelimiter(delimiter);
				connectTime = ofGetElapsedTimeMillis();
			}

		}
	}
}

void TcpClientManager::exit() {
	if (isSetup) {
		playBtn.removeListener(this, &TcpClientManager::playButtonPressed);
		prevBtn.removeListener(this, &TcpClientManager::prevButtonPressed);
		nextBtn.removeListener(this, &TcpClientManager::nextButtonPressed);
		stopBtn.removeListener(this, &TcpClientManager::stopButtonPressed);
		volumeUp.removeListener(this, &TcpClientManager::volumeUpButtonPressed);
		volumeDn.removeListener(this, &TcpClientManager::volumeDnButtonPressed);
	}
}


//--------------------------------------------------------------
void TcpClientManager::draw() {
	if (isSetup) {
		stringstream ss;
		ss << "ip : " << ip << " port : " << port << endl;
		ofSetColor(20);
		ofDrawBitmapString(ss.str(), 15, 30);


		//======================== Network ==========================

		if (tcpClient.isConnected()) {
			if (!msgTx.empty()) {
				status = "sending : " + msgTx;
				ofDrawBitmapString("sending:", 15, 55);
				ofDrawBitmapString(msgTx, 85, 55);
			}
			else {
				ofDrawBitmapString("status: type something to send data to port " + ofToInt(port), 15, 55);
			}
			ofDrawBitmapString("from server: \n" + msgRx, 15, 270);
		}
		else {
			ofDrawBitmapString("status: server not found. launch server app and check ports!\n\nreconnecting in " + ofToString((5000 - deltaTime) / 1000) + " seconds", 15, 55);
		}

		//======================== JSON =============================

		//======================== GUI ==============================
		tcpGui.draw();
	}
}


//--------------------------------------------------------------
void TcpClientManager::keyPressed(int key) {
	if (isSetup) {
		// you can only type if you're connected
		// we accumulate 1 line of text and send every typed character
		// on the next character after a breakline we clear the buffer
		if (tcpClient.isConnected()) {
			if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
				if (!msgTx.empty()) {
					msgTx = msgTx.substr(0, msgTx.size() - 1);
				}
			}
			else if (key != 0) {
				ofUTF8Append(msgTx, key);
			}
			tcpClient.send(msgTx);
			if (!msgTx.empty() && msgTx.back() == '\n') {
				msgTx.clear();
			}
		}
	}
}


void TcpClientManager::sendTCPMessage(string s) {

	if (tcpClient.isConnected()) {
		tcpClient.send(s);
		status = s;
	}
	else {
		status = "not connected, " + s;
	}


}

//======================== JSON =============================
void TcpClientManager::saveSettings() {
	// now write pretty print

	settings["ip"] = ofToString(ip);
	settings["port"] = ofToString(port);
	if (!settings.save("tcpSettings.json", true))
	{
		ofLogNotice("TcpClientManager::setup") << "example_output_pretty.json written unsuccessfully.";
	}
	else
	{
		ofLogNotice("TcpClientManager::setup") << "example_output_pretty.json written successfully.";
	}
}

void TcpClientManager::loadSettings() {

	std::string file = "tcpSettings.json";
	// Now parse the JSON
	bool parsingSuccessful = settings.open(file);

	if (parsingSuccessful)
	{
		ofLogNotice("TcpClientManager::setup") << settings.getRawString();

		ip = settings["ip"].asString();
		port = settings["port"].asString();

		ofxTCPSettings IPsettings(ip, ofToInt(port));

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

		ofLog() << " settings['video-commands'].size(); " << settings["video-commands"].size();

		for (int i = 0; i < settings["video-commands"].size(); i++) {
			videoCommandsName.push_back(settings["video-commands"][i]["name"].asString());
			videoCommandsString.push_back(settings["video-commands"][i]["string"].asString());
		}

	}
	else
	{
		ofLogError("TcpClientManager::setup") << "Failed to parse JSON" << endl;
	}


}
