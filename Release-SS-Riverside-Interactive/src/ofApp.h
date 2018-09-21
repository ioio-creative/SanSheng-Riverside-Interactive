#pragma once
#include "ofMain.h"

/* ======================================================
//Includes
======================================================*/
#include "ofxCv.h"
#include "ofxJSON.h"
#include "ofxHPVPlayer.h"
#include "ofxKinectForWindows2.h"
#include "ofxGui.h"

/* ======================================================
//Settings
======================================================*/
//Pls check final resolution in main.cpp
#define NUM_OF_VID 2

//Kinect sources dimensions
#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424
#define DEPTH_SIZE DEPTH_WIDTH * DEPTH_HEIGHT

#define COLOR_WIDTH 1920
#define COLOR_HEIGHT 1080

#define MAX_PLAYERS 6
#define REFJOINTTYPE JointType_SpineShoulder

#define CANVAS_WIDTH 1200 /3
#define CANVAS_HEIGHT 3456 /3

#define KINECTAREA_WIDTH CANVAS_WIDTH
#define KINECTAREA_HEIGHT CANVAS_HEIGHT / 2

#define KINECTNOTICELOG ofLogNotice() << "[KINECT_MSG]"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyReleased(int key);

		//unused
		void keyPressed(int key) {};
		void mouseMoved(int x, int y) {};
		void mouseDragged(int x, int y, int button) {};
		void mousePressed(int x, int y, int button) {};
		void mouseReleased(int x, int y, int button) {};
		void mouseEntered(int x, int y) {};
		void mouseExited(int x, int y) {};
		void windowResized(int w, int h) {};
		void dragEvent(ofDragInfo dragInfo) {};
		void gotMessage(ofMessage msg) {};

		//basic variables
		bool debugMode;
		int w, h;
		
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

		ofTexture colorTex;
		vector<ofVec2f> colorCoords;
		int numBodiesTracked;
		bool bHaveAllStreams;
		
		glm::vec3 projectedPointOntoPlane(glm::vec3 point, Vector4 plane);

		void setupKinect2();
		void updateKinect2();
		void drawKinectFbo();

		//kinect 3D to 2D calibration
		cv::Point2f kinectFourCorners[4] = 
		{ 
			cv::Point2f(0, 0),
			cv::Point2f(10, 0),
			cv::Point2f(10, 10),
			cv::Point2f(0, 10)
		};
		cv::Point2f canvasFourCorners[4] =
		{
			cv::Point2f(100, 100),
			cv::Point2f(KINECTAREA_WIDTH - 100, 100),
			cv::Point2f(KINECTAREA_WIDTH - 100, KINECTAREA_HEIGHT - 100),
			cv::Point2f(100, KINECTAREA_HEIGHT - 100)
		};
		cv::Mat camToScreenTransform;

		ofImage target;
		ofParameter<glm::vec3> bodyPos;
		ofxPanel calibrationGui;

		bool calibrationMode = false;


		//fbo CG / video
		ofFbo CGFbo;
		ofFbo VideoFbo;
		ofFbo KinectVisionFbo;
		ofEasyCam kinect3DCam;
		ofFbo CanvasCalibrateFbo;
		void setupCavasCalibrateFbo();
		//video player
		vector<ofxHPVPlayer> vid;
		int currVidID;
		void setupVideoPlayer();

		//Settings
		ofxJSONElement settings;
		string dataPath;

		void saveSettings();
		void loadSettings();

};


/* ======================================================
 //TODO
 ======================================================*/

 /* ======================================================
  //Known Issue
  ======================================================*/