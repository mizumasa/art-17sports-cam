#include "ofPerspective.h"

using namespace ofxCv;
using namespace cv;


void ofPerspective::setup() {
    bHideGui = true;
    

    b_DrawImage = true;
    
    camDrawScaleW=1;
    camDrawScaleH=1;
    camWidth = 400;
    camHeight = 300;
    camWidth = PERSPECTIVE_CAM_W;
    camHeight = PERSPECTIVE_CAM_H;

    srcPt[0] = Point2f(100, 100);
    srcPt[1] = Point2f(100, 300);
    srcPt[2] = Point2f(400, 300);
    srcPt[3] = Point2f(400, 100);
    dstPt[0] = Point2f(camWidth/4, 0);
    dstPt[1] = Point2f(camWidth/4, camHeight/2);
    dstPt[2] = Point2f(camWidth*3/4, camHeight/2);
    dstPt[3] = Point2f(camWidth*3/4, 0);
    
    srcPt[0] = Point2f(100, 100);
    srcPt[1] = Point2f(400, 100);
    srcPt[2] = Point2f(400, 400);
    srcPt[3] = Point2f(100, 400);
    dstPt[0] = Point2f(200, 200);
    dstPt[1] = Point2f(500, 300);
    dstPt[2] = Point2f(500, 400);
    dstPt[3] = Point2f(200, 500);

    srcPt[0] = Point2f(400, 00);
    srcPt[1] = Point2f(880, 00);
    srcPt[2] = Point2f(1080, 600);
    srcPt[3] = Point2f(200, 600);
    dstPt[0] = Point2f(0, 0);
    dstPt[1] = Point2f(camWidth,0);
    dstPt[2] = Point2f(camWidth, camHeight);
    dstPt[3] = Point2f(0, camHeight);

    isHolding = false;
    
}

void ofPerspective::setPixels(ofPixels _pixels){
    srcImg.setFromPixels(_pixels);
}

ofPixels ofPerspective::getPixels(){
    return dstImg.getPixels();
}


void ofPerspective::update() {
    srcMat=toCv(srcImg);
    
    Mat perspectiveMat = getPerspectiveTransform(srcPt, dstPt);
    warpPerspective(srcMat, dstMat, perspectiveMat, srcMat.size());
    
    //Mat affineMat = getAffineTransform(srcPt, dstPt);
    //warpAffine(srcMat, dstMat, affineMat, srcMat.size());
    
    toOf(dstMat, dstImg);

}

void ofPerspective::draw() {
    ofSetColor(255);
    srcImg.update();
    srcImg.draw(camWidth*3/4,0,camWidth/4,camHeight/4);
    dstImg.update();
    
    if(b_DrawImage){
        dstImg.draw(camWidth*3/4, camHeight/4,camWidth/4,camHeight/4);
    }
    else{
        dstImg.draw(0, 0, camWidth,camHeight);
    }
    drawGUI();
}


void ofPerspective::drawGUI(){
    ofSetColor(128);
    ofDrawLine(srcPt[0].x, srcPt[0].y, dstPt[0].x, dstPt[0].y);
    ofDrawLine(srcPt[1].x, srcPt[1].y, dstPt[1].x, dstPt[1].y);
    ofDrawLine(srcPt[2].x, srcPt[2].y, dstPt[2].x, dstPt[2].y);
    ofDrawLine(srcPt[3].x, srcPt[3].y, dstPt[3].x, dstPt[3].y);
    
    
    ofSetColor(255, 0, 0);
    ofNoFill();
    ofBeginShape();
    for(int i = 0; i < 4; i++){
        ofVertex(srcPt[i].x, srcPt[i].y);
    }
    ofEndShape(true);
    //    o(srcPt[0].x, srcPt[0].y, srcPt[1].x, srcPt[1].y, srcPt[2].x, srcPt[2].y);
    for(int i = 0; i < 4; i++){
        ofDrawEllipse(srcPt[i].x, srcPt[i].y, 20, 20);
    }
    ofSetColor(0, 0, 255);
    ofNoFill();
    ofBeginShape();
    for(int i = 0; i < 4; i++){
        ofVertex(dstPt[i].x, dstPt[i].y);
    }
    ofEndShape(true);
    for(int i = 0; i < 4; i++){
        ofDrawEllipse(dstPt[i].x, dstPt[i].y, 20, 20);
    }
    
    /*
    ofSetColor(128);
    ofDrawLine(srcPt[0].x, srcPt[0].y, dstPt[0].x, dstPt[0].y);
    ofDrawLine(srcPt[1].x, srcPt[1].y, dstPt[1].x, dstPt[1].y);
    ofDrawLine(srcPt[2].x, srcPt[2].y, dstPt[2].x, dstPt[2].y);
    
    
    ofSetColor(255, 0, 0);
    ofNoFill();
    ofDrawTriangle(srcPt[0].x, srcPt[0].y, srcPt[1].x, srcPt[1].y, srcPt[2].x, srcPt[2].y);
    for(int i = 0; i < 3; i++){
        ofDrawEllipse(srcPt[i].x, srcPt[i].y, 20, 20);
    }
    ofSetColor(0, 0, 255);
    ofNoFill();
    ofDrawTriangle(dstPt[0].x, dstPt[0].y, dstPt[1].x, dstPt[1].y, dstPt[2].x, dstPt[2].y);
    for(int i = 0; i < 3; i++){
        ofDrawEllipse(dstPt[i].x, dstPt[i].y, 20, 20);
    }
    ofSetColor(255);
     */
    
}
/*void ofDetection::saveParam(){
    gui.saveToFile("settings.xml");
}

void ofDetection::loadParam(){
    gui.loadFromFile("settings.xml");
}*/
//--------------------------------------------------------------

void ofPerspective::mouseDragged(float mouseX, float mouseY, int button){
    if(isHolding){
        heldPt->x = mouseX*camDrawScaleW;
        heldPt->y = mouseY*camDrawScaleH;
    }
}

//--------------------------------------------------------------
void ofPerspective::mousePressed(float mouseX, float mouseY, int button){
    for(int i = 0; i < 4; i++){
        if(ofDist(mouseX*camDrawScaleW, mouseY*camDrawScaleH, srcPt[i].x, srcPt[i].y) < 20){
            isHolding = true;
            heldPt = &srcPt[i];
            return;
        }
    }
    for(int i = 0; i < 4; i++){
        if(ofDist(mouseX*camDrawScaleW, mouseY*camDrawScaleH, dstPt[i].x, dstPt[i].y) < 20){
            isHolding = true;
            heldPt = &dstPt[i];
            return;
        }
    }
}

//--------------------------------------------------------------
void ofPerspective::mouseReleased(float mouseX, float mouseY, int button){
    isHolding = false;
}
void ofPerspective::toggleImage(){
    b_DrawImage = !b_DrawImage;
}

