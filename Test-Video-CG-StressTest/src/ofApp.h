#pragma once

#include "ofMain.h"

/* ======================================================
 //Settings
 ======================================================*/
#define USE_HPVPLAYER
#define USE_PARTICLE



#define NUM_OF_VID 2

 /* ======================================================
  //Includes
  ======================================================*/
#include "ofxJSON.h"
#ifdef USE_HPVPLAYER
#include "ofxHPVPlayer.h"
#endif
#ifdef USE_PARTICLE
#include "ofxGpuParticles.h"
#define _PROGRAMMABLE_RENDERER
#endif



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

		bool debugMode;

		//video
#ifdef USE_HPVPLAYER
		vector<ofxHPVPlayer> vid;
		int currVidID;
#else
		vector<ofVideoPlayer> vid;
#endif

		ofFbo CGFbo;
		ofFbo VideoFbo;

		//Settings
		ofxJSONElement settings;
		string dataPath;

		void saveSettings();
		void loadSettings();

		int w, h;

		//Timer
		float prevTime;
		float currTime;

#ifdef USE_PARTICLE
		// set any update uniforms in this function
		void onParticlesUpdate(ofShader& shader);

		ofxGpuParticles particles;
		ofEasyCam cam;
#endif

};

#ifdef USE_HPVPLAYER

#else

#endif


#ifdef USE_PARTICLE

#endif