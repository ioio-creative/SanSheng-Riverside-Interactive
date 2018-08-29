#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	
	ofGLWindowSettings settings;
	settings.width = 1920;
	settings.height = 1080;
	settings.windowMode = OF_WINDOW;
	settings.setGLVersion(4, 1);

	ofCreateWindow(settings);

	//ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context


	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:


	ofRunApp(new ofApp());

}
