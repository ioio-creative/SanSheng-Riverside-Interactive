#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(230);

	TcpClientManager.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
	TcpClientManager.update();
}

void ofApp::exit() {
	TcpClientManager.exit();

}

//--------------------------------------------------------------
void ofApp::draw(){

	TcpClientManager.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(ofKeyEventArgs & key){
	TcpClientManager.keyPressed(key);
}

