#include "ofPerspective.h"

void ofPerspective::setup() {
    bHideGui = true;
}

void ofPerspective::setPixels(ofPixels _pixels){
    grayImage.setFromPixels(_pixels);
}

ofPixels ofPerspective::getPixels(){
    return grayImage.getPixels();
}


void ofPerspective::update() {
}

void ofPerspective::draw() {
}

/*void ofDetection::saveParam(){
    gui.saveToFile("settings.xml");
}

void ofDetection::loadParam(){
    gui.loadFromFile("settings.xml");
}*/

