#include "ofApp.h"
#include "ofMotionDetect.h"

using namespace ofxCv;
using namespace cv;


void ofApp::setup() {
    ofSetVerticalSync(true);
    ofBackground(0);
    
#ifdef _USE_LIVE_VIDEO
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(WEB_CAM_W,WEB_CAM_H);
#else
    movie.loadMovie("video2.mov");
    movie.setVolume(0.0);
    movie.play();
#endif
    
    colorImg.allocate(WEB_CAM_W,WEB_CAM_H);
    grayImage.allocate(WEB_CAM_W,WEB_CAM_H);

    detect.setup();
}


void ofApp::update() {
    
#ifdef _USE_LIVE_VIDEO
    vidGrabber.update();
    if (vidGrabber.isFrameNew()){
        colorImg.setFromPixels(vidGrabber.getPixels());
        grayImage = colorImg;
        detect.setPixels(grayImage.getPixels());
    }
#else
    movie.update();
    if(movie.isFrameNew()) {
        blur(movie, 10);
    }
#endif
    detect.update();
}

void ofApp::draw() {
    ofSetColor(255);
    detect.draw();
}

void ofApp::keyPressed(int key) {
    if(key == ' ') detect.toggleImage();
    if(key == 'g') detect.bHideGui = !detect.bHideGui;
    if(key == 's') detect.saveParam();
    if(key == 'l') detect.loadParam();
}
