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
        gui.draw();
        ofPixels colorPixels;
        colorPixels = colorImg.getPixels();
        for(int i = 0; i < contourFinder.size(); i++) {
            ofPoint center = toOf(contourFinder.getCenter(i));
            cv::Rect rect = contourFinder.getBoundingRect(i);
            ofPixels detPixels;
            colorPixels.cropTo(detPixels,center.x - rect.width/2,center.y - rect.height/2,rect.width,rect.height);
            ofPushMatrix();
            ofTranslate(center.x - rect.width/2, center.y - rect.height/2);
            
            if(0){//raw color Image
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

            rgb.setFromPixels(detPixels);
            string msg ;
            cvCvtColor(rgb.getCvImage(), hsv.getCvImage(), CV_BGR2HSV);
            //h.setFromPixels(hsv.getPixels().getChannel(0));
            //s.setFromPixels(hsv.getPixels().getChannel(1));
            //v.setFromPixels(hsv.getPixels().getChannel(2));
            //hsv.resize(1, 1);
            //rgb.resize(1,1);
            ofPixels hsvPixels;
            hsvPixels = hsv.getPixels();
            unsigned char * buf = hsvPixels.getData();
            int aveH = 0;
            int countH = 0;
            for(int j = 0 ;j<width*height; j++){
                if(int(buf[j*3+1]) > 128 and int(buf[j*3+2])> 128){
                    aveH += int(buf[j*3]);
                    countH++;
                }
            }
            //ofPixels rgbPixels;
            //rgbPixels = rgb.getPixels();
            //cout << int(hsvPixels.getData()[0]) <<endl;
            if(countH > 10){
                msg = ofToString(float(aveH)/countH);
            }
            //string msg = ofToString(hsvPixels.getData()[0])+":"+ofToString(hsvPixels.getData()[1])+":"+ofToString(hsvPixels.getData()[2]);
            //ofSetColor(int(rgbPixels.getData()[0]), int(rgbPixels.getData()[1]), int(rgbPixels.getData()[2]));
            //ofDrawRectangle(0, 0, 20, 20);
            //h.draw(0, 0);
            
            int label = contourFinder.getLabel(i);
            //string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
            ofVec2f velocity = toOf(contourFinder.getVelocity(i));
            //string msg = ofToString(velocity.x)+":"+ofToString(velocity.y);
            ofScale(5, 5);
            ofSetColor(0, 255, 0);
            ofLine(0, 0, velocity.x, velocity.y);
            ofDrawBitmapString(msg, 0, 0);
            ofPopMatrix();
        }

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

