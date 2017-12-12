#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "ofxGUI.h"
#include "ofDetection.h"
#include "ofPerspective.h"
#include "ofxBlackMagic.h"

class RateTimer {
protected:
    float lastTick, averagePeriod, smoothing;
    bool secondTick;
public:
    RateTimer() :
    smoothing(.9) {
        reset();
    }
    void reset() {
        lastTick = 0, averagePeriod = 0, secondTick = false;
    }
    void setSmoothing(float smoothing) {
        this->smoothing = smoothing;
    }
    float getFramerate() {
        return averagePeriod == 0 ? 0 : 1 / averagePeriod;
    }
    void tick() {
        float curTick = ofGetElapsedTimef();
        if(lastTick == 0) {
            secondTick = true;
        } else {
            float curDiff = curTick - lastTick;;
            if(secondTick) {
                averagePeriod = curDiff;
                secondTick = false;
            } else {
                averagePeriod = ofLerp(curDiff, averagePeriod, smoothing);
            }
        }
        lastTick = curTick;
    }
};

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
#ifdef _USE_BLACKMAGIC
    ofxBlackMagic cam;
    RateTimer timer;
    ofPixels camPixels;
    ofImage camImg;
    bool b_CamStart;
#else
    ofVideoGrabber 		vidGrabber;
#endif
#else
		  ofVideoPlayer 		movie;
#endif
    

    
    ofxCvColorImage         colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofDetection detect;
    ofPerspective perspective;

    
    bool b_DrawImage;
    
};
