#include "ofMain.h"
#include "ofApp.h"


string BG_VIDEO_PATH = "ioio_testing_bg_3.mp4";


int FLOOR_CANVAS_WIDTH = 1165;
int FLOOR_CANVAS_HEIGHT = 3828;

//dummy circle positions
int P1X = 170;
int P1Y = 2800;
int P2X = 366;
int P2Y = 3200;
int P3X = 490;
int P3Y = 6200;
int P4X = 650;
int P4Y = 6200;
int P5X = 772;
int P5Y = 3200;
int P6X = 970;
int P6Y = 2800;

//time triggers
float TRIGGER_SCENE_1_START = 0.1;
float TRIGGER_SCENE_1_END = 10;
float TRIGGER_SCENE_2A_START = 15;
float TRIGGER_SCENE_2A_END = 20;
float TRIGGER_SCENE_2B_START = 30;
float TRIGGER_SCENE_2B_END = 40;
float TRIGGER_SCENE_2C_START = 50;
float TRIGGER_SCENE_2C_END = 60;
float TRIGGER_SCENE_3_START = 70;
float TRIGGER_SCENE_3_END = 90;

//repel mode settings
float MODE_REPEL_FORCE = 15000;
float MODE_REPEL_SCATTER = 7000;
float MODE_REPEL_DAMP = 0.98;
float PARTICLE_PATH_SCATTER = 0.8;

float SCENE_GRAVITY_1 = 0.4;
float SCENE_GRAVITY_2 = 4.0;
float SCENE_GRAVITY = SCENE_GRAVITY_1;

//attract mode settings
float MODE_ATTRACT_SCATTER = 0;
float MODE_ATTRACT_FORCE = 0.1;
float MODE_ATTRACT_GRAVITY = -0.3;
float MODE_ATTRACT_DAMP = 0.9;

//emitter settings
float EMITTER_EMIT_SPEED = 25;
float EMITTER_LIFE_MIN = 300;
float EMITTER_LIFE_MAX = 500;
float PARTICLE_HUE_MIN = 30;
float PARTICLE_HUE_MAX = 40;
float PARTICLE_SATUATION = 180;
float PARTICLE_BRIGHTNESS = 250;
float PARTICLE_RADIUS = 20;
float PARTICLE_RANGE = 400;
float PARTICLE_SPEED_RANGE = 5;
string PARTICLE_SPRITE_PATH = "lightTexture/flake_sprite.png";
int MAX_PARTICLES = 6000;

float PARTICLE_MIN_RAISE_SPEED = 0.2;       //new parameter for raise
float PARTICLE_MAX_RAISE_SPEED = 0.4;       //new parameter for raise
float PARTICLE_RAISE_Z_CLIPPING = 1000;     //new parameter for raise

//ring settings
float RING_PARTICLE_RADIUS_MIN = 1;
float RING_PARTICLE_RADIUS_MAX = 3;
float RING_PARTICLE_RADIUS_GROW_SPEED = 0.01;
float RING_PARTICLE_RADIUS_DECAY_SPEED = 0.995;
float RING_ORBIT_SPEED_MIN = 0.001;
float RING_ORBIT_SPEED_MAX = 0.01;
float RING_ORBIT_RADIUS_1 = 140;
float RING_ORBIT_RADIUS_2 = 20;
float RING_ORBIT_RADIUS = RING_ORBIT_RADIUS_1;
float ORBIT_RADIUS_SCATTER = 2;
float ORBIT_RADIUS_SCATTER_SPEED_MIN = 0.5;
float ORBIT_RADIUS_SCATTER_SPEED_MAX = 2.5;
int RING_PARTICLE_LIFE_MIN = 100;
int RING_PARTICLE_LIFE_MAX = 100;
float RING_PARTICLE_HUE_MIN = 30;
float RING_PARTICLE_HUE_MAX = 40;
float RING_PARTICLE_SATUATION = 180;
float RING_PARTICLE_BRIGHTNESS = 255;
float RING_PARTICLE_VIRTUAL_ALPHA = 355;
string RING_PARTICLE_SPRITE_PATH = "lightTexture/ring_sprite.png";
int MAX_USERS = 6;
int MAX_PARTICLE_PER_RING = 300;
bool FIRST_USER_FOLLOW_CURSOR = false;

//========================================================================

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;

	settings.multiMonitorFullScreen = false;

#ifdef LANDSCAPE_MODE
	#ifdef EXHIBITION
	//ofSetupOpenGL(CANVAS_HEIGHT,CANVAS_WIDTH,OF_WINDOW);
	settings.setSize(CANVAS_HEIGHT, CANVAS_WIDTH);
	#else
	//ofSetupOpenGL(CANVAS_HEIGHT, CANVAS_WIDTH, OF_WINDOW);
	settings.setSize(CANVAS_HEIGHT, CANVAS_WIDTH);
	#endif

#else
	#ifdef EXHIBITION
//ofSetupOpenGL(CANVAS_WIDTH, CANVAS_HEIGHT, OF_FULLSCREEN);
	settings.setSize(CANVAS_WIDTH, CANVAS_HEIGHT);
	#else
//	ofSetupOpenGL(CANVAS_WIDTH, CANVAS_HEIGHT, OF_WINDOW);
	settings.setSize(CANVAS_WIDTH, CANVAS_HEIGHT);
	#endif

#endif

	//settings.setSize(CANVAS_HEIGHT, CANVAS_WIDTH);
	settings.setPosition(ofVec2f(0, 0));
	settings.resizable = true;
	settings.windowMode = OF_WINDOW;
	//settings.windowMode = OF_FULLSCREEN;


	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	shared_ptr<ofApp> mainApp(new ofApp);

	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();


	//ofRunApp(new ofApp());
}
