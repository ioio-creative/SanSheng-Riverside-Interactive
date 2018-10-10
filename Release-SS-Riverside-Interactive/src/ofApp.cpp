#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
	ofLog() << "CANVAS_WIDTH : " << CANVAS_WIDTH << "CANVAS_HEIGHT : " << CANVAS_HEIGHT;
	//debug
	debugMode = false;
	ofLogToConsole();
	ofSetLogLevel(OF_LOG_ERROR);

	//JSON - load settings
	loadSettings();

	//misc
	if (!debugMode) {
		//ofHideCursor();
	}

	//init
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofEnableAntiAliasing();
	ofEnableSmoothing();
	
	//kinect
	setupKinect2();
	//calibration gui
	setupCalibrationGui();
	KinectMapper.refreshCamToScreenTransform();

	//------------------------------------- VideoPlayerManager ------------------------------------- 
	VideoPlayerManager.setup();
	drawVideoPlayerManager = true;

	//------------------------------------- Particle Visuals Manager-------------------------------------
	ParticleVisualsManager.setup(CANVAS_WIDTH, CANVAS_HEIGHT);
	
	//------------------------------------- TCP Client Manager-------------------------------------
	//TcpClientManager.setup();

	//fbo
	CGFbo.allocate(CANVAS_WIDTH, CANVAS_HEIGHT, GL_RGBA);
	KinectVisionFbo.allocate(CANVAS_WIDTH, CANVAS_HEIGHT / 3, GL_RGBA);
	KinectVisionFbo.begin();
	ofClear(ofColor::black);
	KinectVisionFbo.end();
	kinect3DCam.setControlArea(ofRectangle(0, CANVAS_HEIGHT * 1/3, CANVAS_WIDTH, CANVAS_HEIGHT /3));
	
	KinectMapper.setupCavasCalibrateFbo();

}


//--------------------------------------------------------------
void ofApp::update(){

	//if (!debugMode) {
	//	ofHideCursor();
	//}
	//else {
	//	ofShowCursor();
	//}

	//kinect update
	updateKinect2();

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
	

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (bodyIdxTracked[i])
		{
			ofSetColor(ofColor::aqua);
			ofFill();
			ofDrawEllipse(bodyPosOnScreen[i].x, bodyPosOnScreen[i].y, 100, 100);
		}
		else continue;
	}


	//------------------------------------- Particle Visuals Manager-------------------------------------
	ParticleVisualsManager.draw();

	//------------------------------------- VideoPlayerManager ------------------------------------- 
	int a = ofMap(mouseY, 0, ofGetScreenHeight(), 0, 255);
	VideoPlayerManager.setAlpha(a);
	VideoPlayerManager.draw(0,0,CANVAS_WIDTH,CANVAS_HEIGHT);

	//------------------------------------- TCP Client Manager-------------------------------------
	TcpClientManager.draw();

	if (calibrationMode)
	{
		KinectMapper.CanvasCalibrateFbo.draw(0, 0);
		drawKinectFbo();

	}
	calibrationGui.draw();
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	ParticleVisualsManager.keyPressed(key);
	TcpClientManager.keyPressed(key);
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
			KinectVisionFbo.begin();
			ofClear(0);
			KinectVisionFbo.end();
		}
		break;

	case 'v':
		ofLog() << "drawVideoPlayerManager : " << drawVideoPlayerManager;
		drawVideoPlayerManager != drawVideoPlayerManager;
		break;
	}
}

//--------------------------------------------------------------
//------------------------- Kinect -----------------------------
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
	bodyPosOnFloor.resize(MAX_PLAYERS);
	bodyPosOnScreen.resize(MAX_PLAYERS);
}

void ofApp::updateKinect2() {
	kinect.update();
	floorPlane = kinect.getBodySource()->getFloorClipPlane();
	floorTransform = glm::mat4(kinect.getBodySource()->getFloorTransform());
	string floorMsg = "[" + to_string(floorPlane.x) + "][" + to_string(floorPlane.y) + "][" + to_string(floorPlane.z) + "][" + to_string(floorPlane.w) + "]";
	KINECTNOTICELOG << "Floor Plane Vector: " << floorMsg << endl;
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
	KINECTNOTICELOG << "tracked body ID: ";
	for (auto& body : bodies) {
		if (body.tracked) {
			numBodiesTracked++;
			int bodyIdx = body.bodyId;
			bodyIdxTracked[bodyIdx] = true;
			ofVec3f refJoint = body.joints.at(REFJOINTTYPE).getPosition();
			bodyPositions[bodyIdx] = glm::vec3(refJoint.x, refJoint.y, refJoint.z);
			ofLogNotice() << "[" << bodyIdx << "]";

			glm::vec3& torsoJoint = bodyPositions[bodyIdx];
			glm::vec3 bodyOnFloor = projectedPointOntoPlane(bodyPositions[bodyIdx], floorPlane);
			glm::vec4 bodyPosVec4 = glm::vec4(bodyOnFloor.x, bodyOnFloor.y, bodyOnFloor.z, 1);
			glm::vec4 bodyPosOnHorizonOffset = floorTransform * bodyPosVec4;
			bodyPosOnFloor[bodyIdx] = cv::Point2f(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.z);
		}
	}

	cv::perspectiveTransform(bodyPosOnFloor, bodyPosOnScreen, KinectMapper.GetTransform());

	colorTex = kinect.getColorSource()->getTexture();
}

void ofApp::drawKinectFbo() {
	KinectVisionFbo.begin();
	ofClear(ofColor::white);
	ofBackground(0);
	kinect3DCam.begin();

	ofPushMatrix();
	ofDrawGrid(20, 20, true, true, true, true);
	ofScale(50, 50, 50);

	ofPushMatrix();
	glm::vec3 originOnFloor = projectedPointOntoPlane(glm::vec3(), floorPlane);
	ofTranslate(originOnFloor);
	ofRotateXDeg(90);
	ofRotateXRad(tiltAngle);
	ofRotateZRad(rollAngle);
	ofSetColor(ofColor::violet);
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
			bodyPosOnFloor[bodyIdx] = cv::Point2f(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.z);
			//bodyPosOnScreen[bodyIdx] = glm::vec2(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.z);
			
			
			//Red Box drawn along 3d floor plane
			ofDrawBox(bodyOnFloor, 0.2);
			//Blue Box drawn along horizontal plane, projected from RedBox
			ofSetColor(ofColor::aqua);
			//ofDrawBox(bodyPosOnScreen[bodyIdx].x, 0, bodyPosOnScreen[bodyIdx].y, 0.15);
			//Gold Box drawn from mat4 multiply Position on Floor
			ofSetColor(ofColor::gold);
			ofNoFill();
			ofDrawBox(glm::vec3(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.y, bodyPosOnHorizonOffset.z), 0.25);
		}
	}

	ofPopMatrix();
	kinect3DCam.end();
	KinectVisionFbo.end();

	
	ofPushMatrix();
	ofTranslate(0, ofGetHeight() * 1 / 3);
	KinectVisionFbo.draw(0, 0);
	ofPopMatrix();


}

glm::vec3 ofApp::projectedPointOntoPlane(glm::vec3 point, Vector4 plane) {
	glm::vec3 n = glm::vec3(plane.x, plane.y, plane.z);
	return (point - (glm::dot(n, point) + plane.w) * n);
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
		bodyPosInspect[i] = ofToString(bodyPositions[i].x) + ", " + ofToString(bodyPositions[i].y) + ", " + ofToString(bodyPositions[i].z);
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