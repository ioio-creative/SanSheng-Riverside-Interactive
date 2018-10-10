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

#define REFJOINTTYPE JointType_SpineShoulder
#define KINECTNOTICELOG ofLogNotice() << "[KINECT_MSG]"

#define KINECT3DVIEW_VERTICALDRAWOFFSET ofGetHeight() / 2

class KinectManager
{
public:
	KinectManager(const int canvasWidth, const int canvasHeight, 
		const int maxPlayers,
		const KinectToFloorScreenMapper& _floorMapper);
	~KinectManager();

	//kinect
	ofxKFW2::Device kinect;
	ICoordinateMapper* coordinateMapper;

	//kinect mapper to floor
	const KinectToFloorScreenMapper* floorMapper;

	Vector4 floorPlane;
	glm::mat4 floorTransform;

	vector<bool> bodyIdxTracked;
	vector<glm::vec3> bodyPositions;
	vector<cv::Point2f> bodyPosOnFloor;
	vector<cv::Point2f> bodyPosOnScreen;

	glm::vec3 projectedPointOntoPlane(glm::vec3& point, Vector4& plane) const;

	ofFbo Kinect3dCamFbo;

	void setup();
	void update();
	void draw();


private:
	int numBodiesTracked;
	bool bHaveAllStreams;

	float tiltAngle;
	float rollAngle;

	ofTexture colorTex;
	ofEasyCam kinect3dCam;
	
};

#endif
