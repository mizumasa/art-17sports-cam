#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGUI.h"

#define PERSPECTIVE_CAM_W 1280
#define PERSPECTIVE_CAM_H 720

class ofPerspective{
public:
    void setup();
    void setPixels(ofPixels _pixels);
    ofPixels getPixels();
    void update();
    void draw();
    void drawGUI();
    void toggleImage();
    //void saveParam();
    //void loadParam();
    void mouseDragged(float mouseX, float mouseY, int button);
    void mousePressed(float mouseX, float mouseY, int button);
    void mouseReleased(float mouseX, float mouseY, int button);

    
    
    bool bHideGui = false;

private:
    
    ofImage grayImage;
    
    ofxPanel gui;
    
    int camWidth;
    int camHeight;
    int camDrawScaleW;
    int camDrawScaleH;

    ofImage screenCapCam;
    
    ofImage srcImg, dstImg;
    cv::Mat srcMat, dstMat;
    cv::Point2f srcPt[4], dstPt[4];
    bool isHolding;
    cv::Point2f *heldPt;

    bool b_DrawImage;
};
