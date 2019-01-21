#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
#ifdef LANDSCAPE_MODE
	landscapeFbo.allocate(CANVAS_HEIGHT, CANVAS_WIDTH, GL_RGBA);
#endif
#ifdef EXHIBITION
	ofHideCursor();
#endif // EXHIBITION

	ofLog() << "CANVAS_WIDTH : " << CANVAS_WIDTH << "CANVAS_HEIGHT : " << CANVAS_HEIGHT;
	//debug
	debugMode = false;
	ofLogToConsole();
	//ofSetLogLevel(OF_LOG_ERROR);
	ofSetLogLevel(OF_LOG_NOTICE);

	ofSetFrameRate(TARGET_FRAMERATE);

	//JSON - load settings
	loadSettings();

	//misc

	if (!debugMode) {
		//ofHideCursor();
	}

	//init
	ofSetVerticalSync(true);

	ofEnableAntiAliasing();
	ofEnableSmoothing();

	//kinect
	//SanShengKinectManager = KinectManager(CANVAS_WIDTH, CANVAS_HEIGHT / 3, MAX_PLAYERS);
	SanShengKinectManager->setup();

	//calibration gui
	setupCalibrationGui();
	//!! refresh after setupCalibrationGui() to retreive xml data
	KinectMapper.refreshCamToScreenTransform();


	//------------------------------------- VideoPlayerManager -------------------------------------
	VideoPlayerManager.setup(CANVAS_WIDTH,CANVAS_HEIGHT);
	VideoPlayerManager.showDelayMillis = VIDEO_PANEL_TRIGGER_DELAY;
	drawVideoPlayerManager = true;

	//------------------------------------- Particle Visuals Manager-------------------------------------
	ParticleVisualsManager.setup(CANVAS_WIDTH, CANVAS_HEIGHT);

#ifdef USE_TCP_COMMUNICATION
	//------------------------------------- TCP Client Manager-------------------------------------
	TcpClientManager.setup();
#endif

	//------------------------------------- Serial-------------------------------------
	serialSetup();


	//fbo
	CGFbo.allocate(CANVAS_WIDTH, CANVAS_HEIGHT, GL_RGBA);
	/*KinectVisionFbo.allocate(CANVAS_WIDTH, CANVAS_HEIGHT / 3, GL_RGBA);
	KinectVisionFbo.begin();
	ofClear(ofColor::black);
	KinectVisionFbo.end();
	kinect3DCam.setControlArea(ofRectangle(0, CANVAS_HEIGHT * 1/3, CANVAS_WIDTH, CANVAS_HEIGHT /3));*/
	KinectMapper.setupCavasCalibrateFbo();


	// ===== scene settings ====

	//init
	num_of_trigger_scene = 12;
	for (int i = 0; i < num_of_trigger_scene; i++) {
		isTriggerScene.push_back(false);
		triggerSceneTime.push_back(0.0f);
	}
	ParticleVisualsManager.keyPressed('0');
	VideoPlayerManager.keyReleased('0');
	resetScene();

}

//--------------------------------------------------------------
void ofApp::update(){

	if (!debugMode) {
		//ofHideCursor();
	}
	else {
		ofShowCursor();
	}

	//kinect update
	SanShengKinectManager->update(KinectMapper, refBodyIdx);

	//update ofxGui
	updateGuiInspectorValues();

	//------------------------------------- VideoPlayerManager -------------------------------------
	VideoPlayerManager.update();
	//------------------------------------- Particle Visuals Manager-------------------------------------
	ParticleVisualsManager.update();

#ifdef USE_TCP_COMMUNICATION
	//------------------------------------- TCP Client Manager-------------------------------------
	TcpClientManager.update();
#endif

//------------------------------------- Serial ---------------------------------



	//------ Control Room -------
	string sTemp = serialReadCtrlrm();
	if (sTemp.find("3333") != std::string::npos) {
		receivedString = sTemp;
		ofLog() << "Show Done, Reset";
		resetScene();

		ParticleVisualsManager.keyPressed('0');
		VideoPlayerManager.keyReleased('0');

	}else if (sTemp.find("4444") != std::string::npos) {
		receivedString = sTemp;
		ofLog() << "Show Begin";
		ParticleVisualsManager.keyPressed('1');
		VideoPlayerManager.isDelayTrigger = true;
		VideoPlayerManager.showDelayMillis = VIDEO_PANEL_TRIGGER_DELAY;
		VideoPlayerManager.keyReleased('1');
	}


}
void ofApp::drawAll() {

	ofSetColor(255);
	ofEnableAlphaBlending();

	//------------------------------------- Kinect Position -------------------------------------
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (debugMode) {
			if (i == 0) {
				bodyPos[i] = ofVec2f(ofGetMouseX(), ofGetMouseY());
			}
		}
		else {
			if (SanShengKinectManager->bodyIdxTracked[i])
			{
				bodyPos[i] = ofVec2f(SanShengKinectManager->bodyPosOnScreen[i].x, SanShengKinectManager->bodyPosOnScreen[i].y + CANVAS_HEIGHT *0.55);

			}
			else {
				//continue;
				bodyPos[i] = ofVec2f(CANVAS_WIDTH / 2, CANVAS_HEIGHT * 2);
			}
		}
	}
	ParticleVisualsManager.setBodyPos(bodyPos);

	//------------------------------------- VideoPlayerManager -------------------------------------
//	int a = ofMap(mouseY, 0, ofGetScreenHeight(), 0, 255);
	VideoPlayerManager.setAlpha(255);
	VideoPlayerManager.draw(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, bodyPos);

	for (int i = 0; i < num_of_trigger_scene; i++) {
		if (VideoPlayerManager.getVideoTime() >= triggerSceneTime[i] && isTriggerScene[i] == false) {
			isTriggerScene[i] = true;
			ofLog() << "isTriggerScene" << i;
			if (i == 0) {
				ParticleVisualsManager.keyPressed('2');
				VideoPlayerManager.keyReleased('2');
			}
			if (i == 1 || i == 3 || i == 5) {
				ParticleVisualsManager.keyPressed('3');
				VideoPlayerManager.keyReleased('3');
			}
			if (i == 2 || i == 4 || i == 6) {
				ParticleVisualsManager.keyPressed('4');
				VideoPlayerManager.keyReleased('4');
			}
			if (i == 7) {
				ParticleVisualsManager.keyPressed('5');
				VideoPlayerManager.keyReleased('5');
			}
			if (i == 8) {
				ParticleVisualsManager.keyPressed('6');
				VideoPlayerManager.keyReleased('6');
			}
			if (i == 9) {
				ParticleVisualsManager.keyPressed('7');
				VideoPlayerManager.keyReleased('7');
			}
			if (i == 10) {
				ParticleVisualsManager.keyPressed('8');
				VideoPlayerManager.keyReleased('8');
			}
			if (i == 11) {
				ParticleVisualsManager.keyPressed('0');
				VideoPlayerManager.keyReleased('0');
			}
		}
	}

	//------------------------------------- Particle Visuals Manager-------------------------------------
//ParticleVisualsManager.setAlpha(a);
	ParticleVisualsManager.draw();

#ifdef USE_TCP_COMMUNICATION
	//------------------------------------- TCP Client Manager-------------------------------------
	TcpClientManager.draw(debugMode);
#endif


	if (debugMode) {

		ofSetColor(ofColor::aqua);
		ofFill();
		for (int i = 0; i < MAX_PLAYERS; i++) {
			ofDrawEllipse(bodyPos[i].x, bodyPos[i].y, 15, 15);
		}
	}

}
//--------------------------------------------------------------
void ofApp::draw(){

#ifdef	LANDSCAPE_MODE
	landscapeFbo.begin();
	ofPushMatrix();
	ofTranslate(CANVAS_HEIGHT, 0);
	ofRotate(90);

	drawAll();
	ofPopMatrix();

	landscapeFbo.end();

	landscapeFbo.draw(0, 0);
#else
	drawAll();
#endif
	//------------------------------------- Kinect 3D View (for calibration) -------------------------
	if (calibrationMode)
	{
		KinectMapper.CanvasCalibrateFbo.draw(0, ofGetHeight() - KINECTAREA_HEIGHT);
		SanShengKinectManager->draw();
		calibrationGui.draw();
	}
	if (debugMode) {
		serialDraw();
		debugDraw();
		ofSetColor(255);
	}
}
//--------------------------------------------------------------
void ofApp::debugDraw() {

	ofSetColor(255, 0, 0);
	std::stringstream ss;

	ss << "framerate : " << ofGetFrameRate();
	ss << "window resolution : " << ofGetWindowWidth() << " x " << ofGetWindowHeight()<< endl;

	ofDrawBitmapString(ss.str(), ofVec2f(20, 400));

}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

	switch (key) {
	case 'd':
		debugMode = !debugMode;
		if (debugMode)
		{
			ofSetLogLevel(OF_LOG_NOTICE);
		}
		else
		{
			ofSetLogLevel(OF_LOG_ERROR);
		}
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'k':
		calibrationMode = !calibrationMode;
		if (!calibrationMode)
		{
			SanShengKinectManager->Kinect3dCamFbo.begin();
			ofClear(0);
			SanShengKinectManager->Kinect3dCamFbo.end();
		}
		break;

	case 'v':
		ofLog() << "drawVideoPlayerManager : " << drawVideoPlayerManager;
		drawVideoPlayerManager != drawVideoPlayerManager;
		break;

	case '0':
		resetScene();
		sendCommand("0000");
	break;

	case '1':
		sendCommand("1111");
		VideoPlayerManager.isDelayTrigger = true;
		VideoPlayerManager.showDelayMillis = VIDEO_FLOOR_TRIGGER_DELAY;
	break;

	case 's':
		calibrationGui.saveToFile("settings.xml");
		break;
	case 'l':
		calibrationGui.loadFromFile("settings.xml");
		break;

	default:
		break;
	}
	ParticleVisualsManager.keyPressed(key);
#ifdef USE_TCP_COMMUNICATION
	TcpClientManager.keyPressed(key);
#endif
	ParticleVisualsManager.keyPressed(key);
	VideoPlayerManager.keyReleased(key);
	//SanShengKinectManager->keyReleased(key);

}

void ofApp::windowResized(int w, int h) {
	SanShengKinectManager->windowResized(w ,h);
}

//--------------------------------------------------------------
//------------------------- Calibration GUI --------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

void ofApp::setupCalibrationGui() {
	calibrationGui.setup("Kinect Calibration");

	refBodyIdx.addListener(this, &ofApp::refBodyIdxChanged);
	bodyPosGuiGroup.add(refBodyIdx.set("Selected Body Index", 0, 0, 5));
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		bodyPosGuiGroup.add(bodyPosInspect[i].set(("Body[" + ofToString(i) + "]"), ""));
	}
	calibrationGui.add(bodyPosGuiGroup);
	calibrationGui.add(selectedBodyLabel.setup(bodyPosInspect[refBodyIdx], 250, 25));

	KinectMapper.setupCalibrationParamGroup(calibrationGui);

}

void ofApp::refBodyIdxChanged(int& idx) {
	selectedBodyLabel.setup(bodyPosInspect[refBodyIdx], 250, 25);
}

void ofApp::updateGuiInspectorValues() {
	for (int i = 0; i < MAX_PLAYERS; i++){
		glm::vec3 bodyPos = SanShengKinectManager->bodyPositions[i];
		bodyPosInspect[i] = ofToString(bodyPos.x) + ", " + ofToString(bodyPos.y) + ", " + ofToString(bodyPos.z);
	}
}


void ofApp::exit() {
	//------------------------------------- VideoPlayerManager -------------------------------------
	VideoPlayerManager.exit();

#ifdef USE_TCP_COMMUNICATION
	//------------------------------------- TCP Client Manager-------------------------------------
	TcpClientManager.exit();
#endif

	//------------------------------------- Serial -------------------------------------

	arduino.unregisterAllEvents(this);
}

//--------------------------------------------------------------
//--------------------------JSON / SETTINGS---------------------
//--------------------------------------------------------------

void ofApp::saveSettings() {
	std::string file = "settings.json";

	// Now parse the JSON
	bool parsingSuccessful = settings.open(file);

	if (parsingSuccessful)
	{

		//settings["day"] = day;

		// now write pretty print
		if (!settings.save("settings.json", true))
		{
			ofLogNotice("ofApp::setup") << "settings.json written unsuccessfully.";
		}
		else
		{
			ofLogNotice("ofApp::setup") << "settings.json written successfully.";
		}
	}
	else
	{
		ofLogError("ofApp::setup") << "Failed to parse JSON" << endl;
	}

}
//--------------------------------------------------------------
void ofApp::loadSettings() {

	std::string file = "settings.json";

	// Now parse the JSON
	bool parsingSuccessful = settings.open(file);

	if (parsingSuccessful)
	{
		ofLogNotice("ofApp::setup") << settings.getRawString();

		//	day = settings["day"].asDouble();
	}
	else
	{
		ofLogError("ofApp::setup") << "Failed to parse JSON" << endl;
	}

	//img1.load("day" + ofToString(day) + "/3.jpg");
	//img2.load("day" + ofToString(day) + "/4.jpg");

}

void ofApp::resetScene() {

	for (int i = 0; i < num_of_trigger_scene; i++) {
		isTriggerScene[i] = false;
	}

	float totalEndTime = VideoPlayerManager.getVideoEndTime();
	//Time Settings
	if (debugMode) {
		ofLog() << "end time : " << totalEndTime;
	}

	float sceneEyeVidStart = 144.0f;  //2
	float sceneEyeExplode1Start = 155.0f;  //3
	float sceneEyeExplode1End = sceneEyeExplode1Start + 3.5f;  //4
	float sceneEyeExplode2Start = 171.0f;  //3
	float sceneEyeExplode2End = sceneEyeExplode2Start + 3.5f; //4
	float sceneEyeExplode3Start = 182.0f; //3
	float sceneEyeExplode3End = sceneEyeExplode3Start + 3.5f; //4
	float sceneRainfallStart = 245.0f; //5
	float sceneRainfallReverse = 271.0f; //6
	float sceneRainfallEnd = 276.0f; //7
	float sceneLastInteractiveStart = 294.0f;  //8
	float sceneAllEnd = totalEndTime; //9

		triggerSceneTime = { sceneEyeVidStart, sceneEyeExplode1Start , sceneEyeExplode1End, sceneEyeExplode2Start , sceneEyeExplode2End,sceneEyeExplode3Start , sceneEyeExplode3End,sceneRainfallStart, sceneRainfallReverse, sceneRainfallEnd, sceneLastInteractiveStart, sceneAllEnd };
		ParticleVisualsManager.keyPressed(']');
		for (int i = 0; i < MAX_PLAYERS; i++) {
			bodyPos.push_back(ofVec2f(CANVAS_WIDTH / 2, CANVAS_HEIGHT * 2));
			ParticleVisualsManager.keyPressed('[');
		}
		ParticleVisualsManager.setBodyPos(bodyPos);


}



//======================Serial=====================================



void ofApp::serialSetup() {


	//================== Serial ==================

	vector<ofxIO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();

	ofLogNotice("ofApp::setup") << "Connected Devices: ";

	for (std::size_t i = 0; i < devicesInfo.size(); ++i)
	{
		ofLogNotice("ofApp::setup") << "\t" << devicesInfo[i];
	}

	if (!devicesInfo.empty())
	{

		for (std::size_t i = 0; i < devicesInfo.size(); ++i)
		{
			string portDesc = devicesInfo[i].getHardwareId();
			ofLog() << "devicesInfo[i].getHardwareId() : " << devicesInfo[i].getHardwareId();

			bool success = false;
			if (portDesc.find("FTDI") != std::string::npos) {

				success = arduino.setup(devicesInfo[i], BAUD);

				if (success)
				{

						arduino.unregisterAllEvents(this);
						arduino.registerAllEvents(this);


					ofLogNotice("ofApp::setup") << "Successfully setup " << devicesInfo[i];


				}
				else
				{
					ofLogNotice("ofApp::setup") << "Unable to setup " << devicesInfo[i];
				}
			}
		}
	}
	else
	{
		ofLogNotice("ofApp::setup") << "No devices connected.";
	}

	ofLog() << "Serial Setup Done";
}






void ofApp::serialDraw() {

	ofSetColor(255);

	std::stringstream ss;

	ss << "Connected to: " << arduino.port() << endl;
	ss << "Received String: " << receivedString << endl;


	ofDrawBitmapString(ss.str(), ofVec2f(20, 200));

}




void ofApp::onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args)
{
	// Buffers will show up here when the marker character is found.
	SerialMessage message(args.getBuffer().toString(), "", 500);
	serialMessages.push_back(message);
	ofLog() << "SERIALLLLLLLL : " << message.message;
}

//--------------------------------------------------------------

void ofApp::onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args)
{
	// Errors and their corresponding buffer (if any) will show up here.
	SerialMessage message(args.getBuffer().toString(),
		args.getException().displayText(),
		500);
	serialMessages.push_back(message);
}

void ofApp::sendCommand(string s) {


	ofx::IO::ByteBuffer textBuffer(s);

		arduino.writeBytes(textBuffer);
		arduino.writeByte('\n');

	ofLog() << "send " << s << " to arduino ";
}



string ofApp::serialReadCtrlrm() {

	string combinedStr = "";


	try
	{
		// Read all bytes from the device;
		uint8_t buffer[1024];
		vector<uint8_t> finalBuffer;
		finalBuffer.clear();
		while (arduino.available() > 0)
		{
			std::size_t sz = arduino.readBytes(buffer, 1024);
			// ofLog() << "CTRL buffer size: " << sz;
			for (std::size_t j = 0; j < sz; ++j)
			{
				std::cout <<"Buffer : "<< buffer[j];
				//  ofLog() << "CTRL buf: " << buffer[j];
				if (buffer[j] == '3' || buffer[j] == '4' || buffer[j] == '5') {
					finalBuffer.push_back(buffer[j]);

				}


			}
			for (int i = 0; i < finalBuffer.size(); i++) {
				//  ofLog() << "CTRL New Buf : " << finalBuffer[i];
				//  ofLog() << "CTRL New Buf SIZE : " << finalBuffer.size();
				combinedStr += ofToString(finalBuffer[i]);
			}
		}

	}
	catch (const std::exception& exc)
	{
		ofLogError("ofApp::update") << exc.what();
	}
	//  }
	if (combinedStr.size() > 0) {
		std::cout << "has Buffer : " << combinedStr;
	}

	return combinedStr;

}
