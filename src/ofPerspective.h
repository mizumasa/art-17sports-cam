#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGUI.h"

class ofPerspective{
public:
    void setup();
    void setPixels(ofPixels _pixels);
    ofPixels getPixels();
    void update();
    void draw();

    //void saveParam();
    //void loadParam();
    
    bool bHideGui = false;

private:
    ofxPanel gui;
};
