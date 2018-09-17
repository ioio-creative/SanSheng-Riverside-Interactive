#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void setupGui();
		void update();
		void draw();
		void drawGui(ofEventArgs & args);
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		float pointDistFromPlane(glm::vec3 point, Vector4 plane) {
			glm::vec3 normal = glm::vec3(plane.x, plane.y, plane.z);
			return (glm::dot(normal, point) + plane.w) / (glm::length(normal));
		}

		glm::vec3 projectedPointOntoPlane(glm::vec3 point, Vector4 plane) {
			glm::vec3 n = glm::vec3(plane.x, plane.y, plane.z);
			return (point - (glm::dot(n, point) + plane.w ) * n);
		}		
		

		ofxKFW2::Device kinect;
		ICoordinateMapper* coordinateMapper;

		Vector4 floorPlane;
		float tiltAngle;
		float rollAngle;
		ofMatrix4x4 floorTransform;

		vector<bool> bodyIdxTracked;
		vector<glm::vec3> bodyPositions;
		vector<glm::vec2> bodyPosOnScreen;

		ofImage bodyIndexImg, foregroundImg;
		ofTexture colorTex;
		vector<ofVec2f> colorCoords;
		int numBodiesTracked;
		bool bHaveAllStreams;

		ofEasyCam Cam3D;

		ofxPanel gui;
		

		bool debugLog = false;
};
