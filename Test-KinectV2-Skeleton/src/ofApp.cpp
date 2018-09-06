#include "ofApp.h"

#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424
#define DEPTH_SIZE DEPTH_WIDTH * DEPTH_HEIGHT

#define COLOR_WIDTH 1920
#define COLOR_HEIGHT 1080

#define MAX_PLAYERS 6

//--------------------------------------------------------------
void ofApp::setup(){
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

	bodyPositions.resize(MAX_PLAYERS+1);
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
	ofLogError() << "Floor Plane Vector: " << floorMsg << endl;

	// Make sure there's some data here, otherwise the cam probably isn't ready yet
	if (!depthPix.size() || !bodyIndexPix.size() || !colorPix.size()) {
		bHaveAllStreams = false;
		return;
	}
	else {
		bHaveAllStreams = true;
	}

	// Count number of tracked bodies
	numBodiesTracked = 0;
	auto& bodies = kinect.getBodySource()->getBodies();

	for (auto& body : bodies) {
		if (body.tracked) {
			numBodiesTracked++;
			//TODO: type casting
			int bodyIdx = body.bodyId;
			bodyPositions[bodyIdx] = body.joints.at(JointType_Head).getPosition();
			ofLogNotice() << "tracked body ID: " << bodyIdx << endl;
		}
		else
		{
			//bodyPositions.erase(bodyPositions.begin()+body.bodyId);
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
}

//--------------------------------------------------------------
void ofApp::draw(){
	/*ofSetColor(255);
	ofFill();
	bodyIndexImg.draw(0, 0);
	foregroundImg.draw(0, DEPTH_HEIGHT);*/

	Cam3D.begin();
	ofPushMatrix();
	
	ofScale(100, 100, 100);

	ofSetColor(ofColor::blue);
	ofFill();

	//ofDrawPlane(floorPlane.x, floorPlane.y, floorPlane.z, 5, 5);
	
	for (auto& body: kinect.getBodySource()->getBodies()){
		if (body.tracked)
		{
			for (auto& joint: body.joints)
			{
				//ofVec2f jointDepthPos = joint.second.getPositionInDepthMap();
				ofVec3f jointPos = joint.second.getPositionInWorld();
				ofDrawSphere(jointPos.x, jointPos.y, jointPos.z, 0.02);
			}
			
			ofSetColor(255, 0, 0);
			//ofFill();
			ofVec3f& headJoint = bodyPositions[int(body.bodyId)];
			ofVec3f headOnFloor = projectedPointOntoPlane(headJoint, floorPlane);
			ofDrawBox(headOnFloor, 0.1);
		}
	}

	ofPopMatrix();
	Cam3D.end();
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
