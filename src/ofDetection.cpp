#include "ofDetection.h"

using namespace ofxCv;
using namespace cv;

void ofDetection::setup() {
    
    grayImage.allocate(WEB_CAM_W,WEB_CAM_H);
    grayImageThr.allocate(WEB_CAM_W,WEB_CAM_H);

    gui.setup("panel");
    gui.add(radMin.set("radMin", 1,1,10));
    gui.add(radMax.set("radMax", 11,11,200));
    gui.add(th.set("contourFinder detection Thr", 200,0,255));             //cv側の検出のthreshold(2値化しないとき)
    gui.add(_th.set("Thr for binarization", 230,0,255));         //2値化のためのthreshold
    gui.add(hDetectThrS.set("H detect thr S", 128,0,255));
    gui.add(hDetectThrV.set("H detect thr V", 128,0,255));
    //gui.add(histscale.set("histscale", 10,3,50));
    //gui.add(detectSpeedMin.set("detectSpeedMin", 4,1,30));
    //gui.add(detectSpeedMax.set("detectSpeedMax", 30,1,30));
    
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
    //histscale.addListener(this, &ofDetection::valChanged);
    
    //osc sender
    sender.setup(HOST, PORT);
    i_ShowMode = 0;
    bHideGui = true;
    
    red.load("red.png");
    green.load("green.png");
    blue.load("blue.png");
    redl.load("redl.png");
    redn.load("redl.png");
    redd.load("redd.png");

    sender.setup(HOST, PORT);
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
    
    for(int i = 0; i < contourFinder.size(); i++) {
        ofPoint center = toOf(contourFinder.getCenter(i));
        ofxOscMessage m;
        m.setAddress("/mouse/position");
        m.addIntArg(center.x);
        m.addIntArg(center.y);
        sender.sendMessage(m);
    }
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
    if(!bHideGui){
        ofPixels colorPixels;
        colorPixels.allocate(colorImg.width, colorImg.height, OF_PIXELS_RGB);
        colorPixels = colorImg.getPixels();
        for(int i = 0; i < contourFinder.size(); i++) {
            ofPoint center = toOf(contourFinder.getCenter(i));
            cv::Rect rect = contourFinder.getBoundingRect(i);
            ofPixels detPixels;
            detPixels.allocate(rect.width, rect.height, OF_PIXELS_RGB);
            colorPixels.cropTo(detPixels,center.x - rect.width/2,center.y - rect.height/2,rect.width,rect.height);
            ofPushMatrix();
            ofTranslate(center.x - rect.width/2, center.y - rect.height/2);
            
            if(1){//raw color Image
                ofImage detImage;
                detImage.setFromPixels(detPixels);
                ofSetColor(255, 255, 255);
                detImage.draw(0, 0);
            }
            int width,height;
            width = rect.width;
            height = rect.height;
            rgb.allocate(width, height);
            hsv.allocate(width, height);
            h.allocate(width, height);
            s.allocate(width, height);
            v.allocate(width, height);

            green.resize(width, height);
            blue.resize(width, height);
            redl.resize(width, height);
            redd.resize(width, height);
            redn.resize(width, height);

            //rgb.setFromPixels(blue.getPixels());
            //cout << rgb.width << rgb.height << endl;
            //cout << detPixels.getWidth() << detPixels.getHeight() << endl;
            rgb.setFromPixels(detPixels);

            string msg ;
            cvCvtColor(rgb.getCvImage(), hsv.getCvImage(), CV_RGB2HSV);
            //h.setFromPixels(hsv.getPixels().getChannel(0));
            //s.setFromPixels(hsv.getPixels().getChannel(1));
            //v.setFromPixels(hsv.getPixels().getChannel(2));
            //hsv.resize(1, 1);
            //rgb.resize(1,1);
            ofPixels hsvPixels;
            hsvPixels = hsv.getPixels();
            unsigned char * buf = hsvPixels.getData();
            ofPixels rgbPixels;
            rgbPixels = rgb.getPixels();
            unsigned char * bufRGB = rgbPixels.getData();

            int aveH = 0;
            int countH = 0;
            for(int j = 0 ;j<width*height; j++){
                if(int(buf[j*3+1]) > hDetectThrS and int(buf[j*3+2])> hDetectThrV){
                    aveH += int(buf[j*3]);
                    countH++;
                }else{
                    bufRGB[j*3]=0;
                    bufRGB[j*3+1]=0;
                    bufRGB[j*3+2]=0;
                }
            }
            //ofPixels rgbPixels;
            //rgbPixels = rgb.getPixels();
            ofSetColor(255);
            rgb.setFromPixels(bufRGB,width, height);
            rgb.draw(0,0);
            //redl.draw(0,0);
            //cout << int(hsvPixels.getData()[0]) <<endl;
            bool b_DrawMsg = false;
            //H 0:Red 120:Blue
            if(countH > 10){
                float valH;
                valH = float(aveH)/countH;
                int score = 0;
                if(140 <= valH and valH <= 180){
                    score = 100;
                }else{
                    if(120 < valH and valH < 140){
                        score = 0;
                    }else{
                        score = 100 - (valH * 5 / 6);
                    }
                }
                msg = ofToString(score);
                //msg = ofToString(valH);
                b_DrawMsg = true;
            }
            //string msg = ofToString(hsvPixels.getData()[0])+":"+ofToString(hsvPixels.getData()[1])+":"+ofToString(hsvPixels.getData()[2]);
            //ofSetColor(int(rgbPixels.getData()[0]), int(rgbPixels.getData()[1]), int(rgbPixels.getData()[2]));
            //ofDrawRectangle(0, 0, 20, 20);
            //h.draw(0, 0);
            ofSetColor(255);
            //red.draw(0,0);
            int label = contourFinder.getLabel(i);
            //string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
            //string msg = ofToString(velocity.x)+":"+ofToString(velocity.y);
            ofScale(5, 5);
            ofSetColor(0, 255, 0);
            //ofVec2f velocity = toOf(contourFinder.getVelocity(i));
            //ofLine(0, 0, velocity.x, velocity.y);
            if(b_DrawMsg)ofDrawBitmapString(msg, 0, 0);
            ofPopMatrix();
            
            
        }
        gui.draw();
    }
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

