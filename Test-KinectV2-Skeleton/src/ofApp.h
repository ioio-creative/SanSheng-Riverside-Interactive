#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"

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
		
		float pointDistFromPlane(ofVec3f point, Vector4 plane) {
			ofVec3f* normal = new ofVec3f(plane.x, plane.y, plane.z);
			return (normal->dot(point) + plane.w) / (normal->length());
		}

		ofVec3f projectedPointOntoPlane(ofVec3f point, Vector4 plane) {
			ofVec3f n = ofVec3f(plane.x, plane.y, plane.z);			
			return (point - (n.dot(point) + plane.w ) * n);
		}		


		//TODO: verify accuracy
		float camYZpointOntoScreenYNorm(ofVec3f point, float YZTiltRad) {
			return (point.z + point.y * tanf(YZTiltRad * PI/180.f));
		}

		float camXYpointOntoScreenXNorm(ofVec3f point, float XYRollRad) {
			return (point.x + point.y * tanf(XYRollRad * PI/180.f));
		}

		ofVec2f screenXYNormFromCamPointNorm(ofVec3f point, float YZTiltRad, float XYRollRad) {
			float screenY = camYZpointOntoScreenYNorm(point, YZTiltRad);
			float screenX = camXYpointOntoScreenXNorm(point, XYRollRad);
			return ofVec2f(screenX, screenY);
		}

		ofxKFW2::Device kinect;
		ICoordinateMapper* coordinateMapper;

		Vector4 floorPlane;
		float tiltAngle;
		float rollAngle;

		vector<ofVec3f> bodyPositions;

		ofImage bodyIndexImg, foregroundImg;
		ofTexture colorTex;
		vector<ofVec2f> colorCoords;
		int numBodiesTracked;
		bool bHaveAllStreams;

		ofEasyCam Cam3D;

		bool debugLog = false;
};
