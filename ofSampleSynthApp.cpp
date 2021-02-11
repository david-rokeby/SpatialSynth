#include "ofSampleSynthApp.h"
#include "cSampleSynth.hpp"

//--------------------------------------------------------------
void ofSampleSynthApp::setup() {
	ofBackground( 0 );
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate( 60 );

    synth = new cSampleSynth( 1024, 128, 1024 );

	synth -> Setup();
    synth -> LoadFromJSONSettings(string(""));
}

//--------------------------------------------------------------
void ofSampleSynthApp::update() {
	synth -> Update();
}

//--------------------------------------------------------------
void ofSampleSynthApp::draw() {
	ofBackground( 0 );
	synth -> Draw();
}

//--------------------------------------------------------------
void ofSampleSynthApp::exit() {
}

//--------------------------------------------------------------
void ofSampleSynthApp::keyPressed (int key) {
	synth -> HandleKeys( key );
}

//--------------------------------------------------------------
void ofSampleSynthApp::mouseDragged(int x, int y, int button)
{
	synth -> MouseDragged( x, y, button );
}

//--------------------------------------------------------------
void ofSampleSynthApp::mousePressed(int x, int y, int button)
{
    synth -> MousePressed( x, y, button );
}

//--------------------------------------------------------------
void ofSampleSynthApp::mouseReleased(int x, int y, int button)
{
	synth -> MouseReleased( x, y, button );
}

//--------------------------------------------------------------
void ofSampleSynthApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofSampleSynthApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofSampleSynthApp::windowResized(int w, int h)
{

}

void ofSampleSynthApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
    synth->MouseScrolled(scrollY);
}
