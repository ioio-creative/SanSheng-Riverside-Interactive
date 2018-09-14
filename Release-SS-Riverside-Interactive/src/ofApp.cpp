#include "ofApp.h"

//Kinect sources dimensions
#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424
#define DEPTH_SIZE DEPTH_WIDTH * DEPTH_HEIGHT

#define COLOR_WIDTH 1920
#define COLOR_HEIGHT 1080

#define MAX_PLAYERS 6
#define REFJOINTTYPE JointType_SpineShoulder

#define CANVAS_WIDTH 1280
#define CANVAS_HEIGHT 720

//--------------------------------------------------------------
void ofApp::setup(){
	//debug
	debugMode = false;
	ofLogToConsole();
	ofSetLogLevel(OF_LOG_ERROR);

	//JSON - load settings
	loadSettings();

	//misc
	if (!debugMode) {
		ofHideCursor();
	}

	//init
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofEnableAntiAliasing();
	ofEnableSmoothing();

	w = ofGetScreenWidth();
	h = ofGetScreenHeight();
	w = ofGetWidth();
	h = ofGetHeight();

	//kinect
	kinect.open();
	kinect.initDepthSource();
	kinect.initColorSource();
	kinect.initInfraredSource();
	kinect.initBodySource();
	kinect.initBodyIndexSource();

	if (kinect.getSensor()->get_CoordinateMapper(&coordinateMapper) < 0) {
		ofLogError() << "Could not acquire CoordinateMapper!";
	}
	bHaveAllStreams = false;

	bodyIdxTracked.resize(MAX_PLAYERS);
	bodyPositions.resize(MAX_PLAYERS);
	bodyPosOnScreen.resize(MAX_PLAYERS);

	//video player
	setupVideoPlayer();
	
	//fbo
	CGFbo.allocate(w, h, GL_RGBA);
	VideoFbo.allocate(w, h, GL_RGBA);
	KinectCalibrateFbo.allocate(w, h, GL_RGBA);

}

//--------------------------------------------------------------
void ofApp::update(){

	if (!debugMode) {
		ofHideCursor();
	}
	else {
		ofShowCursor();
	}

	//kinect update
	kinect.update();
	floorPlane = kinect.getBodySource()->getFloorClipPlane();
}

//--------------------------------------------------------------
void ofApp::draw(){

	if (debugMode) {
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 20, 20);
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

	switch (key) {
	case 'd':
		debugMode = !debugMode;
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	}
}
//--------------------------------------------------------------
//------------------------- Video Player -----------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::setupVideoPlayer() {

	HPV::InitHPVEngine();
	/* Create resources for new player */
	/* Alternatively, if you experience playback stutter, try to toggle double-buffering true/false
	 * Default: OFF
	 *
	 * hpvPlayer.setDoubleBuffered(true);
	 */

	for (int i = 0; i < NUM_OF_VID; i++) {
		vid.push_back(ofxHPVPlayer());
		vid[i].init(HPV::NewPlayer());
		ofLog() << i;
		/* Try to load file and start playback */
		vid[i].load(ofToString(i) + ".hpv");
		vid[i].setLoopState(OF_LOOP_NONE);
		vid[i].play();
		vid[i].setPaused(true);
	}

	//Play Standby Video
/*
vid[currVidID].setPaused(false);
vid[currVidID].play();
*/
}

void ofApp::exit() {
	/* Cleanup and destroy HPV Engine upon exit */
	HPV::DestroyHPVEngine();

	/* Close and dispose Kinect sensor upon exit*/
	kinect.close();
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