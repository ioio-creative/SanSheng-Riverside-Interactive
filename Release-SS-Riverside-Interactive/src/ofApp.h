#pragma once

#include "ofMain.h"

/* ======================================================
 //Settings
 ======================================================*/
//Pls check final resolution in main.cpp
#define NUM_OF_VID 2

 /* ======================================================
  //Includes
  ======================================================*/
#include "ofxJSON.h"
#include "ofxHPVPlayer.h"
#include "ofxKinectForWindows2.h"

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

		vector<bool> bodyIdxTracked;
		vector<glm::vec3> bodyPositions;
		vector<glm::vec2> bodyPosOnScreen;

		ofImage bodyIndexImg, foregroundImg;
		ofTexture colorTex;
		vector<ofVec2f> colorCoords;
		int numBodiesTracked;
		bool bHaveAllStreams;

		//fbo CG / video
		ofFbo CGFbo;
		ofFbo VideoFbo;
		ofFbo KinectCalibrateFbo;

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