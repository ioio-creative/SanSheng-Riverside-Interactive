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
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofEnableAntiAliasing();
	ofEnableSmoothing();


	VideoPlayerManager.setup();

#ifdef USE_PARTICLE

	//particle
	 // 1,000,000 particles
	unsigned partW = w;
	unsigned partH = h;

	particles.init(partW, partH);

	if (ofIsGLProgrammableRenderer()) particles.loadShaders("shaders330/update", "shaders330/draw");
	else particles.loadShaders("shaders120/update", "shaders120/draw");

	// initial positions
	// use new to allocate 4,000,000 floats on the heap rather than
	// the stack
	float* particlePosns = new float[partW * partH * 4];
	for (unsigned y = 0; y < partH; ++y)
	{
		for (unsigned x = 0; x < partW; ++x)
		{
			unsigned idx = y * partW + x;
			particlePosns[idx * 4] = 400.f * x / (float)partW - 200.f; // particle x
			particlePosns[idx * 4 + 1] = 400.f * y / (float)partH - 200.f; // particle y
			particlePosns[idx * 4 + 2] = 0.f; // particle z
			particlePosns[idx * 4 + 3] = 0.f; // dummy
		}
	}
	particles.loadDataTexture(ofxGpuParticles::POSITION, particlePosns);
	delete[] particlePosns;

	// initial velocities
	particles.zeroDataTexture(ofxGpuParticles::VELOCITY);

	// listen for update event to set additonal update uniforms
	ofAddListener(particles.updateEvent, this, &ofApp::onParticlesUpdate);


#endif
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

	VideoPlayerManager.update();


#ifdef USE_PARTICLE
	particles.update();
#endif
}

//--------------------------------------------------------------
void ofApp::draw() {

	//ofBackground(255, 255, 255);
	int vidAlpha = ofMap(mouseY, 0, ofGetWindowHeight(), 0, 255);


	//fbo - CG
	CGFbo.begin();
	ofClear(255, 255, 255, 0);
	//ofBackground(0, 0, 0);

#ifdef USE_PARTICLE
	cam.begin();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	particles.draw();
	ofDisableBlendMode();
	cam.end();
#endif
	CGFbo.end();

	VideoPlayerManager.draw();
	ofSetColor(255, 255, 255, vidAlpha);
	CGFbo.draw(0, 0);

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


void ofApp::exit() {
	VideoPlayerManager.exit();
}



#ifdef USE_PARTICLE
// set any update uniforms in this function
void ofApp::onParticlesUpdate(ofShader& shader)
{
	ofVec3f mouse(ofGetMouseX() - .5f * ofGetWidth(), .5f * ofGetHeight() - ofGetMouseY(), 0.f);
	shader.setUniform3fv("mouse", mouse.getPtr());
	shader.setUniform1f("elapsed", ofGetLastFrameTime());
	shader.setUniform1f("radiusSquared", 200.f * 200.f);
}

#endif