#pragma once

#include "ofMain.h"
#include "FloorParticle.h"
#include "FloorUser.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
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
    void updateAttractMode();
    void updateEmitMode();
    void prepareFloorCanvas();
    
    //timecode related
    bool sequenceGoing;
    float sequenceStartTimestamp;
    float lastTimestamp;
    void checkTimeTriggers();
};
