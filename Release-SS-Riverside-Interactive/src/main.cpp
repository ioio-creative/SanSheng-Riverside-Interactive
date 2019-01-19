#include "ofMain.h"
#include "ofApp.h"


string BG_VIDEO_PATH = "ioio_testing_bg_3.mp4";


int FLOOR_CANVAS_WIDTH = 1200;
int FLOOR_CANVAS_HEIGHT = 3456;

//dummy circle positions
int P1X = 150;
int P1Y = 2100;
int P2X = 300;
int P2Y = 2400;
int P3X = 450;
int P3Y = 2700;
int P4X = 750;
int P4Y = 2700;
int P5X = 900;
int P5Y = 2400;
int P6X = 1050;
int P6Y = 2100;

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
float MODE_REPEL_FORCE = 3500;
float MODE_REPEL_SCATTER = 500;
float MODE_REPEL_DAMP = 0.98;
float PARTICLE_PATH_SCATTER = 0.8;
float SCENE_GRAVITY = 0.2;

//attract mode settings
float MODE_ATTRACT_SCATTER = 0;
float MODE_ATTRACT_FORCE = 0.1;
float MODE_ATTRACT_GRAVITY = -0.3;
float MODE_ATTRACT_DAMP = 0.9;

//emitter settings
float EMITTER_EMIT_SPEED = 3;
float EMITTER_LIFE_MIN = 300;
float EMITTER_LIFE_MAX = 500;
float PARTICLE_HUE_MIN = 30;
float PARTICLE_HUE_MAX = 40;
float PARTICLE_SATUATION = 180;
float PARTICLE_BRIGHTNESS = 155;
float PARTICLE_RADIUS = 40;
float PARTICLE_RANGE = 400;
float PARTICLE_SPEED_RANGE = 5;
string PARTICLE_SPRITE_PATH = "lightTexture/flake_sprite.png";
int MAX_PARTICLES = 4000;

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
float RING_ORBIT_RADIUS = 100;
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




#ifdef	LANDSCAPE_MODE
#ifdef	EXHIBITION
	ofSetupOpenGL(CANVAS_HEIGHT, CANVAS_WIDTH, OF_FULLSCREEN);
#endif
	ofSetupOpenGL(CANVAS_HEIGHT, CANVAS_WIDTH, OF_WINDOW);
#else
#ifdef	EXHIBITION
	ofSetupOpenGL(CANVAS_WIDTH, CANVAS_HEIGHT, OF_FULLSCREEN);
#endif
	ofSetupOpenGL(CANVAS_WIDTH, CANVAS_HEIGHT, OF_WINDOW);
#endif



	ofRunApp(new ofApp());
}
