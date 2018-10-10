#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxJSON.h"
#include "ofxGui.h"

#include "TcpClientManager.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(ofKeyEventArgs & key);
		void exit();

		void keyReleased(int key) {};
		void mouseMoved(int x, int y) {};
		void mousePressed(int x, int y, int button) {};
		void mouseDragged(int x, int y, int button) {};
		void mouseReleased(int x, int y, int button) {};
		void mouseEntered(int x, int y) {};
		void mouseExited(int x, int y) {};
		void windowResized(int w, int h) {};
		void dragEvent(ofDragInfo dragInfo) {};
		void gotMessage(ofMessage msg) {};

		TcpClientManager TcpClientManager;


};

