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
    perspective.setup();
    b_DrawImage=true;
}


void ofApp::update() {
    
#ifdef _USE_LIVE_VIDEO
    vidGrabber.update();

    if (vidGrabber.isFrameNew()){
        perspective.setPixels(vidGrabber.getPixels());
        perspective.update();

        colorImg.setFromPixels(vidGrabber.getPixels());
        grayImage = colorImg;
        if(b_DrawImage){
            //detect.setPixels(grayImage.getPixels());
            detect.setColorPixels(colorImg.getPixels());
        }else{
            detect.setColorPixels(perspective.getPixels());
        }
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
    if(b_DrawImage){
        perspective.draw();
    }
    if(!(detect.bHideGui)){
        string info = "";
        info += "space key draw binarized image\n";
        info += " d key toggle perspective or raw image\n";
        info += " ";
        ofSetColor(255,255,255);
        ofDrawBitmapString(info, 20,ofGetHeight()/2);
    }
}

void ofApp::keyPressed(int key) {
    if(key == ' ') detect.toggleImage();
    if(key == 'g') detect.bHideGui = !detect.bHideGui;
    if(key == 's') detect.saveParam();
    if(key == 'l') detect.loadParam();
    //if(key == 'c') perspective.toggleImage();
    if(key == 'd') b_DrawImage = !b_DrawImage;
    if(key == 'f') ofToggleFullscreen();
}

void ofApp::mouseDragged(int x, int y, int button){
    if(b_DrawImage)perspective.mouseDragged(mouseX, mouseY, button);
}
void ofApp::mousePressed(int x, int y, int button){
    if(b_DrawImage)perspective.mousePressed(mouseX, mouseY, button);
    detect.sendPosOSC(x,y);
}
void ofApp::mouseReleased(int x, int y, int button){
    if(b_DrawImage)perspective.mouseReleased(mouseX, mouseY, button);
}

