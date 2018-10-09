#ifndef __VideoPlayerManager__
#define __VideoPlayerManager__

#define USE_HPVPLAYER

#define NUM_OF_VID 1

#ifdef USE_HPVPLAYER
#include "ofxHPVPlayer.h"
#endif

#include <stdio.h>
#include "ofMain.h"

class VideoPlayerManager
{

public:
	void setup();
	void update();
	void draw(int x, int y, int w, int h);
	void setAlpha(int a);
	void exit();


	int vidAlpha;
	bool debugMode;

	int screenW, screenH;

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