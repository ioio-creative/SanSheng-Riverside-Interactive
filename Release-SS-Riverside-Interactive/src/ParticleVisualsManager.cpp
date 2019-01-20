#include "ParticleVisualsManager.h"

extern string BG_VIDEO_PATH;

extern int P1X;
extern int P1Y;
extern int P2X;
extern int P2Y;
extern int P3X;
extern int P3Y;
extern int P4X;
extern int P4Y;
extern int P5X;
extern int P5Y;
extern int P6X;
extern int P6Y;

extern float TRIGGER_SCENE_1_START;
extern float TRIGGER_SCENE_1_END;
extern float TRIGGER_SCENE_2A_START;
extern float TRIGGER_SCENE_2A_END;
extern float TRIGGER_SCENE_2B_START;
extern float TRIGGER_SCENE_2B_END;
extern float TRIGGER_SCENE_2C_START;
extern float TRIGGER_SCENE_2C_END;
extern float TRIGGER_SCENE_3_START;
extern float TRIGGER_SCENE_3_END;

extern float MODE_REPEL_FORCE;
extern float MODE_REPEL_SCATTER;
extern float MODE_REPEL_DAMP;

extern float MODE_ATTRACT_SCATTER;
extern float MODE_ATTRACT_FORCE;
extern float MODE_ATTRACT_GRAVITY;
extern float MODE_ATTRACT_DAMP;

extern bool FIRST_USER_FOLLOW_CURSOR;

extern float PARTICLE_MIN_RAISE_SPEED;
extern float PARTICLE_MAX_RAISE_SPEED;

extern int FLOOR_CANVAS_WIDTH;
extern int FLOOR_CANVAS_HEIGHT;

//--------------------------------------------------------------
void ParticleVisualsManager::setup(int w, int h) {
	//default parameters
	floorMode = 1; // 1 : repel | 2 : attract | 3 : emit | 4 : repel with float
	isRenderStaticRings = false;
	sequenceGoing = false;
	sequenceStartTimestamp = -1;

	ofSetFrameRate(60);

	//init canvas
	floorCanvas.allocate(FLOOR_CANVAS_WIDTH, FLOOR_CANVAS_HEIGHT);
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

	//init bg video
	//bgPlayer.load(BG_VIDEO_PATH);
	//bgPlayer.setLoopState(OF_LOOP_NORMAL);
	//bgPlayer.play();

	//init static rings
	initStaticRings();

	//setup screenWidth and screenHeight variables instead of using ofGetWidth()/screenHeight, for testing in OF_FULLSCREEN
	screenHeight = h;
	screenWidth = screenHeight / (float)floorCanvas.getHeight() * floorCanvas.getWidth();
}

//--------------------------------------------------------------
void ParticleVisualsManager::initStaticRings()
{
	ofVec2f p1 = ofVec2f(P1X, P1Y);
	ofVec2f p2 = ofVec2f(P2X, P2Y);
	ofVec2f p3 = ofVec2f(P3X, P3Y);
	ofVec2f p4 = ofVec2f(P4X, P4Y);
	ofVec2f p5 = ofVec2f(P5X, P5Y);
	ofVec2f p6 = ofVec2f(P6X, P6Y);
	staticRingPos.push_back(p1);
	staticRingPos.push_back(p2);
	staticRingPos.push_back(p3);
	staticRingPos.push_back(p4);
	staticRingPos.push_back(p5);
	staticRingPos.push_back(p6);

	for (int i = 0; i < staticRingPos.size(); i++)
	{
		FloorRing r;
		r.update(staticRingPos[i].x, staticRingPos[i].y);
		staticRings.push_back(r);
	}
}

//--------------------------------------------------------------
void ParticleVisualsManager::checkTimeTriggers()
{
	if (sequenceGoing)
	{
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_1_START && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_1_START)
		{
			keyPressed('1');
		}
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_1_END && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_1_END)
		{
			keyPressed('2');
		}
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_2A_START && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_2A_START)
		{
			keyPressed('[');    // place dummy player on cursor
			keyPressed('3');
		}
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_2A_END && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_2A_END)
		{
			keyPressed('4');
		}
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_2B_START && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_2B_START)
		{
			keyPressed('3');
		}
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_2B_END && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_2B_END)
		{
			keyPressed('4');
		}
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_2C_START && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_2C_START)
		{
			keyPressed('3');
		}
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_2C_END && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_2C_END)
		{
			keyPressed('4');
		}
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_3_START && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_3_START)
		{
			keyPressed('5');
		}
		if (ofGetElapsedTimef() - sequenceStartTimestamp >= TRIGGER_SCENE_3_END && lastTimestamp - sequenceStartTimestamp < TRIGGER_SCENE_3_END)
		{
			keyPressed('6');
		}
		lastTimestamp = ofGetElapsedTimef();
	}
}

//--------------------------------------------------------------
void ParticleVisualsManager::update() {

	checkTimeTriggers();

	//bgPlayer.update();

	for (int i = 0; i < staticRings.size(); i++)
	{
		staticRings[i].update(staticRingPos[i].x, staticRingPos[i].y, isRenderStaticRings);
	}

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

	else if (floorMode == 4)  //convert users into umbrella shape repellers
	{
		updateRepelModeWithRaise();
	}
	else if (floorMode == 5)  //repel reverse
	{
		updateRepelModeReverse();
	}




	particleManager.update();
	floorUserManager.update();

	if (FIRST_USER_FOLLOW_CURSOR && floorUserManager.floorUsers.size() > 0)
	{
		float ss = screenHeight / (float)floorCanvas.getHeight();        //scale floorCanvas for development
		floorUserManager.floorUsers[0].pos.x = (ofGetMouseX() - screenWidth / 2) / ss;
		floorUserManager.floorUsers[0].pos.y = (ofGetMouseY() - screenHeight / 2) / ss;
	}
}

//--------------------------------------------------------------
void ParticleVisualsManager::prepareFloorCanvas()
{
	floorCanvas.begin();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//render static rings (if there are ring particles alive)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (int i = 0; i < staticRings.size(); i++)
	{
		staticRings[i].draw(floorUserManager.ringSprite);
	}

	particleManager.drawFloor();
	floorUserManager.draw(0);
	floorCanvas.end();

	//reset blending mode
	glDisable(GL_BLEND);
	ofEnableAlphaBlending();
}

//--------------------------------------------------------------
void ParticleVisualsManager::draw() {
	//ofBackground(30);       //set dark grey background for visible boundary in full screen mode
	ofDisableAntiAliasing();
	ofSetVerticalSync(false);
	prepareFloorCanvas();

	glPushMatrix();
	float ss = screenHeight / (float)floorCanvas.getHeight();        //scale floorCanvas for development
	glScalef(ss, ss, ss);

	//////////////////////////////////
	//  RENDERING BACKGROUND VIDEO  //
	//////////////////////////////////
	//ofSetColor(255);
	//bgPlayer.draw(0, 0, floorCanvas.getWidth(), floorCanvas.getHeight());  //draw video background (stretching video to floor size, not maintaining aspect ratio)

	ofSetColor(255);
	floorCanvas.draw(0, 0);
	glPopMatrix();

	ofSetColor(255, 0, 0);
	//ofDrawBitmapString("press SPACE to start sequence,\npress '[' to add user under cursor, \ncycle F2 and F6 to call attract / repel modes", 20, 20);    //show instructions
	//ofDrawBitmapString("FRAMERATE = " + ofToString(ofGetFrameRate()), 20, 80);       //show framerate
	//ofDrawBitmapString("NUM PARTICLES = " + ofToString(particleManager.particles.size()), 20, 100);       //show num particles
	if (sequenceGoing)
	{
		//	ofDrawBitmapString("TIMECODE = " + ofToString(ofGetElapsedTimef() - sequenceStartTimestamp), 20, 100);       //show timecode
	}
}

//--------------------------------------------------------------
void ParticleVisualsManager::updateRepelMode()
{
	vector <ofVec3f> repellers;
	for (int i = 0; i < floorUserManager.floorUsers.size(); i++)
	{
		repellers.push_back(ofVec3f(floorUserManager.floorUsers[i].pos));
	}
	particleManager.massRepel(repellers, MODE_REPEL_FORCE, MODE_REPEL_SCATTER);

	SCENE_GRAVITY = abs(SCENE_GRAVITY);
	particleManager.damp(MODE_REPEL_DAMP);
}

//--------------------------------------------------------------
void ParticleVisualsManager::updateRepelModeReverse()
{
	vector <ofVec3f> repellers;
	for (int i = 0; i < floorUserManager.floorUsers.size(); i++)
	{
		repellers.push_back(ofVec3f(floorUserManager.floorUsers[i].pos));
	}
	particleManager.massRepel(repellers, MODE_REPEL_FORCE, MODE_REPEL_SCATTER);
	SCENE_GRAVITY = -abs(SCENE_GRAVITY);
	//particleManager.gravity(ofVec3f(0, 0, 0));
	particleManager.damp(MODE_REPEL_DAMP);
}

//--------------------------------------------------------------
void ParticleVisualsManager::updateRepelModeWithRaise()
{
	vector <ofVec3f> repellers;
	for (int i = 0; i < floorUserManager.floorUsers.size(); i++)
	{
		repellers.push_back(ofVec3f(floorUserManager.floorUsers[i].pos));
	}
	particleManager.massRepel(repellers, MODE_REPEL_FORCE, MODE_REPEL_SCATTER);

	particleManager.raise(PARTICLE_MIN_RAISE_SPEED, PARTICLE_MAX_RAISE_SPEED);
	particleManager.damp(MODE_REPEL_DAMP);
}

//--------------------------------------------------------------
void ParticleVisualsManager::updateAttractMode()
{
	vector <ofVec3f> attractions;
	for (int i = 0; i < floorUserManager.floorUsers.size(); i++)
	{
		attractions.push_back(ofVec3f(floorUserManager.floorUsers[i].pos));
	}
	particleManager.massAttract(attractions, MODE_ATTRACT_FORCE, MODE_ATTRACT_SCATTER);
	particleManager.gravity(ofVec3f(0, MODE_ATTRACT_GRAVITY, 0));
	particleManager.damp(MODE_ATTRACT_DAMP);
}

//--------------------------------------------------------------
void ParticleVisualsManager::updateEmitMode()    //unsed at the moment ?
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
	SCENE_GRAVITY = abs(SCENE_GRAVITY);
	particleManager.gravity(ofVec3f(0, -0.2, 0));
	particleManager.damp(1.05);
}

//--------------------------------------------------------------
void ParticleVisualsManager::keyPressed(int key) {

	////////////////////////////
	//  PRESS SPACE TO START  //
	////////////////////////////
	if (key == ' ')                      //start & end sequence
	{
		sequenceGoing = !sequenceGoing;
		if (sequenceGoing)
		{
			lastTimestamp = sequenceStartTimestamp = ofGetElapsedTimef();
		}
		else
		{
			lastTimestamp = sequenceStartTimestamp = -1;
		}
	}

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
		particleManager.addTopEmitter(1, 5);
	}
	if (key == OF_KEY_F3)    //set floor mode to 3, remove all emitters
	{
		floorMode = 3;
		particleManager.clearEmitter();
	}
	if (key == OF_KEY_F4)
	{
		//place dummy circles
		isRenderStaticRings = true;
	}
	if (key == OF_KEY_F5)
	{
		//place dummy circles
		isRenderStaticRings = false;
	}
	if (key == OF_KEY_F6)
	{
		//triggering repel mode with raise
		floorMode = 4;
		particleManager.clearEmitter();
	}
	if (key == OF_KEY_F7)
	{
		//triggering repel mode with raise
		floorMode = 5;
		particleManager.clearEmitter();
		particleManager.addBottomEmitter(5);

	}
	if (key == '[')          //add 1 floor user at cursor for development
	{
		float ss = screenHeight / (float)floorCanvas.getHeight();
		floorUserManager.addFloorUser((ofGetMouseX() - screenWidth / 2) / ss, (ofGetMouseY() - screenHeight / 2) / ss);
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
	if (key == '0')
	{
		keyPressed(OF_KEY_F6);          
		keyPressed(OF_KEY_BACKSPACE);
		keyPressed(OF_KEY_F4);       
	}
	if (key == '1')
	{
		keyPressed(OF_KEY_F5);          //end scene 1
	}
	if (key == '2')
	{
		//keyPressed(OF_KEY_F2);          //start scene 2
	}
	if (key == '3')
	{
		keyPressed(OF_KEY_F3);
	}
	if (key == '4')
	{
		keyPressed(OF_KEY_F6);          //start scene 3
	}
	if (key == '5')
	{
		keyPressed(OF_KEY_F1);
	}
	if (key == '6')
	{
		keyPressed(OF_KEY_F7);   //end scene 3
	}
	if (key == '7')
	{
		keyPressed(OF_KEY_BACKSPACE);   //end scene 3
	}
	if (key == '8')
	{
	}
}

void ParticleVisualsManager::setBodyPos(vector<ofVec2f> bodyPos) {

	for (int i = 0; i < floorUserManager.floorUsers.size(); i++)
	{
		if (bodyPos.size() > i ) {
			float ss = screenHeight / (float)floorCanvas.getHeight();        //scale floorCanvas for development
			floorUserManager.floorUsers[i].pos.x = (bodyPos[i].x - screenWidth / 2) / ss;
			floorUserManager.floorUsers[i].pos.y = (bodyPos[i].y - screenHeight / 2) / ss;
		}
	}
}
//--------------------------------------------------------------
void ParticleVisualsManager::keyReleased(int key) {

}

//--------------------------------------------------------------
void ParticleVisualsManager::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ParticleVisualsManager::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ParticleVisualsManager::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ParticleVisualsManager::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ParticleVisualsManager::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ParticleVisualsManager::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ParticleVisualsManager::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ParticleVisualsManager::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ParticleVisualsManager::dragEvent(ofDragInfo dragInfo) {

}
