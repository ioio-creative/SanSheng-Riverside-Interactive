#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	
//	ofGLWindowSettings settings;
//
//
//	settings.setSize(CANVAS_WIDTH, CANVAS_HEIGHT);//<-- final resolution
//
//#ifdef EXHIBITION
//	settings.windowMode = OF_FULLSCREEN;
//#else
//	settings.windowMode = OF_WINDOW;
//#endif
//
//	settings.setGLVersion(4, 1);
//
//	ofCreateWindow(settings);
	
	ofSetupOpenGL(400, 1150, OF_WINDOW);

	ofRunApp(new ofApp());
}
