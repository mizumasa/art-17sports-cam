#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "ofxGUI.h"
#include "ofDetection.h"
#include "ofPerspective.h"


class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

    int detectWidth;
    int detectHeight;
#ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
#else
		  ofVideoPlayer 		movie;
#endif
    ofxCvColorImage         colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofDetection detect;
    ofPerspective perspective;

    
    bool b_DrawImage;
    
};
