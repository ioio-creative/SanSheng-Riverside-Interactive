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

	FloorParticleManager particleManager;
	FloorUserManager floorUserManager;

	ofFbo floorCanvas;

	int floorMode;  // 1 : repel | 2 : attract | 3 : emit
	float floorCanvasClearRate;

	void updateRepelMode();
	void updateAttractMode();
	void updateEmitMode();
	void prepareFloorCanvas();



};



#endif