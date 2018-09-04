#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	//debug
	debugMode = false;

	//load settings
	//JSON
	loadSettings();

	//misc
	if (!debugMode) {
		ofHideCursor();
	}

	//init
	ofEnableAntiAliasing();
	ofEnableSmoothing();
	ofSetVerticalSync(true);

	w = ofGetScreenWidth();
	h = ofGetScreenHeight();

	//video
	currVidID = 0;
#ifdef USE_HPVPLAYER
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
#else
	vid[i].load("1.hpv");
#endif
	}

	CGFbo.allocate(w, h, GL_RGBA);
	VideoFbo.allocate(w, h, GL_RGBA);

	vid[currVidID].setPaused(false);
	vid[currVidID].play();
}

//--------------------------------------------------------------
void ofApp::update() {
	currTime = ofGetElapsedTimef();

	if (!debugMode) {
		ofHideCursor();
	}
	else {
		ofShowCursor();
	}

	//video
#ifdef USE_HPVPLAYER
	HPV::Update();
#else
	vid[currVidID].update();
#endif

	if (vid[currVidID].getCurrentFrame() == vid[currVidID].getTotalNumFrames()-1) {
		vid[currVidID].setPaused(true);
		vid[currVidID].seekToFrame(0);
		if (debugMode) {
			ofLog() << "Movie Done: " << currVidID;
		}

		currVidID++;
		

		if (currVidID >= NUM_OF_VID) {
			currVidID = 0;
		}
		if (debugMode) {
		ofLog() << "Movie Play: " << currVidID;
		}

		vid[currVidID].setPaused(false);
		vid[currVidID].play();
	}


}

//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(255, 255, 255);
	int vidAlpha = ofMap(mouseY, 0, ofGetWindowHeight(), 0, 255);


	//fbo - CG
	CGFbo.begin();
	ofClear(255, 255, 255, 0);
	CGFbo.end();


	//fbo - Video
	VideoFbo.begin();
	ofClear(255, 255, 255, 0);
	ofSetColor(255, 255, 255);
	vid[0].draw(0, 0, w, h / 2);
	vid[1].draw(0, h / 2, w, h/2);
	VideoFbo.end();

	//fbo - draw
	CGFbo.draw(0,0);
	ofSetColor(255, 255, 255, vidAlpha);
	VideoFbo.draw(0,0);

	if (debugMode) {
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 20, 20);
	}
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



//--------------------------------------------------------------
//-------------------------- Controls --------------------------
//--------------------------------------------------------------


void ofApp::keyReleased(int key) {
	switch (key) {
	case 'd':
		debugMode = !debugMode;
		break;
	}

}

//--------------------------------------------------------------
//-------------------------- video --------------------------
//--------------------------------------------------------------
#ifdef USE_HPVPLAYER
void exit() {
	/* Cleanup and destroy HPV Engine upon exit */
	HPV::DestroyHPVEngine();
}
#else

#endif