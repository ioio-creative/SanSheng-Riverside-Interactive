#include "VideoPlayerManager.h"

void VideoPlayerManager::setup() {
	//init
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofEnableAntiAliasing();
	ofEnableSmoothing();

	screenW = ofGetScreenWidth();
	screenW = ofGetScreenWidth();
	screenH = ofGetScreenHeight();

	//video
	currVidID = 0;
#ifdef USE_HPVPLAYER
	HPV::InitHPVEngine();
	/* Create resources for new player */
	/* Alternatively, if you experience playback stutter, try to toggle double-buffering true/false
	 * Default: OFF
	 *
	 * hpvPlayer.setDoubleBuffered(true);
	 */
	for (int i = 0; i < NUM_OF_VID; i++) {
		vid.push_back(ofxHPVPlayer());
		vid[i].init(HPV::NewPlayer());
		ofLog() << i;
		/* Try to load file and start playback */
		vid[i].load("videos/" + ofToString(i) + ".hpv");
		vid[i].setLoopState(OF_LOOP_NONE);
		vid[i].play();
		vid[i].setPaused(true);
#else
	vid[i].load("videos/1.hpv");
#endif
	}
videoFbo.allocate(screenW, screenH, GL_RGBA);

vid[currVidID].setPaused(false);
vid[currVidID].play();


}


void VideoPlayerManager::update() {

	//video
#ifdef USE_HPVPLAYER
	HPV::Update();
#else
	vid[currVidID].update();
#endif

	if (vid[currVidID].getCurrentFrame() == vid[currVidID].getTotalNumFrames() - 1) {
		vid[currVidID].setPaused(true);
		vid[currVidID].seekToFrame(0);
		if (debugMode) {
			ofLog() << "Movie Done: " << currVidID;
		}

		currVidID++;

		if (currVidID >= NUM_OF_VID) {
			currVidID = 0;
		}

		if (debugMode) {
			ofLog() << "Movie Play: " << currVidID;
		}

		vid[currVidID].setPaused(false);
		vid[currVidID].play();
	}

}


void VideoPlayerManager::draw(int x, int y, int w, int h) {

#ifdef USE_HPVPLAYER
	//fbo - Video
	videoFbo.begin();
	ofClear(255, 255, 255, 0);
	ofSetColor(255, 255, 255, vidAlpha);
	vid[0].draw(0, 0, videoFbo.getWidth(), videoFbo.getHeight());
	videoFbo.end();

	//fbo - draw

	ofSetColor(255, 255, 255, vidAlpha);
		videoFbo.draw(x,y,w,h);
	
#endif // USE_HPVPLAYER

}


void VideoPlayerManager::setAlpha(int a) {
	vidAlpha = a;
}

//--------------------------------------------------------------
//-------------------------- video --------------------------
//--------------------------------------------------------------
#ifdef USE_HPVPLAYER
void VideoPlayerManager::exit() {
	/* Cleanup and destroy HPV Engine upon exit */
	HPV::DestroyHPVEngine();
}
#else

#endif

