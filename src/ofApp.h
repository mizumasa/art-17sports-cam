#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "ofxGUI.h"
#include "ofDetection.h"




class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
#ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
#else
		  ofVideoPlayer 		movie;
#endif
    ofxCvColorImage         colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofDetection detect;
};
