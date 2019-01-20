#ifndef __ParticleVisualsManager__
#define __ParticleVisualsManager__

#include "FloorParticle.h"
#include "FloorUser.h"

#include <stdio.h>
#include "ofMain.h"

class ParticleVisualsManager : public ofBaseApp
{

public:
	void setup(int w, int h);
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	int screenWidth;
	int screenHeight;

	ofVideoPlayer bgPlayer;

	//static ring related
	vector<FloorRing> staticRings;
	vector<ofVec2f> staticRingPos;
	bool isRenderStaticRings;
	void initStaticRings();

	//floor related
	int floorMode;  // 1 : repel | 2 : attract | 3 : emit
	FloorParticleManager particleManager;
	FloorUserManager floorUserManager;
	ofFbo floorCanvas;
	void updateRepelMode();
	void updateRepelModeReverse();
	void updateAttractMode();
	void updateEmitMode();
	void updateRepelModeWithRaise();
	void prepareFloorCanvas();

	//timecode related
	bool sequenceGoing;
	float sequenceStartTimestamp;
	float lastTimestamp;
	void checkTimeTriggers();

	bool debugMode;

	void setBodyPos(vector<ofVec2f> bodyPos);
	vector<ofVec2f> dynamicRingPos;

};



#endif