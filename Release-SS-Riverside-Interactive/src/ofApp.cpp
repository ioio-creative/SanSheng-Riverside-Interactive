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
	setupKinect2();

	//video player
	setupVideoPlayer();
	
	//fbo
	CGFbo.allocate(w, h, GL_RGBA);
	VideoFbo.allocate(w, h, GL_RGBA);
	KinectCalibrateFbo.allocate(w, h, GL_RGBA);
	KinectCalibrateFbo.begin();
	ofClear(ofColor::black);
	KinectCalibrateFbo.end();

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
	updateKinect2();
}

//--------------------------------------------------------------
void ofApp::draw(){

	if (debugMode) {
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 20, 20);
	}

	if (calibrationMode)
	{
		drawKinectFbo();		
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
	case 'k':
		calibrationMode = !calibrationMode;
		if (!calibrationMode)
		{
			KinectCalibrateFbo.clear();
		}
	}
}

//--------------------------------------------------------------
//------------------------- Kinect -----------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::setupKinect2() {
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
}

void ofApp::updateKinect2() {
	kinect.update();
	floorPlane = kinect.getBodySource()->getFloorClipPlane();
	floorTransform = glm::mat4(kinect.getBodySource()->getFloorTransform());
	string floorMsg = "[" + to_string(floorPlane.x) + "][" + to_string(floorPlane.y) + "][" + to_string(floorPlane.z) + "][" + to_string(floorPlane.w) + "]";
	ofLogNotice() << "Floor Plane Vector: " << floorMsg << endl;
	tiltAngle = atan(floorPlane.z / floorPlane.y);
	rollAngle = atan(floorPlane.x / floorPlane.y);

	// Make sure there's some data here, otherwise the cam probably isn't ready yet
	auto& bodySrc = kinect.getBodySource();
	if ( bodySrc->isFrameNew() && !bodySrc->getBodies().size()) {
		bHaveAllStreams = false;
		return;
	}
	else {
		bHaveAllStreams = true;
	}

	// Count number of tracked bodies, and clear bool flags
	numBodiesTracked = 0;
	for (int i = 0; i < bodyIdxTracked.size(); i++)
	{
		bodyIdxTracked[i] = false;
	}

	auto& bodies = bodySrc->getBodies();
	ofLogNotice() << "tracked body ID: ";
	for (auto& body : bodies) {
		if (body.tracked) {
			numBodiesTracked++;
			int bodyIdx = body.bodyId;
			bodyIdxTracked[bodyIdx] = true;
			ofVec3f refJoint = body.joints.at(REFJOINTTYPE).getPosition();
			bodyPositions[bodyIdx] = glm::vec3(refJoint.x, refJoint.y, refJoint.z);
			ofLogNotice() << "[" << bodyIdx << "]";
		}
	}
	ofLogNotice() << endl;

	colorTex = kinect.getColorSource()->getTexture();
}

void ofApp::drawKinectFbo() {
	
	
	
	KinectCalibrateFbo.begin();
	

	ofPushMatrix();
	ofDrawGrid(20, 20, true, true, true, true);
	ofScale(100, 100, 100);

	ofPushMatrix();
	glm::vec3 originOnFloor = projectedPointOntoPlane(glm::vec3(), floorPlane);
	ofTranslate(originOnFloor);
	ofRotateXDeg(90);
	ofRotateXRad(tiltAngle);
	ofRotateZRad(rollAngle);
	ofSetColor(ofColor::lightCoral);
	ofDrawPlane(0, 0, 0, 5, 5);
	ofPopMatrix();

	//draw origin projected on floor
	//ofSetColor(0);
	//ofFill();
	//ofDrawSphere(originOnFloor, 0.1);

	for (auto& body : kinect.getBodySource()->getBodies()) {
		int bodyIdx = int(body.bodyId);
		if (body.tracked)
		{
			ofSetColor(ofColor::blue);
			ofFill();
			for (auto& joint : body.joints)
			{
				//ofVec2f jointDepthPos = joint.second.getPositionInDepthMap();
				ofVec3f jointPos = joint.second.getPositionInWorld();
				ofDrawSphere(jointPos.x, jointPos.y, jointPos.z, 0.02);
			}

			ofSetColor(255, 50, 50);
			ofFill();

			glm::vec3& torsoJoint = bodyPositions[bodyIdx];
			glm::vec3 bodyOnFloor = projectedPointOntoPlane(torsoJoint, floorPlane);
			glm::vec4 bodyPosVec4 = glm::vec4(bodyOnFloor.x, bodyOnFloor.y, bodyOnFloor.z, 1);
			glm::vec4 bodyPosOnHorizonOffset = floorTransform * bodyPosVec4;
			bodyPosOnScreen[bodyIdx] = glm::vec2(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.z);
			//Red Box drawn along 3d floor plane
			ofDrawBox(bodyOnFloor, 0.2);
			//Blue Box drawn along horizontal plane, projected from RedBox
			ofSetColor(ofColor::aqua);
			ofDrawBox(bodyPosOnScreen[bodyIdx].x, 0, bodyPosOnScreen[bodyIdx].y, 0.15);
			//Gold Box drawn from mat4 multiply Position on Floor
			ofSetColor(ofColor::gold);
			ofNoFill();
			ofDrawBox(glm::vec3(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.y, bodyPosOnHorizonOffset.z), 0.25);
		}
	}
	ofPopMatrix();


	KinectCalibrateFbo.end();
	KinectCalibrateFbo.draw(0, 0);
}

glm::vec3 ofApp::projectedPointOntoPlane(glm::vec3 point, Vector4 plane) {
	glm::vec3 n = glm::vec3(plane.x, plane.y, plane.z);
	return (point - (glm::dot(n, point) + plane.w) * n);
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