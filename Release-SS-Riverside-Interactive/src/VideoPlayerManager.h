#ifndef __VideoPlayerManager__
#define __VideoPlayerManager__

#define USE_HPVPLAYER

#define NUM_OF_VID 4

#ifdef USE_HPVPLAYER
#include "ofxHPVPlayer.h"
#endif

#include <stdio.h>
#include "ofMain.h"

class VideoPlayerManager
{

public:
	void setup(int w, int h);
	void update();
	void draw(int x1, int y1, int w1, int h1, vector<ofVec2f> bodyPos);
	void setAlpha(int a);
	void keyReleased(int k);
	void exit();

	float getVideoTime();
	float getVideoEndTime();
	stringstream debugSS;

	int vidAlpha;
	bool debugMode;

	int screenW, screenH;

	int currScene;

	bool isDelayTrigger;
	long nextTrigger;
	long showDelayMillis;
	void showBegin();

	//video
#ifdef USE_HPVPLAYER
	vector<ofxHPVPlayer> vid;
	int currVidID;
#else
	vector<ofVideoPlayer> vid;
#endif

	ofFbo videoFbo;
};



#endif