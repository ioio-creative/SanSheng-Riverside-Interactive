#include "ofApp.h"

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
	ofBackground(0);

	ofLogToConsole();
	ofSetLogLevel(OF_LOG_ERROR);
	ofSetWindowPosition(DEPTH_WIDTH, DEPTH_HEIGHT*2);

	kinect.open();
	kinect.initDepthSource();
	kinect.initColorSource();
	kinect.initInfraredSource();
	kinect.initBodySource();
	kinect.initBodyIndexSource();

	if (kinect.getSensor()->get_CoordinateMapper(&coordinateMapper) < 0){
		ofLogError() << "Could not acquire CoordinateMapper!";
	}

	bHaveAllStreams = false;

	bodyIndexImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
	foregroundImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);

	colorCoords.resize(DEPTH_WIDTH * DEPTH_HEIGHT);

	bodyIdxTracked.resize(MAX_PLAYERS);
	bodyPositions.resize(MAX_PLAYERS);
	bodyPosOnScreen.resize(MAX_PLAYERS);
}

void ofApp::setupGui() {
	ofBackground(0);
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();

	// Get pixel data
	auto& depthPix = kinect.getDepthSource()->getPixels();
	auto& bodyIndexPix = kinect.getBodyIndexSource()->getPixels();
	auto& colorPix = kinect.getColorSource()->getPixels();
	floorPlane = kinect.getBodySource()->getFloorClipPlane();
	string floorMsg = "[" + to_string(floorPlane.x) + "][" + to_string(floorPlane.y) + "][" + to_string(floorPlane.z) + "][" + to_string(floorPlane.w) + "]";
	ofLogNotice() << "Floor Plane Vector: " << floorMsg << endl;
	tiltAngle = atan(floorPlane.z / floorPlane.y);
	rollAngle = atan(floorPlane.x / floorPlane.y);


	// Make sure there's some data here, otherwise the cam probably isn't ready yet
	if (!depthPix.size() || !bodyIndexPix.size() || !colorPix.size()) {
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

	auto& bodies = kinect.getBodySource()->getBodies();

	for (auto& body : bodies) {
		if (body.tracked) {			
			numBodiesTracked++;
			int bodyIdx = body.bodyId;
			bodyIdxTracked[bodyIdx] = true;
			ofVec3f refJoint = body.joints.at(REFJOINTTYPE).getPosition();
			bodyPositions[bodyIdx] = glm::vec3(refJoint.x, refJoint.y, refJoint.z);
			ofLogError() << "tracked body ID: " << bodyIdx << endl;
		}
	}

	// Do the depth space -> color space mapping
	// More info here:
	// https://msdn.microsoft.com/en-us/library/windowspreview.kinect.coordinatemapper.mapdepthframetocolorspace.aspx
	// https://msdn.microsoft.com/en-us/library/dn785530.aspx
	coordinateMapper->MapDepthFrameToColorSpace(DEPTH_SIZE, (UINT16*)depthPix.getPixels(), DEPTH_SIZE, (ColorSpacePoint*)colorCoords.data());

	// Loop through the depth image
	for (int y = 0; y < DEPTH_HEIGHT; y++) {
		for (int x = 0; x < DEPTH_WIDTH; x++) {
			int index = (y * DEPTH_WIDTH) + x;
			bodyIndexImg.setColor(x, y, ofColor::white);
			foregroundImg.setColor(x, y, ofColor::white);

			// This is the check to see if a given pixel is inside a tracked  body or part of the background.
			// If it's part of a body, the value will be that body's id (0-5), or will > 5 if it's
			// part of the background
			// More info here:
			// https://msdn.microsoft.com/en-us/library/windowspreview.kinect.bodyindexframe.aspx
			float val = bodyIndexPix[index];
			if (val >= bodies.size()) {
				continue;
			}

			// Give each tracked body a color value so we can tell
			// them apart on screen
			ofColor c = ofColor::fromHsb(val * 255 / bodies.size(), 200, 255);
			bodyIndexImg.setColor(x, y, c);

			// For a given (x,y) in the depth image, lets look up where that point would be
			// in the color image
			ofVec2f mappedCoord = colorCoords[index];

			// Mapped x/y coordinates in the color can come out as floats since it's not a 1:1 mapping
			// between depth <-> color spaces i.e. a pixel at (100, 100) in the depth image could map
			// to (405.84637, 238.13828) in color space
			// So round the x/y values down to ints so that we can look up the nearest pixel
			mappedCoord.x = floor(mappedCoord.x);
			mappedCoord.y = floor(mappedCoord.y);

			// Make sure it's within some sane bounds, and skip it otherwise
			if (mappedCoord.x < 0 || mappedCoord.y < 0 || mappedCoord.x >= COLOR_WIDTH || mappedCoord.y >= COLOR_HEIGHT) {
				continue;
			}

			// Finally, pull the color from the color image based on its coords in
			// the depth image
			foregroundImg.setColor(x, y, colorPix.getColor(mappedCoord.x, mappedCoord.y));
		}
	}

	// Update the images since we manipulated the pixels manually. This uploads to the
	// pixel data to the texture on the GPU so it can get drawn to screen
	bodyIndexImg.update();
	foregroundImg.update();

	colorTex = kinect.getColorSource()->getTexture();
}

//--------------------------------------------------------------
void ofApp::draw(){
	/*ofSetColor(255);
	ofFill();
	bodyIndexImg.draw(0, 0);
	foregroundImg.draw(0, DEPTH_HEIGHT);*/

	ofSetColor(ofColor::gold);
	ofDrawEllipse(100,100,20,20);

	Cam3D.begin();
	ofPushMatrix();
	
	//ofDrawGrid(20, 20, true, true, true, true);
	ofScale(100, 100, 100);

	glm::vec3 originOnFloor = projectedPointOntoPlane(glm::vec3(), floorPlane);
	
	ofPushMatrix();
	ofTranslate(originOnFloor);
	ofRotateXDeg(90);
	ofRotateXRad(tiltAngle);
	ofRotateZRad(rollAngle);
	ofSetColor(255);
	ofFill();
	ofDrawPlane(0,0,0,5,5);
	ofPopMatrix();
		
	//draw origin projected on floor
	ofSetColor(0);
	ofFill();
	//ofDrawSphere(originOnFloor, 0.1);

	for (auto& body: kinect.getBodySource()->getBodies()){
		int bodyIdx = int(body.bodyId);
		if (body.tracked)
		{
			ofSetColor(ofColor::blue);
			ofFill();		
			for (auto& joint: body.joints)
			{
				//ofVec2f jointDepthPos = joint.second.getPositionInDepthMap();
				ofVec3f jointPos = joint.second.getPositionInWorld();
				ofDrawSphere(jointPos.x, jointPos.y, jointPos.z, 0.02);
			}
			
			ofSetColor(255, 50, 50);
			ofFill();
			
			glm::vec3& headJoint = bodyPositions[bodyIdx];
			glm::vec3 bodyOnFloor = projectedPointOntoPlane(headJoint, floorPlane);
			bodyPosOnScreen[bodyIdx] = screenXYNormFromCamPointOnPlane(bodyOnFloor, tiltAngle, rollAngle);
			ofDrawBox(bodyOnFloor, 0.2);
		}

	}
	ofScale(0.01, 0.01, 0.01);
	ofPopMatrix();
	Cam3D.end();
	//for (int i = 0; i < bodyPosOnScreen.size(); i++)
	//{
	//	float screenX = bodyPosOnScreen[i].x * ofGetWidth() + ofGetWidth()/2;
	//	float screenY = bodyPosOnScreen[i].y * ofGetHeight() + ofGetHeight()/2;
	//	ofLogError() << screenX << " || " << screenY << endl;
	//	ofDrawEllipse(int(bodyPosOnScreen[i].x), int(bodyPosOnScreen[i].y), 10, 10);
	//}
	ofSetColor(ofColor::silver);
	ofDrawEllipse(200, 200, 20, 20);
}

void ofApp::drawGui(ofEventArgs & args) {
	//colorTex.draw(0, 0, ofGetWidth(), ofGetHeight());

	ofSetColor(ofColor::crimson);
	ofNoFill();
	ofSetLineWidth(3);

	for (int i = 0; i < bodyPosOnScreen.size(); i++)
	{
		if (!bodyIdxTracked[i])
		{			
			continue;
		}
		float screenX = bodyPosOnScreen[i].x * CANVAS_WIDTH + CANVAS_WIDTH / 2;
		float screenY = bodyPosOnScreen[i].y * CANVAS_HEIGHT / 2;
		//ofLogError() << screenX << " || " << screenY << endl;
		ofLogError() << i << endl;
		ofDrawEllipse(screenX, screenY, 20, 20);
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'd')
	{
		debugLog = !debugLog;
		if (debugLog)
		{
			ofSetLogLevel(OF_LOG_NOTICE);
		}
		else
		{
			ofSetLogLevel(OF_LOG_ERROR);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
