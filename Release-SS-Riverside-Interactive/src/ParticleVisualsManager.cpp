#include "ParticleVisualsManager.h"


//--------------------------------------------------------------
void ParticleVisualsManager::setup(int w, int h) {
	//init canvas
	floorCanvas.allocate(w, h);
	floorCanvas.begin();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	floorCanvas.end();

	//init particle and floor manager
	particleManager.canvasWidth = floorCanvas.getWidth();
	particleManager.canvasHeight = floorCanvas.getHeight();
	floorUserManager.canvasWidth = floorCanvas.getWidth();
	floorUserManager.canvasHeight = floorCanvas.getHeight();
	floorUserManager.loadTextures();

	floorMode = 1; // 1 : repel | 2 : attract | 3 : emit

	floorCanvasClearRate = 0.2; //(0.0 ~ 1.0)
}


//--------------------------------------------------------------
void ParticleVisualsManager::update() {

	if (floorMode == 1)  //convert users into umbrella shape repellers
	{
		updateRepelMode();
	}

	else if (floorMode == 2) //convert users into attracting points
	{
		updateAttractMode();
	}

	else if (floorMode == 3) //convert users into particle emitters
	{
		updateEmitMode();
	}

	particleManager.update();
	floorUserManager.update();
}

void ParticleVisualsManager::prepareFloorCanvas()
{
	floorCanvas.begin();
	//partially clear fbo, leaving trails
	ofSetColor(0, floorCanvasClearRate * 255);
	ofRect(0, 0, floorCanvas.getWidth(), floorCanvas.getHeight());

	particleManager.drawFloor();
	floorUserManager.draw(floorMode);
	floorCanvas.end();

	//reset blending mode
	glDisable(GL_BLEND);
	ofEnableAlphaBlending();
}

//--------------------------------------------------------------
void ParticleVisualsManager::draw() {
	ofBackground(0,0);
	prepareFloorCanvas();

	ofSetColor(255, 255, 255, canvasAlpha);
	glPushMatrix();
	float ss = ofGetHeight() / (float)floorCanvas.getHeight();  
	//scale floorCanvas for development
	glScalef(ss, ss, ss);
	floorCanvas.draw(0, 0);
	glPopMatrix();
}


//--------------------------------------------------------------
void ParticleVisualsManager::updateRepelMode()
{
	float scatter = 500;
	vector <ofVec3f> repellers;
	for (int i = 0; i < floorUserManager.floorUsers.size(); i++)
	{
		repellers.push_back(ofVec3f(floorUserManager.floorUsers[i].pos));
	}
	particleManager.massRepel(repellers, 3500, scatter);
	particleManager.damp(0.98);
}

//--------------------------------------------------------------
void ParticleVisualsManager::updateAttractMode()
{
	float scatter = 0;
	vector <ofVec3f> attractions;
	for (int i = 0; i < floorUserManager.floorUsers.size(); i++)
	{
		attractions.push_back(ofVec3f(floorUserManager.floorUsers[i].pos));
	}
	particleManager.massAttract(attractions, 0.1, scatter);
	particleManager.gravity(ofVec3f(0, 0.5, 0));
	particleManager.damp(0.9);
}

//--------------------------------------------------------------
void ParticleVisualsManager::updateEmitMode()
{
	if (particleManager.emitters.size() != floorUserManager.floorUsers.size())
	{
		particleManager.clearEmitter();
		for (int i = 0; i < floorUserManager.floorUsers.size(); i++)
		{
			particleManager.addEmitter();
		}
	}

	for (int i = 0; i < floorUserManager.floorUsers.size(); i++)
	{
		particleManager.emitters[i].pos = floorUserManager.floorUsers[i].pos;
	}

	particleManager.gravity(ofVec3f(0, -0.5, 0));
	particleManager.damp(0.98);
}

//--------------------------------------------------------------
void ParticleVisualsManager::keyPressed(int key) {
	//key controls for development
	if (key == OF_KEY_F1)    //set floor mode to 1, place 5 emitters to top
	{
		floorMode = 1;
		particleManager.clearEmitter();
		particleManager.addTopEmitter(5);
	}
	if (key == OF_KEY_F2)    //set floor mode to 2, place 1 emitter to top
	{
		floorMode = 2;
		particleManager.clearEmitter();
		particleManager.addTopEmitter(1);
	}
	if (key == OF_KEY_F3)    //set floor mode to 3, remove all emitters
	{
		floorMode = 3;
		particleManager.clearEmitter();
	}
	if (key == '[')          //add 1 floor user at cursor for development
	{
		float ss = ofGetHeight() / (float)floorCanvas.getHeight();
		floorUserManager.addFloorUser((ofGetMouseX() - ofGetWidth() / 2) / ss, (ofGetMouseY() - ofGetHeight() / 2) / ss);
	}
	if (key == ']')          //remove all floor users
	{
		floorUserManager.clearFloorUser();
	}
	if (key == OF_KEY_BACKSPACE) //remove all emitters
	{
		particleManager.clearEmitter();
	}


	//sequencing for demo
	if (key == '1')
	{
		keyPressed(OF_KEY_F1);          //start mode 1
	}
	if (key == '2')
	{
		keyPressed(OF_KEY_BACKSPACE);   //end mode 1
	}
	if (key == '3')
	{
		keyPressed(OF_KEY_F2);          //start mode 2
	}
	if (key == '4')
	{
		keyPressed(OF_KEY_BACKSPACE);   //end mode 2
	}
	if (key == '5')
	{
		keyPressed(OF_KEY_F3);          //start mode 3
	}
	if (key == '6')
	{
		keyPressed(']');                //end mode 3
	}
}

void ParticleVisualsManager::setAlpha(int alpha) {
	canvasAlpha = alpha;
}
