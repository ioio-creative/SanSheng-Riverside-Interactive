#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLWindowSettings settings;
	settings.setSize(400, 1152);//(1920, 1080); //<-- final resolution
	settings.windowMode = OF_WINDOW;//OF_FULLSCREEN;
	settings.setGLVersion(4, 1);

	ofCreateWindow(settings);

	ofRunApp(new ofApp());
}
