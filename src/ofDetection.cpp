#include "ofDetection.h"

using namespace ofxCv;
using namespace cv;

void ofDetection::setup() {
    
    grayImage.allocate(WEB_CAM_W,WEB_CAM_H);
    
    gui.setup("panel");
    gui.add(radMin.set("radMin", 1,1,10));
    gui.add(radMax.set("radMax", 11,11,200));
    gui.add(th.set("th_detection", 200,0,255));             //cv側の検出のthreshold(2値化しないとき)
    gui.add(_th.set("th_binarization", 230,0,255));         //2値化のためのthreshold
    gui.add(histscale.set("histscale", 10,3,50));
    gui.add(detectSpeedMin.set("detectSpeedMin", 4,1,30));
    gui.add(detectSpeedMax.set("detectSpeedMax", 30,1,30));
    gui.add(detectAreaTop.set("detectAreaTop", 20,0,256));
    gui.add(detectAreaTopL.set("detectAreaTopL", 64,0,256));
    gui.add(detectAreaTopR.set("detectAreaTopR", 192,0,256));
    gui.add(detectAreaBottom.set("detectAreaBottom", 236,0,256));
    gui.add(detectAreaBottomL.set("detectAreaBottomL", 32,0,256));
    gui.add(detectAreaBottomR.set("detectAreaBottomR", 224,0,256));

    contourFinder.setMinAreaRadius(radMin);
    contourFinder.setMaxAreaRadius(radMax);
    contourFinder.setThreshold(th);
    // wait for half a frame before forgetting something
    contourFinder.getTracker().setPersistence(15);          //見失っても覚えててくれるパラメータ
    // an object can move up to 100 pixels per frame
    contourFinder.getTracker().setMaximumDistance(100);     //横振りとるために増やしました
    
    radMin.addListener(this, &ofDetection::valChanged);
    radMax.addListener(this, &ofDetection::valChanged);
    th.addListener(this, &ofDetection::valChanged);
    histscale.addListener(this, &ofDetection::valChanged);
    detectAreaTop.addListener(this, &ofDetection::areaChanged);
    detectAreaTopL.addListener(this, &ofDetection::areaChanged);
    detectAreaTopR.addListener(this, &ofDetection::areaChanged);
    detectAreaBottom.addListener(this, &ofDetection::areaChanged);
    detectAreaBottomL.addListener(this, &ofDetection::areaChanged);
    detectAreaBottomR.addListener(this, &ofDetection::areaChanged);
    
    //osc sender
    sender.setup(HOST, PORT);
    i_ShowMode = 0;
    bHideGui = true;
}

void ofDetection::areaChanged(int &val){
    bClearLog=true;
}

void ofDetection::valChanged(int &val){
    contourFinder.setMinAreaRadius(radMin);
    contourFinder.setMaxAreaRadius(radMax);
    contourFinder.setThreshold(th);
}

void ofDetection::setPixels(ofPixels _pixels){
    grayImage.setFromPixels(_pixels);
}
void ofDetection::setColorPixels(ofPixels _pixels){
    colorImg.setFromPixels(_pixels);
    grayImage = colorImg;
}

ofPixels ofDetection::getPixels(){
    return grayImage.getPixels();
}


void ofDetection::update() {
    grayImageThr = grayImage;
    grayImageThr.threshold(_th);
    contourFinder.findContours(grayImageThr);
}

void ofDetection::drawWaku(){
    int w;
    w = ofGetWidth();
    int h;
    h = ofGetHeight();
    ofSetColor(255, 255, 0);
    ofSetLineWidth(3);
    areaTop= (h*detectAreaTop)>>8;
    areaTopL= (w*detectAreaTopL)>> 8;
    areaTopR= (w*detectAreaTopR)>> 8;
    areaBottom= (h*detectAreaBottom)>>8;
    areaBottomR= (w*detectAreaBottomR)>> 8;
    areaBottomL= (w*detectAreaBottomL)>> 8;
    
    ofLine(  areaTopL, areaTop, areaTopR ,areaTop);
    ofLine(  areaTopL, areaTop, areaBottomL ,areaBottom);
    ofLine(  areaTopR, areaTop, areaBottomR ,areaBottom);
    ofLine(  areaBottomL, areaBottom, areaBottomR ,areaBottom);
}


void ofDetection::draw() {
    RectTracker& tracker = contourFinder.getTracker();
    ofSetColor(255);
#ifdef _USE_LIVE_VIDEO
    switch(i_ShowMode){
        case 0:
            grayImage.draw(0, 0);
            break;
        case 1:
            grayImageThr.draw(0, 0);
            break;
        default:
            break;
    }
#endif
    contourFinder.draw();
    if(!bHideGui) gui.draw();
    drawWaku();
}

void ofDetection::toggleImage(){
    i_ShowMode = (i_ShowMode + 1) %2;
}
void ofDetection::saveParam(){
    gui.saveToFile("settings.xml");
}

void ofDetection::loadParam(){
    gui.loadFromFile("settings.xml");
}

