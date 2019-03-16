/*
 * heatmap for eye tracking by cvs
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Masahito H.
 *
 * Using addon: ofxHeatMap
 * Copyright (c) 2015 Hideyuki SAITO
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    fbo.allocate(ofGetWidth(), ofGetHeight());
    
    gazeFrame = 0;
    agentFrame = 0;
    
    string dummy;
    gazeData.open("[csvdata(subject's gaze) path]", ios::in);   //set csvdata(subject's gaze)
    generativeData.open("[csvdata(generative art video) path]", ios::in); //set csvdata(generative art video)
    getline(gazeData, dummy);
    getline(generativeData, dummy);
    
    heatmap.setup(ofGetWidth(), ofGetHeight(), 32);
    
    stepInit = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    if(gazeData.is_open() && generativeData.is_open()){
        int reload = generativeData.tellg();
        
        string oneShot;
        if(getline(generativeData, oneShot)){
            vector<string> oneData = split(oneShot, ',');
            agentFrame = stoi(oneData[4]);
        }
        else{
            agentFrame = -1;
        }
        
        generativeData.seekg(reload);
        
        
        string gazePoint;
        while(gazeFrame == agentFrame){
            if(getline(gazeData, gazePoint)){
                vector<string> gazeData = split(gazePoint, ',');
                int x = stof(gazeData[2]);
                int y = stof(gazeData[3]);
                gazeFrame = stoi(gazeData[1]);
            
                heatmap.addPoint(x, y);
                heatmap.update(OFX_HEATMAP_CS_SPECTRAL_MIXED);
                
                memoryGazeX = x;
                memoryGazeY = y;
            }
            else{
                break;
            }
        }
        
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(gazeData.is_open() && generativeData.is_open()){
        
        ofSetColor(255);
        
        fbo.begin();
        ofClear(0);
        
        for(int i = 0; i < AGENTS + 1; i++){
            string agentPoint;
            
            if(!getline(generativeData, agentPoint)){
                gazeData.close();
                generativeData.close();
                break;
            }
            
            vector<string> agentData = split(agentPoint, ',');
            int x = stoi(agentData[2]);
            int y = stoi(agentData[3]);
            
            if(agentData[0] == "agent"){
                ofDrawCircle(x, y, 2.5);
            }
            else if(agentData[0] == "region"){
                regionX = x;
                regionY = y;
                
                if(stepInit){
                    stepX = regionX;
                    stepY = regionY;
                    stepInit = false;
                }
                
                else{
                    if(stepX != regionX || stepY != regionY){
                        //heatmap.clear();
                        //heatmap.setup(ofGetWidth(), ofGetHeight(), 32);
                        heatmap.addPoint(memoryGazeX, memoryGazeY);
                        heatmap.update(OFX_HEATMAP_CS_SPECTRAL_MIXED);
                        
                        stepX = regionX;
                        stepY = regionY;
                    }
                }
                
            }
        }
        
        ofSetColor(255, 170, 0);
        ofDrawCircle(regionX, regionY, 5);
        ofSetColor(255);
        
        fbo.end();
        
        heatmap.draw(0, 0);
        fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
        
        stringstream ss;
        img.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        ss << "[file name]" << agentFrame << ".png";    //set file name
        img.save(ss.str());
    }
    
    ofDrawBitmapString(ofToString(ofGetFrameRate()) + "fps", 20, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}



vector<string> ofApp::split(string& input, char delimiter){
    istringstream stream(input);
    string field;
    vector<string> result;
    
    while(getline(stream, field, delimiter)){
        result.push_back(field);
    }
    
    return result;
}
