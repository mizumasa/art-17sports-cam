#include "ofDetection.h"

using namespace ofxCv;
using namespace cv;

void ofDetection::setup() {
    
    grayImage.allocate(WEB_CAM_W,WEB_CAM_H);
    grayImageThr.allocate(WEB_CAM_W,WEB_CAM_H);

    gui.setup("panel");
    gui.add(ptMin.set("grad top Min", 0,-100,255));
    gui.add(ptMax.set("grad top Max", 255,0,500));
    gui.add(pbMin.set("grad bottom Min", 0,-100,255));
    gui.add(pbMax.set("grad bottom Max", 255,0,500));

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
    setGradVarticle(ptMin,ptMax,pbMin,pbMax);

    // wait for half a frame before forgetting something
    contourFinder.getTracker().setPersistence(15);          //見失っても覚えててくれるパラメータ
    // an object can move up to 100 pixels per frame
    contourFinder.getTracker().setMaximumDistance(100);     //横振りとるために増やしました
    
    ptMin.addListener(this, &ofDetection::valChanged);
    ptMax.addListener(this, &ofDetection::valChanged);
    pbMin.addListener(this, &ofDetection::valChanged);
    pbMax.addListener(this, &ofDetection::valChanged);

    radMin.addListener(this, &ofDetection::valChanged);
    radMax.addListener(this, &ofDetection::valChanged);
    th.addListener(this, &ofDetection::valChanged);
    //histscale.addListener(this, &ofDetection::valChanged);
    
    //osc sender
    i_ShowMode = 0;
    bHideGui = true;
    
    red.load("red.png");
    green.load("green.png");
    blue.load("blue.png");
    redl.load("redl.png");
    redn.load("redl.png");
    redd.load("redd.png");

    sender.setup(HOST, PORT);
    senderToL.setup(HOST_TO_L, PORT_TO_L);
    senderToR.setup(HOST_TO_R, PORT_TO_R);
}

void ofDetection::areaChanged(int &val){
    bClearLog=true;
}

void ofDetection::valChanged(int &val){
    contourFinder.setMinAreaRadius(radMin);
    contourFinder.setMaxAreaRadius(radMax);
    contourFinder.setThreshold(th);
    setGradVarticle(ptMin,ptMax,pbMin,pbMax);
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

void ofDetection::setGradVarticle(int tMin,int tMax,int bMin,int bMax){
    i_tMin = tMin;
    i_tMax = tMax;
    i_bMin = bMin;
    i_bMax = bMax;
}


void ofDetection::update() {
    ofPixels grayImagePixel;
    int w,h;
    w = grayImage.getWidth();
    h = grayImage.getHeight();
    grayImagePixel.allocate(w, h, OF_IMAGE_GRAYSCALE);
    unsigned char *data = grayImagePixel.getData();
    unsigned char *data2 = grayImage.getPixels().getData();
    int _Min,_Max,_H;
    for(int i=0;i<h;i++){
        _Min = int((i_tMin * (h-i)+i_bMin * i)/h);
        _Max = int((i_tMax * (h-i)+i_bMax * i)/h);
        _H = _Max - _Min;
        for(int j=0;j<w;j++){
            data[i*w+j] = int(ofClamp( 255 * (data2[i*w+j] - _Min) / _H, 0, 255));
        }
    }
    grayImage.setFromPixels(grayImagePixel);
    grayImageThr = grayImage;
    grayImageThr.threshold(_th);
    contourFinder.findContours(grayImageThr);
    
}

void ofDetection::sendPosOSC(int x,int y){
    ofxOscMessage m;
    m.setAddress("/debug/position");
    char x_;
    char y_;
    x_ = (char)(int)(255*x/ofGetWidth());
    y_ = (char)(int)(255*y/ofGetHeight());
    m.addCharArg(x_);
    m.addCharArg(y_);
    sender.sendMessage(m);
}


void ofDetection::draw() {
    RectTracker& tracker = contourFinder.getTracker();
    ofSetColor(255);
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
                
                ofxOscMessage m;
                m.setAddress("/mouse/position");
                char x_;
                char y_;
                x_ = (char)(int)(255*center.x/ofGetWidth());
                y_ = (char)(int)(255*center.y/ofGetHeight());
                m.addCharArg(x_);
                m.addCharArg(y_);
                m.addCharArg((char)score);
                sender.sendMessage(m);
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

