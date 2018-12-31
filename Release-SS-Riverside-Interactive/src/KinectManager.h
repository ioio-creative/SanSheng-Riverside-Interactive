#pragma once

#ifndef KinectManager_h
#define KinectManager_h
#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxCv.h"

#include "KinectToFloorScreenMapper.h"

//Kinect sources dimensions
#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424
#define DEPTH_SIZE DEPTH_WIDTH * DEPTH_HEIGHT

#define COLOR_WIDTH 1920
#define COLOR_HEIGHT 1080

#define MAX_BODIES 6

#define BODYCOLOR_Color ofColor::

#define REFJOINTTYPE JointType_SpineShoulder
#define KINECTNOTICELOG ofLogNotice() << "[KINECT_MSG]"

#define KINECT3DVIEW_VERTICALDRAWOFFSET (ofGetHeight() / 2)

class KinectManager
{
public:
	KinectManager(int fboWidth, int fboHeight, int maxPlayers = MAX_BODIES);
	/*KinectManager(const KinectManager& manager) {
		kinect = manager.kinect;
		coordinateMapper = manager.coordinateMapper;

		monitorWidth = manager.monitorWidth;
		monitorHeight = manager.monitorHeight;
		players = manager.players;
		bHaveAllStreams = false;

		bodyIdxTracked.resize(players);
		bodyPositions.resize(players);
		bodyPosOnFloor.resize(players);
		bodyPosOnScreen.resize(players);
	};*/
	~KinectManager();

	//kinect
	ofxKFW2::Device kinect;
	ICoordinateMapper* coordinateMapper;
	//FBO
	ofFbo Kinect3dCamFbo;

	Vector4 floorPlane;
	glm::mat4 floorTransform;

	vector<bool> bodyIdxTracked;
	vector<glm::vec3> bodyPositions;
	vector<cv::Point2f> bodyPosOnFloor;
	vector<cv::Point2f> bodyPosOnScreen;

	ofParameterGroup bodyPosGuiGroup;
	ofxLabel selectedBodyLabel;
	ofParameter<int> refBodyIdx;
	vector<ofParameter<string>> bodyPosInspect;
	vector<ofParameter<bool>> refBodyIdxFlags;


	glm::vec3 projectedPointOntoPlane(glm::vec3& point, const Vector4& plane) const;

	void setup();
	void update(KinectToFloorScreenMapper& floorMapper, const int refBodyIdx);
	void draw();
	void windowResized(int w, int h);
	void keyReleased(int key);

	bool debugMode;

private:
	int monitorWidth, monitorHeight;
	int players;
	int numBodiesTracked;
	bool bHaveAllStreams;

	float tiltAngle;
	float rollAngle;

	ofTexture colorTex;
	ofEasyCam kinect3dCam;
	
};

#endif
