#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "ofxGUI.h"

#define HOST "localhost"
//#define HOST "10.0.1.24"

#define PORT 12345

#define MAXSENDSIZE 400

#define _USE_LIVE_VIDEO
#define WEB_CAM_W 1280
#define WEB_CAM_H 720


class ofDetection{
public:
    void setup();
    void setPixels(ofPixels _pixels);
    void setColorPixels(ofPixels _pixels);
    ofPixels getPixels();
    void update();
    void draw();
    void toggleImage();
    void saveParam();
    void loadParam();
    
    bool bHideGui = false;

private:
    
    ofTrueTypeFont font;
    ofxOscSender sender;
    int sendhistory[MAXSENDSIZE];
    
    float threshold;
    ofxCv::ContourFinder contourFinder;
    
    ofxCvColorImage         colorImg;

    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayImageThr;
    void areaChanged(int &val);
    void valChanged(int &val);
    
    ofParameter<int>  radMin;
    ofParameter<int>  radMax;
    ofParameter<int>  th;
    ofParameter<int>  _th;
    ofParameter<int>  histscale;
    ofParameter<int>  detectSpeedMin;
    ofParameter<int>  detectSpeedMax;
    
    //検出エリアパラメータ 百分率
    /*
     ofParameter<int>  detectAreaTop;
     ofParameter<int>  detectAreaBottom;
     ofParameter<int>  detectAreaTopR;
     ofParameter<int>  detectAreaTopL;
     ofParameter<int>  detectAreaBottomR;
     ofParameter<int>  detectAreaBottomL;
    int areaTop,areaBottom,areaTopR,areaTopL,areaBottomR,areaBottomL;
    */
    
    ofxPanel gui;
    int i_ShowMode;
    bool bClearLog;
    
    ofxCvColorImage rgb, hsv;
    ofxCvGrayscaleImage r, g, b;
    ofxCvGrayscaleImage h, s, v;
    vector<float> histogramH, histogramS, histogramV;
    
    ofImage red,green,blue,redd,redl;
};
