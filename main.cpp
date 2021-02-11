#include "ofAppRunner.h"
#include "ofSampleSynthApp.h"
#include "ofWindowSettings.h"

int main() {
    ofSetupOpenGL(1440, 880, OF_WINDOW);
    ofRunApp(new ofSampleSynthApp());
}

