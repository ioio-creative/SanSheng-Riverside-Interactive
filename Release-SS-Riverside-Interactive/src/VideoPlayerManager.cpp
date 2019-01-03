#include "VideoPlayerManager.h"

void VideoPlayerManager::setup(int w, int h) {
	//init
	ofSetVerticalSync(true);

	ofEnableAntiAliasing();
	ofEnableSmoothing();

	debugMode = false;

	screenW = w;
	screenH = h;

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
		if (i == 0) {
			vid[i].setLoopState(OF_LOOP_NORMAL);
		}
		else {
			vid[i].setLoopState(OF_LOOP_NONE);
		}
		vid[i].play();
		vid[i].setPaused(true);
#else
	vid[i].load("videos/1.hpv");
#endif
	}
videoFbo.allocate(screenW, screenH, GL_RGBA);

vid[0].setPaused(false);
vid[0].play();

currScene = 0;
}

void VideoPlayerManager::update() {

	//video
#ifdef USE_HPVPLAYER
	HPV::Update();
#else
	vid[currVidID].update();
#endif

	if (vid[currVidID].getCurrentFrame() == vid[currVidID].getTotalNumFrames() - 1) {
		ofLog() << "Movie Done";
	}

}


void VideoPlayerManager::draw(int x, int y, int w, int h) {

#ifdef USE_HPVPLAYER
	//fbo - Video
	videoFbo.begin();
	ofClear(255, 255, 255, 0);
	ofSetColor(255, 255, 255, vidAlpha);
	if (currScene == 0) {
		vid[0].draw(0, 0, videoFbo.getWidth(), videoFbo.getHeight());
	}
	else if (currScene == 1) {
		vid[1].draw(0, 0, videoFbo.getWidth(), videoFbo.getHeight());
	}
	else if (currScene == 2) {
			vid[1].draw(0, 0, videoFbo.getWidth(), videoFbo.getHeight());
			vid[2].draw(ofGetMouseX()-250, ofGetMouseY()-250, 500, 500);
			vid[2].draw(ofGetMouseX() , ofGetMouseY() - 250, 500, 500);
	}
	if (debugMode) {
		ofDrawBitmapStringHighlight(debugSS.str(), 10, ofGetHeight() / 2 * 3, ofColor(255, 255, 255), ofColor(0, 0, 0));
		//ofLog() << debugSS.str();
		debugSS.clear();
	}
	videoFbo.end();

	//fbo - draw

	ofSetColor(255, 255, 255, vidAlpha);
	videoFbo.draw(x, y, w, h);

#endif // USE_HPVPLAYER

}


void VideoPlayerManager::setAlpha(int a) {
	vidAlpha = a;
}

void VideoPlayerManager::keyReleased(int k) {
	switch (k) {
	case 'd':
		debugMode = !debugMode;
		break;

	case '0': //scene 0 begin
		currScene = 0;
		currVidID = 0;
		for (int i = 0; i < NUM_OF_VID; i++) {
			vid[i].stop();
		}
		vid[0].setPaused(false);
		vid[0].play();
		break;

	case '1': //scene 0 end

		vid[0].stop();
		vid[2].stop();

		break;

	case '2': //scene 1 begin
		currScene = 1;
		vid[1].setPaused(false);
		vid[1].play();
		break;

	case '3': //scene 1 end
		vid[0].stop();
		vid[1].stop();
		vid[2].stop();
		break;

	case '4': //scene 2 begin
		currScene = 2;
		vid[1].setPaused(false);
		vid[2].setPaused(false);
		vid[1].play();
		vid[2].play();
		ofLog() << "eye Scene triggered";
		break;

	case '5': //scene 2 end

		break;

	case '6': //scene 3 begin
		currScene = 3;
		break;

	case '7': //scene 3 end

		break;


	default:
		break;
	}

	ofLog() << "Video Curr Scene" << currScene << "key Triggered" << k;
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


float VideoPlayerManager::getVideoTime() {

	return vid[1].getDuration() * vid[1].getPosition()/1.0f;
}

float VideoPlayerManager::getVideoEndTime(){
	return vid[1].getDuration();
}
