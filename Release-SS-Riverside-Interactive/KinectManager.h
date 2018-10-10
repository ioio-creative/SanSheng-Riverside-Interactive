#pragma once

#ifndef KinectManager_h
#define KinectManager_h
#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxCv.h"

class KinectManager
{
public:
	KinectManager();
	~KinectManager();

	//kinect
	ofxKFW2::Device kinect;
	ICoordinateMapper* coordinateMapper;

	Vector4 floorPlane;
	float tiltAngle;
	float rollAngle;
	glm::mat4 floorTransform;

	vector<bool> bodyIdxTracked;
	vector<glm::vec3> bodyPositions;
	vector<cv::Point2f> bodyPosOnFloor;
	vector<cv::Point2f> bodyPosOnScreen;

private:
	int numBodiesTracked;
	bool bHaveAllStreams;
};

#endif
