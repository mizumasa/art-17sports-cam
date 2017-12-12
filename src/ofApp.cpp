#include "ofApp.h"
#include "ofMotionDetect.h"

using namespace ofxCv;
using namespace cv;


void ofApp::setup() {
    ofSetVerticalSync(true);
    ofBackground(0);

    detectWidth = CAPTURE_W;
    detectHeight = CAPTURE_H;
    detect.initAllocate(detectWidth,detectHeight);
#ifdef _USE_LIVE_VIDEO
    
#ifdef _USE_BLACKMAGIC
    cam.setup(BLACKMAGIC_W, BLACKMAGIC_H, BLACKMAGIC_FPS);
#else
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(WEB_CAM_W,WEB_CAM_H);
#endif
    
#else
    movie.loadMovie("MAH00013.MP4");
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

#ifdef _USE_BLACKMAGIC

    if(cam.update()){
        timer.tick();
        //b_CamStart=true;
        camPixels=cam.getColorPixels();
        camPixels.resize(CAPTURE_W, CAPTURE_H);
        camPixels.setImageType(OF_IMAGE_COLOR);
        perspective.setPixels(camPixels);
        perspective.update();
        colorImg.setFromPixels(camPixels);
        grayImage = colorImg;
        if(b_DrawImage){
            //detect.setPixels(grayImage.getPixels());
            detect.setColorPixels(colorImg.getPixels());
        }else{
            detect.setColorPixels(perspective.getPixels());
        }
        //camImg.setFromPixels(camPixels.getData(), BLACKMAGIC_W, BLACKMAGIC_H, OF_IMAGE_COLOR_ALPHA);
    }

#else
    
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
    
#endif
    
#else
    movie.update();
    if(movie.isFrameNew()) {
        ofPixels movieBuf;
        movieBuf = movie.getPixels();
        movieBuf.resize(CAPTURE_W,CAPTURE_H);
        perspective.setPixels(movieBuf);
        perspective.update();
        
        colorImg.setFromPixels(movieBuf);
        grayImage = colorImg;
        if(b_DrawImage){
            //detect.setPixels(grayImage.getPixels());
            detect.setColorPixels(colorImg.getPixels());
        }else{
            detect.setColorPixels(perspective.getPixels());
        }
    }
#endif
    detect.update();
    ofSetWindowTitle("cam:"+ofToString(detect.detectMode)+" mode:"+ofToString(ofGetFrameRate()));
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

void ofApp::keyReleased(int key) {
    if(key == ' ') detect.sendOSC(false);
    if(key == 'r') detect.rotateDetectOn(false);
}

void ofApp::keyPressed(int key) {
    detect.keyPressed(key);
    if(key == 'r') detect.rotateDetectOn(true);
    if(key == ' ') detect.sendOSC(true);
    if(key == 'b') detect.toggleImage();
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

