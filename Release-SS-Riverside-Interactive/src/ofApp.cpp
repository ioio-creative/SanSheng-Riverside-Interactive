#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){

#ifdef EXHIBITION
	ofHideCursor();
#endif // EXHIBITION

	ofLog() << "CANVAS_WIDTH : " << CANVAS_WIDTH << "CANVAS_HEIGHT : " << CANVAS_HEIGHT;
	//debug
	debugMode = true;
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
	KinectMapper.refreshCamToScreenTransform();
	//calibration gui
	setupCalibrationGui();
	

	//------------------------------------- VideoPlayerManager ------------------------------------- 
	VideoPlayerManager.setup(CANVAS_WIDTH,CANVAS_HEIGHT);
	drawVideoPlayerManager = true;

	//------------------------------------- Particle Visuals Manager-------------------------------------
	ParticleVisualsManager.setup(CANVAS_WIDTH, CANVAS_HEIGHT);
	
	//------------------------------------- TCP Client Manager-------------------------------------
	//TcpClientManager.setup();

	//fbo
	CGFbo.allocate(CANVAS_WIDTH, CANVAS_HEIGHT, GL_RGBA);
	/*KinectVisionFbo.allocate(CANVAS_WIDTH, CANVAS_HEIGHT / 3, GL_RGBA);
	KinectVisionFbo.begin();
	ofClear(ofColor::black);
	KinectVisionFbo.end();
	kinect3DCam.setControlArea(ofRectangle(0, CANVAS_HEIGHT * 1/3, CANVAS_WIDTH, CANVAS_HEIGHT /3));*/
	
	KinectMapper.setupCavasCalibrateFbo();



	VideoPlayerManager.keyReleased('d');
	for (int i = 0; i < MAX_PLAYERS; i++) {
		bodyPos.push_back(ofVec2f(-100,-100));
	}

	// ===== scene settings ====

	//init
	num_of_trigger_scene = 7;
	for (int i = 0; i < num_of_trigger_scene; i++) {
		isTriggerScene.push_back(false);
		triggerSceneTime.push_back(0.0f);
	}
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
	//------------------------------------- TCP Client Manager-------------------------------------
	TcpClientManager.update();

}

//--------------------------------------------------------------
void ofApp::draw(){

	if (debugMode) {
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 20, 20);
	}
	ofSetColor(255);
	ofEnableAlphaBlending();

	


	//------------------------------------- VideoPlayerManager ------------------------------------- 
//	int a = ofMap(mouseY, 0, ofGetScreenHeight(), 0, 255);
	VideoPlayerManager.setAlpha(255);
	VideoPlayerManager.draw(0,0,CANVAS_WIDTH,CANVAS_HEIGHT);


	for (int i = 0; i < num_of_trigger_scene; i++) {
		if (VideoPlayerManager.getVideoTime() >= triggerSceneTime[i] && isTriggerScene[i] == false) {
			isTriggerScene[i] = true;
			ofLog() << "isTriggerScene" << i;
			if (i == 0) {
				ParticleVisualsManager.keyPressed('0');
				VideoPlayerManager.keyReleased('0');
			}
			if (i == 1) {
				ParticleVisualsManager.keyPressed('1');
				VideoPlayerManager.keyReleased('1');
			}
			if (i == 2) {
				ParticleVisualsManager.keyPressed('2');
				VideoPlayerManager.keyReleased('2');
			}
			if (i == 3) {
				ParticleVisualsManager.keyPressed('3');
				VideoPlayerManager.keyReleased('3');
			}
			if (i == 4) {
				ParticleVisualsManager.keyPressed('4');
				VideoPlayerManager.keyReleased('4');
			}
			if (i == 5) {
				ParticleVisualsManager.keyPressed('5');
				VideoPlayerManager.keyReleased('5');
			}
		}
	}


	//------------------------------------- Particle Visuals Manager-------------------------------------
//ParticleVisualsManager.setAlpha(a);
	ParticleVisualsManager.draw();
	
	//------------------------------------- TCP Client Manager-------------------------------------
	TcpClientManager.draw();

	//------------------------------------- Kinect 3D View (for calibration) -------------------------
	if (calibrationMode)
	{
		KinectMapper.CanvasCalibrateFbo.draw(0, 0);
		SanShengKinectManager->draw();
		calibrationGui.draw();
	}


	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (SanShengKinectManager->bodyIdxTracked[i])
		{
			
			bodyPos[i] = ofVec2f(SanShengKinectManager->bodyPosOnScreen[i].x, SanShengKinectManager->bodyPosOnScreen[i].y);
			ofSetColor(ofColor::aqua);
			ofFill();
			ofDrawEllipse(bodyPos[i].x, bodyPos[i].y, 100, 100);
		}
		else continue;
	}

	
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	ParticleVisualsManager.keyPressed(key);
	TcpClientManager.keyPressed(key);
	VideoPlayerManager.keyReleased(key);
	//SanShengKinectManager->keyReleased(key);
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
	break;
	}


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
	//------------------------------------- TCP Client Manager-------------------------------------
	TcpClientManager.exit();
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
	float scene1Start = 10.0f;
	float scene1End = 20.0f;
	float scene2Start = 35.0f;
	float scene2End = 40.0f;
	float scene3Start = 50.0f;
	float scene3End = 60.0f;

	float totalEndTime = VideoPlayerManager.getVideoEndTime();
	//Time Settings
	if (debugMode) {
		ofLog() << "end time : " << totalEndTime;
	}


	for (int i = 0; i < num_of_trigger_scene; i++) {
		triggerSceneTime = { totalEndTime, scene1Start , scene1End, scene2Start, scene2End, scene3Start, scene3End };
	}

}
