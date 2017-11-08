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
    

    //Affine
    ofImage srcImg, dstImg;
    cv::Mat srcMat, dstMat;
    cv::Point2f srcPt[3], dstPt[3];
    bool isHolding;
    cv::Point2f *heldPt;
    //Affine

};
