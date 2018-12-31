#ifndef __VideoPlayerManager__
#define __VideoPlayerManager__

#define USE_HPVPLAYER

#define NUM_OF_VID 3

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
	void draw(int x, int y, int w, int h);
	void setAlpha(int a);
	void keyReleased(int k);
	void exit();

	stringstream debugSS;

	int vidAlpha;
	bool debugMode;

	int screenW, screenH;

	int currScene;

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