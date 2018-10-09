#ifndef __VideoPlayerManager__
#define __VideoPlayerManager__

#define USE_HPVPLAYER

#define NUM_OF_VID 2

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
	void draw();
	void exit();

	bool debugMode;

	int w, h;

	//video
#ifdef USE_HPVPLAYER
	vector<ofxHPVPlayer> vid;
	int currVidID;
#else
	vector<ofVideoPlayer> vid;
#endif



	ofFbo VideoFbo;
};



#endif