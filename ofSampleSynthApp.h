#pragma once

#include "ofBaseApp.h"
class cSampleSynth;

class ofSampleSynthApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
    void mouseScrolled(int x, int y, float scrollX, float scrollY);

	cSampleSynth *synth;
};
