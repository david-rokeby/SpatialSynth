//
//  cSoundEditor.cpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-25.
//

#include "cSoundEditor.hpp"

#include "ofMath.h"
#include "cPlayer.h"
#include "cSampleSynth.hpp"


void cSoundEditor::SetupGUI( void )
{
	int width = guiWidth;
    int left = 0;
    guiElementHeight = 28;
	int offset = 0;
	int heightStart = 256;
	int column = 0;
    sliderLabelWidth = 160;
    waveRange = 1.0f;
    menuExpanded = false;
	
	offset ++;
    currentGUIXStart = left;
    currentGUIYStart = heightStart;
    CreateButton("LOAD SYNTH", guiWidth * 1.0f / 3.0f, column*guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onLoadSynthEvent );
    CreateButton("SAVE SYNTH", guiWidth * 1.0f / 3.0f, column*guiWidth + guiWidth * 1.0f / 3.0f, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onSaveSynthEvent );
    CreateToggle("SHOW GUI", &visible, guiWidth * 1.0f / 3.0f, column * guiWidth + guiWidth * 2.0f / 3.0f, offset);
    globalComponents.push_back( guiComponents_.back() );

    offset ++;
    CreateButton("CREATE FROM FOLDER", guiWidth * .5, column * guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onCreateSynthFromFolderEvent );
    CreateToggle("STACK", &stackSamples, guiWidth * .25, column * guiWidth + guiWidth * .5, offset);
    offset = CreateToggle("RANDOM", &randomizeSamples, guiWidth* .25, column * guiWidth + guiWidth * .5 + guiWidth* .25, offset);
    
    offset = CreateToggle("GLOBAL EDIT", &globalEdit, guiWidth / 2, column * guiWidth, offset);
    guiComponents_.back()->onToggleEvent( this, &cSoundEditor::onGlobalEditEvent );
    
    offset ++;
    offset = CreateDropDown("DEVICE", owner->deviceNames, &owner->currentDevice, guiWidth, column*guiWidth, offset );
    guiComponents_.back()->onDropdownEvent( this, &cSoundEditor::onSelectDeviceDropdownEvent );
    globalComponents.push_back( guiComponents_.back() );
    
    CreateNumericInputInt("# CHANNELS", &owner->outChannelCount, guiWidth *.5, column * guiWidth, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onNumOutChannelsSelectEvent );
    globalComponents.push_back( guiComponents_.back() );
    offset = CreateNumericInputInt("1ST CHANNEL", &owner -> channelOffset, guiWidth *.5, column * guiWidth + guiWidth *.5, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onChannelOffsetChangedEvent );
    globalComponents.push_back( guiComponents_.back() );
    
    offset = CreateFloatSlider("MASTER VOLUME", 0.0f, 4.0f, &owner->masterVolume, guiWidth, column*guiWidth, offset);
    globalComponents.push_back( guiComponents_.back() );
    offset = CreateFloatSlider("INPUT LIMIT", 0.0f, 4.0f, &owner->impulseLimit, guiWidth, column*guiWidth, offset);
    globalComponents.push_back( guiComponents_.back() );

    offset ++;
    CreateButton("DISTRIBUTE SOUND", guiWidth / 2, column * guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onDistributeLinearEvent );

    offset = CreateButton("DISTRIBUTE RANDOM", guiWidth / 2, column*guiWidth + guiWidth / 2, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onDistributeRandomEvent );

    CreateNumericInputInt("LEFT RIGHT #", &leftRightCount, guiWidth *.5, column*guiWidth, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onLeftRightEvent );
    globalComponents.push_back( guiComponents_.back() );
    offset = CreateNumericInputInt("TOP BOTTOM #", &topBottomCount, guiWidth * .5, column * guiWidth + guiWidth *.5, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onTopBottomEvent );
    globalComponents.push_back( guiComponents_.back() );
    offset = CreateNumericInputInt("FRONT BACK #", &frontBackCount, guiWidth *.5, column*guiWidth, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onFrontBackEvent );
    globalComponents.push_back( guiComponents_.back() );
    
    offset ++;
    CreateButton("NORMALIZE SAMPLES", guiWidth / 2, column*guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onNormalizeSamplesEvent );
    offset = CreateButton("SAVE FFT", guiWidth / 2, column*guiWidth + guiWidth / 2.0, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onSaveFFTEvent );
    offset = CreateFloatSlider( "ENERGY", 0.0f, 1.0f, &owner -> normBalance, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onChangeNormalizeBalanceEvent );
    globalComponents.push_back( guiComponents_.back() );
    offset = CreateFloatSlider("COMPLEXITY", 0.0f, 1.0f, &owner -> complexityWeight, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onChangeComplexityBalanceEvent );
    globalComponents.push_back( guiComponents_.back() );
    offset = CreateFloatSlider("SPECTRUM", 0.0f, 1.0f, &owner -> complexitySpectrumWeight, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onChangeComplexitySpectrumBalanceEvent );
    globalComponents.push_back( guiComponents_.back() );

    offset ++;
    offset = CreateFloatSlider("RELEASE HOLD", 0.0f, 1.0f, &owner -> inactiveThreshold, guiWidth, column * guiWidth, offset );
    globalComponents.push_back( guiComponents_.back() );
    offset = CreateNumericInputInt( "OSC PORT", &owner -> oscPort, guiWidth - 94, column * guiWidth, offset );
    globalComponents.push_back( guiComponents_.back() );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onOSCPortEvent );
    rxIndicatorRect = ofRectangle(currentGUIXStart + column * guiWidth + guiWidth - 94, offset * guiElementHeight + currentGUIYStart - guiElementHeight + 2, 92, guiElementHeight - 6);
    
    column ++;
    offset = 0;
    CreateNumericInputInt("SOUND OBJECT", &soundObjectID, guiWidth *.6, column*guiWidth, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onSoundObjectSelectEvent );
    
    offset = CreateButton("RANDOMIZE", guiWidth * 0.4, column*guiWidth + guiWidth *.6, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onRandomizeSoundObjectsEvent );

    CreateButton("LOAD SOUND OBJECT", guiWidth / 2, column*guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onLoadSoundObjectEvent );

    offset = CreateButton("SAVE SOUND OBJECT", guiWidth / 2, column*guiWidth + guiWidth / 2, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onSaveSoundObjectEvent );

    soundObjectGUIComponentsIndex = guiComponents_.size();
    offset = CreateFloatSlider( "AMPLITUDE", 0.0f, 4.0f, GetAmplitudeFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onAmpEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    
    offset = CreateFloatSlider( "PITCH", 0.0f, 10.0f, GetObjPitchFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onObjPitchEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    offset = CreateFloatSlider( "SENSITIVITY", 0.0f, 1024.0f, GetSensitivityFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSensitivityEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    offset = CreateFloatSlider( "SMOOTHING", 0.0f, 1.0f, GetSmoothingFunction, guiWidth, column * guiWidth, offset);
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSmoothingEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    offset = CreateFloatSlider( "FADE IN TIME", 0.0f, 4.0f, GetObjectFadeInRateFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onObjectFadeInRateEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    offset = CreateFloatSlider( "FADE OUT TIME", 0.0f, 4.0f, GetFadeOutRateFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onFadeOutRateEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    offset = CreateFloatSlider( "FADE CURVE", 0.0f, 8.0f, GetFadeCurveFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onFadeCurveEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    
    offset = CreateFloatSlider( "RETRIG DELAY", 0.0f, 1000.0f, GetRetriggerDelayFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onRetriggerDelayEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    offset = CreateToggle("SIMPLE TRIGGER", GetTriggerModeFunction, guiWidth / 2, column * guiWidth, offset);
    guiComponents_.back()->onToggleEvent( this, &cSoundEditor::onTriggerModeEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    offset ++;
    vector<string> options = {"STEREO", "STEREO PLACED", "STEREO QUAD"};
    CreateDropDown("MIX MODE", options, GetMixModeFunction, guiWidth / 2, column*guiWidth, offset );
    guiComponents_.back()->onDropdownEvent( this, &cSoundEditor::onMixModeDropdownEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    vector<string> channelOptions = {"QUAD CHANNELS 1 2 3 4", "QUAD CHANNELS 3 4 5 6", "QUAD CHANNELS 5 6 7 8"};
    offset = CreateDropDown("QUAD CHANNELS", channelOptions, GetQuadChannelsFunction, guiWidth / 2, column*guiWidth + guiWidth / 2, offset );
    guiComponents_.back()->onDropdownEvent( this, &cSoundEditor::onQuadChannelsDropdownEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    quadChannelsMenuIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "PAN", 0.0f, 1.0f, GetPanFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onPanEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    CreateFloatSlider( "FRONT BACK", 0.0f, 1.0f, GetPanFrontBackFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onPanFrontBackEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    frontBackIndex = guiComponents_.size() - 1;

    CreateNumericInputInt("LEFT CHAN", GetLeftChannelFunction, guiWidth / 2, column * guiWidth, offset);
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onLeftChannelAssignEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(0);
    leftChannelIndex = guiComponents_.size() - 1;
    
    offset = CreateNumericInputInt("RIGHT CHAN", GetRightChannelFunction, guiWidth / 2, column * guiWidth + guiWidth / 2, offset);
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onRightChannelAssignEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(1);
    rightChannelIndex = guiComponents_.size() - 1;
    
    CreateNumericInputInt("LEFT FRONT", GetLeftChannelFunction, guiWidth * .5, column * guiWidth, offset);
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onLeftChannelAssignEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(0);
    leftFrontChannelIndex = guiComponents_.size() - 1;
    
    offset = CreateNumericInputInt("RIGHT FRONT", GetRightChannelFunction, guiWidth * .5, column * guiWidth + guiWidth * 0.5, offset);
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onRightChannelAssignEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(1);
    rightFrontChannelIndex = guiComponents_.size() - 1;

    CreateNumericInputInt("LEFT BACK", GetLeftBackChannelFunction, guiWidth * .5, column * guiWidth, offset);
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onLeftBackChannelAssignEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(0);
    leftBackChannelIndex = guiComponents_.size() - 1;
    
    offset = CreateNumericInputInt("RIGHT BACK", GetRightBackChannelFunction, guiWidth * 0.5, column * guiWidth + guiWidth * 0.5, offset);
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onRightBackChannelAssignEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(1);
    rightBackChannelIndex = guiComponents_.size() - 1;

    offset = 0;
    column ++;
    
    CreateNumericInputInt("SOUND SLOT", &selectedSoundDefIndex, guiWidth *.5, column*guiWidth, offset );
    soundSlotIndex = guiComponents_.size() - 1;
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onSoundDefIndexSelectEvent );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(-1);

    vector<string> choiceOptions = {"FIXED SLOT", "RANDOM SLOT", "SEQUENTIAL SLOT"};
    offset = CreateDropDown("CHOICE MODE", choiceOptions, GetSoundDefChoiceFunction, guiWidth *.5, column*guiWidth + guiWidth *.5, offset );
    guiComponents_.back()->onDropdownEvent( this, &cSoundEditor::onSoundChoiceSelectEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

//    offset = CreateNumericInputInt("CHOICE MODE", GetSoundDefChoiceFunction, guiWidth *.5, column*guiWidth + guiWidth *.5, offset );
//    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onSoundChoiceSelectEvent );
//    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(-1);
    
    offset = CreateButton("NEW SLOT", guiWidth / 2, column*guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onNewSlotEvent );
    
    offset = CreateNumericInputInt("SOUND DEF", GetSoundDefFunction, guiWidth *.5, column*guiWidth, offset );
    guiComponents_.back()->onTextInputEvent( this, &cSoundEditor::onSoundDefSelectEvent );
    ((ofxDatGuiNumericIntInput *)guiComponents_.back())->setValue(-1);
	
    CreateButton("LOAD SOUND DEF", guiWidth / 2, column * guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onLoadSoundDefEvent );
    soundDefGUIComponentsStartIndex = guiComponents_.size();

    offset = CreateButton("SAVE SOUND DEF", guiWidth / 2, column * guiWidth + guiWidth / 2.0, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onSaveSoundDefEvent );

    CreateButton("LOAD SAMPLE", sliderLabelWidth, column * guiWidth, offset);
    guiComponents_.back()->onButtonEvent( this, &cSoundEditor::onLoadSampleButtonEvent );
	
    offset = CreateLabel("", guiWidth - sliderLabelWidth, column * guiWidth + sliderLabelWidth, offset);
    guiComponents_.back()->setLabelUpperCase( false );
    sampleNameIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "AMPLITUDE", 0.0f, 10.0f, GetSampleAmpFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleAmpEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    offset = CreateFloatSlider( "PITCH", 0.0f, 10.0f, GetSamplePitchFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSamplePitchEvent );
    globalEditGUIList.push_back( guiComponents_.back() );

    offset = CreateFloatSlider( "START", 0.0f, 1.0f, GetSampleStartFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleStartEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    sampleStartIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "END", 0.0f, 1.0f, GetSampleEndFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleEndEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    sampleEndIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "ATTACK TIME", 0.0f, 4.0f, GetSampleAttackFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleAttackEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
	attackIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "DECAY TIME", 0.0f, 4.0f, GetSampleDecayFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleDecayEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
	decayIndex = guiComponents_.size() - 1;

	offset = CreateToggle("LOOP", GetSampleDoLoopFunction, guiWidth / 2, column * guiWidth, offset );
    guiComponents_.back()->onToggleEvent( this, &cSoundEditor::onSampleDoLoopEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    loopIndex = guiComponents_.size() - 1;
    
    offset = CreateFloatSlider( "LOOP START", 0.0f, 1.0f, GetSampleLoopStartFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleLoopStartEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    loopStartIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "LOOP END", 0.0f, 1.0f, GetSampleLoopEndFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleLoopEndEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
    loopEndIndex = guiComponents_.size() - 1;

    offset = CreateFloatSlider( "CROSS FADE", 0.0f, 0.5f, GetSampleCrossFadeFunction, guiWidth, column * guiWidth, offset );
    guiComponents_.back()->onSliderEvent( this, &cSoundEditor::onSampleCrossFadeEvent );
    globalEditGUIList.push_back( guiComponents_.back() );
}

void cSoundEditor::ShowGUI( void )
{
    std::ostringstream scale;
    scale << std::setprecision(2);
	ofColor white( 255, 255, 255 );
	ofSetColor( white );
	
	float max = ofGetWidth();
	float height = 256;
    if( visible)
        ShowGUIElements();
    
	
	ofNoFill();
	cSoundDefinition *soundDef = SoundDef();
	
	if( soundDef )
		{
		if( soundDef -> SampleLoaded() && visible )
			{
			ofPushMatrix();
			for( int c = 0; c < soundDef -> channelCount; ++ c )
				{
                ofSetColor(0, 192, 64);
				ofBeginShape();
                ofSetLineWidth(0.5);
				for( int x = 0; x < max; ++ x)
					{
					int n = ofMap( x, 0, max, 0, soundDef -> sampleCount, true );
					float val = soundDef -> buffer[ n * soundDef -> channelCount + c ];
                    float y = ofMap( val, -waveRange, waveRange, height * 0.5, 0.0f, true);
                    //ofClamp(y, -1, 1);
					ofVertex( x, y );
					}
				ofEndShape();
				ofTranslate( 0.0, height * 0.5 );
				}
			ofPopMatrix();
            ofSetLineWidth(1.0);
            scale << (1.0f/waveRange);

            ofSetColor(128, 128, 128);
            guiTheme.font.ptr->draw("SCALE: " + scale.str(), 20, height * 0.5 - 10);
            ofDrawLine(glm::vec2(0.0, height * 0.5), glm::vec2(max, height * 0.5));
            ofSetColor(ofColor::white);
        }

		if( visible )
			{
			//soundDef
			ofFill();
			if( soundDef -> SampleLoaded() )
				{

				bool looping = ( soundDef -> loop );
				if( looping )
					{
					ofColor red( looping ? 255 : 192, looping ? 192 : 128, 0 );
					ofSetColor( red );
					float n = ofMap( soundDef -> startLoopSample, 0, soundDef -> sampleCount, 0, max, true );
					ofDrawLine( n, 0, n, height);
					n = ofMap( soundDef -> endLoopSample, 0, soundDef -> sampleCount, 0, max, true );
					ofDrawLine( n, 0, n, height);
					
					ofColor dark_red( looping ? 192 : 128, looping ? 128 : 64, 0 );
					ofSetColor( dark_red );
					n = ofMap( soundDef -> enterLoopCrossFadeSample, 0, soundDef -> sampleCount, 0, max, true );
					ofDrawLine( n, 0, n, height);
					n = ofMap( soundDef -> exitLoopCrossFadeSample, 0, soundDef -> sampleCount, 0, max, true );
					ofDrawLine( n, 0, n, height);
					}
				
				ofColor dark_blue( 0, 128, 255 );
				ofSetColor( dark_blue );
				float sa = ofMap( soundDef -> startSample, 0, soundDef -> sampleCount, 0, max, true );
				float attackSamples = ofMap( soundDef -> attackSamples, 0, soundDef -> sampleCount, 0, max, true );
				ofDrawLine( sa + attackSamples, 0, sa, height);
				float ed = ofMap( soundDef -> endSample, 0, soundDef -> sampleCount, 0, max, true );
				float decaySamples = ofMap( soundDef -> releaseSamples, 0, soundDef -> sampleCount, 0, max, true );
				ofDrawLine( ed - decaySamples, 0, ed, height);
				}
			}
		}
	if( soundDef && soundDef -> SampleLoaded() && visible )
		{
		ofNoFill();
		ofColor red( 255, 0, 0 );
		ofSetColor( red );
		
		if(	player && player -> active )
			{
			uint64_t pos = static_cast<uint64_t>(player -> position);
			if( pos > soundDef -> sampleCount )
				pos = pos - soundDef -> sampleCount + soundDef -> exitLoopCrossFadeSample;
			float phx = ofMap( pos, 0, soundDef -> sampleCount, 0, ofGetWidth() );
			ofDrawLine( phx, 0, phx, height);
			}
		}
	if( visible )
		{
		ofFill();
		for( int c = 0; c < owner -> players.size(); ++ c )
			{
			cPlayer *thisPlayer = owner -> players[ c ];
			if( thisPlayer -> claimed )
				{
				if( thisPlayer -> active )
					{
					switch(  thisPlayer -> envelopeMode )
						{
						case t_EnvelopeIdle:
							ofSetColor( 128, 0, 255 ); //purple
							break;
						
						case t_EnvelopeAttackMode:
							ofSetColor( 255, 255, 0 );  //yellow
							break;
							
						case t_EnvelopeSustainMode:
							ofSetColor( 0, 255, 0 );     //green
							break;
							
						case t_EnvelopeReleaseMode:     //blue
                            ofSetColor( 0, 128, 255 );
							break;
                                
                        case t_EnvelopeInteractiveMode:
                            ofSetColor( 255, 255, 255 );
                            break;

                        case t_EnvelopeInteractiveReleaseMode:
                            ofSetColor( 0, 255, 255 );
                            break;
                        }
					}
				else
					{
					switch( thisPlayer -> envelopeMode )
						{
						case t_EnvelopeIdle:
							ofSetColor( 64, 0, 64 );    //grey
							break;
							
						case t_EnvelopeAttackMode:      // light orange
							ofSetColor( 64, 64, 0 );
							break;
							
						case t_EnvelopeSustainMode:     //
							ofSetColor( 0, 64, 0 );
							break;
							
						case t_EnvelopeReleaseMode:
                            ofSetColor( 0, 48, 64 );
							break;
						}
                    }
				}
			else
				{
				ofSetColor( 48, 48, 48 );
				}
			
			ofRectangle rect;
            rect.x = ( c / 4 ) * 15;
            rect.y = 258 + ( c & 3 ) * 6;
            rect.width = 14;
			rect.height = 5;
			ofDrawRectangle(rect);
			}
		}
}

void cSoundEditor::UpdateGUI( void )
{
	if( soundObject && visible )
		{
        UpdateGUIElements();
		}
}


void cSoundEditor::ExtractGlobalValuesForDisplay( void )
{
    for( ofxDatGuiComponent *comp: globalComponents )
    {
        switch(comp -> getType())
        {
            case ofxDatGuiType::TOGGLE:
                ((cToggle *)comp) -> UpdateValueFromSource();
                //               ((cToggle *)comp) -> update();
                break;
                
            case ofxDatGuiType::DROPDOWN:
                ((cDropDown *)comp) -> UpdateValueFromSource();
                //                ((cDropDown *)comp) -> update();
                break;
                
            case ofxDatGuiType::SLIDER:
                ((cFocussedSlider *)comp) -> UpdateValueFromSource();
                //               ((cFocussedSlider *)comp) -> update();
                break;
                
            case ofxDatGuiType::TEXT_INPUT:
            {
                ofxDatGuiNumericInput *input = ((ofxDatGuiNumericInput *)comp);
                if( input -> isInt)
                {
                    ((ofxDatGuiNumericIntInput *)comp) -> UpdateValueFromSource();
                }
                else
                {
                    ((ofxDatGuiNumericInput *)comp) -> UpdateValueFromSource();
                }
                //                ((ofxDatGuiNumericInput *)comp) -> update();
                break;
            }
        }
    }
}

void cSoundEditor::ExtractSoundObjectValuesForDisplay( void )
{
    if( soundObject == nullptr )
    {
        AttachToSoundObjectByID( soundObjectID );
        return;
    }

    if( soundObject )
        {
        cSoundDefinition *soundDef = SoundDef();
		if(soundDef)
			soundDef -> editor = this;
        for( int i = soundObjectGUIComponentsIndex; i < soundDefGUIComponentsStartIndex; i ++ )
        {
            ofxDatGuiComponent *comp = guiComponents_[i];
            switch(comp -> getType())
            {
            case ofxDatGuiType::BUTTON:
                ((ofxDatGuiButton *)comp) -> update();
                break;

            case ofxDatGuiType::TOGGLE:
                ((cToggle *)comp) -> UpdateValueFromSource();
 //               ((cToggle *)comp) -> update();
                break;

            case ofxDatGuiType::DROPDOWN:
                ((cDropDown *)comp) -> UpdateValueFromSource();
//                ((cDropDown *)comp) -> update();
                break;

            case ofxDatGuiType::SLIDER:
                ((cFocussedSlider *)comp) -> UpdateValueFromSource();
 //               ((cFocussedSlider *)comp) -> update();
                break;

            case ofxDatGuiType::VALUE_PLOTTER:
                ((ofxDatGuiValuePlotter *)comp) -> update(true);
                break;

            case ofxDatGuiType::TEXT_INPUT:
            {
                ofxDatGuiNumericInput *input = ((ofxDatGuiNumericInput *)comp);
                if( input -> isInt)
                {
                    ((ofxDatGuiNumericIntInput *)comp) -> UpdateValueFromSource();
                }
                else
                {
                    ((ofxDatGuiNumericInput *)comp) -> UpdateValueFromSource();
                }
//                ((ofxDatGuiNumericInput *)comp) -> update();
                break;
            }

            case ofxDatGuiType::LABEL:
                if(i==sampleNameIndex)
                {
                    if(SoundDef())
                        ((ofxDatGuiLabel*)comp) ->setLabel(SoundDef()->sampleName);
                }
                break;
            }
        }
    }
    ExtractSoundDefValuesForDisplay();
}


void cSoundEditor::ExtractSoundDefValuesForDisplay( void )
{
	if( soundObject == nullptr )
		AttachToSoundObjectByID( soundObjectID );
	
	if( soundObject )
		{
		cSoundDefinition *soundDef = SoundDef();

		if( soundDef == nullptr )
			return;

        for( int i = soundDefGUIComponentsStartIndex; i < guiComponents_.size(); i ++ )
        {
            ofxDatGuiComponent *comp = guiComponents_[i];
            switch(comp -> getType())
            {
            case ofxDatGuiType::BUTTON:
                ((ofxDatGuiButton *)comp) -> update();
                break;

            case ofxDatGuiType::TOGGLE:
                ((cToggle *)comp) -> UpdateValueFromSource();
 //               ((cToggle *)comp) -> update();
                break;

            case ofxDatGuiType::DROPDOWN:
                ((cDropDown *)comp) -> UpdateValueFromSource();
//                ((cDropDown *)comp) -> update();
                break;

            case ofxDatGuiType::SLIDER:
                ((cFocussedSlider *)comp) -> UpdateValueFromSource();
 //               ((cFocussedSlider *)comp) -> update();
                break;

            case ofxDatGuiType::VALUE_PLOTTER:
                ((ofxDatGuiValuePlotter *)comp) -> update(true);
                break;

            case ofxDatGuiType::TEXT_INPUT:
            {
                ofxDatGuiNumericInput *input = ((ofxDatGuiNumericInput *)comp);
                if( input -> isInt)
                {
                    ((ofxDatGuiNumericIntInput *)comp) -> UpdateValueFromSource();
                }
                else
                {
                    ((ofxDatGuiNumericInput *)comp) -> UpdateValueFromSource();
                }
//                ((ofxDatGuiNumericInput *)comp) -> update();
            }
                break;

            case ofxDatGuiType::LABEL:
                if(i==sampleNameIndex)
                {
                    if(SoundDef())
                        ((ofxDatGuiLabel*)comp) ->setLabel(SoundDef()->sampleName);
                }
                break;
            }
        }
    }
}

void GetSoundDefChoiceFunction(cSoundEditor *editor, ofxDatGuiComponent *dropDown)
{
    ((cDropDown *)dropDown) -> select( editor -> soundObject -> soundDefChoiceMode );
}



void GetMixModeFunction(cSoundEditor *editor, ofxDatGuiComponent *dropDown)
{
    ((cDropDown *)dropDown) -> select( editor -> soundObject -> mix.mode );
}

void GetQuadChannelsFunction(cSoundEditor *editor, ofxDatGuiComponent *dropDown)
{
    ((cDropDown *)dropDown) -> select( editor -> soundObject -> mix.quadSet );
}

void cSoundEditor::onMasterVolumeEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	float vol = e.target -> getValue();
	owner -> masterVolume = vol;
}


void GetPanFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> mix.pan );
}


void cSoundEditor::onPanEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	float pan = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.SetPan( pan );
    }
    else if( soundObject )
		soundObject -> mix.SetPan( pan );
}

void GetPanFrontBackFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> mix.frontBackPan );
}


void cSoundEditor::onPanFrontBackEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	float panFrontBack = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.SetFrontBackPan( panFrontBack );
    }
    else if( soundObject )
		soundObject -> mix.SetFrontBackPan( panFrontBack );
}

void GetAmplitudeFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> mix.globalLevel );
}


void cSoundEditor::onAmpEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	float amp = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.globalLevel = amp;
    }
    else if( soundObject )
		soundObject -> mix.globalLevel = amp;
	
}

void GetObjPitchFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> pitch );
}


void cSoundEditor::onObjPitchEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	float pitch = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> pitch = pitch;
    }
    else if( soundObject )
		soundObject -> pitch = pitch;
	
}


void GetSoundDefFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    ((ofxDatGuiNumericInput *)numberBox) -> setValue( editor -> soundObject -> CurrentSoundDefID() );
}


void GetRetriggerDelayFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> retriggerDelay );
}

void cSoundEditor::onRetriggerDelayEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	float delay = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> retriggerDelay = delay;
    }
    else if(soundObject)
        soundObject -> retriggerDelay = delay;
}

void GetTriggerModeFunction(cSoundEditor *editor, ofxDatGuiComponent *toggle)
{
    ((cToggle *)toggle) -> setChecked( editor -> soundObject -> triggerMode == t_ObjectOneShot );
}

void cSoundEditor::onTriggerModeEvent(ofxDatGuiToggleEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    bool oneShot = e.target -> getChecked();
    T_TriggerMode mode = t_ObjectInteractive;
    if( oneShot)
        mode = t_ObjectOneShot;
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> triggerMode = mode;
    }
    else if(soundObject)
        soundObject -> triggerMode = mode;
}

void GetObjectFadeInRateFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> fadeInRate );
}


void cSoundEditor::onObjectFadeInRateEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float fadeInRate = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> fadeInRate = fadeInRate;
    }
    else if(soundObject)
        soundObject -> fadeInRate = fadeInRate;
}




void GetFadeOutRateFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> fadeOutRate );
}


void cSoundEditor::onFadeOutRateEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float fadeOutRate = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> fadeOutRate = fadeOutRate;
    }
    else if(soundObject)
        soundObject -> fadeOutRate = fadeOutRate;
}


void GetFadeCurveFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> releasePower );
}


void cSoundEditor::onFadeCurveEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float fadeCurve = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> releasePower = fadeCurve;
    }
    else if(soundObject)
        soundObject -> releasePower = fadeCurve;
}


void GetSmoothingFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( sqrt( editor -> soundObject -> smoothing ) );
}


void cSoundEditor::onSmoothingEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float smoothing = e.target -> getValue();
    smoothing *= smoothing;
    
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> smoothing = smoothing;
    }
    else if(soundObject)
        soundObject -> smoothing = smoothing;
}

void GetSensitivityFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    ((cFocussedSlider *)slider) -> setValue( editor -> soundObject -> sensitivity );
}


void cSoundEditor::onSensitivityEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float sensitivity = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> sensitivity = sensitivity;
    }
    else if(soundObject)
        soundObject -> sensitivity = sensitivity;
}


void cSoundEditor::onSoundObjectSelectEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	int selectedSoundObject = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
    if(selectedSoundObject < 0 )
        selectedSoundObject = 0;
    if( player && player -> active )
		player -> Release();
	AttachToSoundObjectByID( selectedSoundObject );
    
//    ExtractSoundObjectValuesForDisplay();
}

void cSoundEditor::onRandomizeSoundObjectsEvent(ofxDatGuiButtonEvent e)
{
    int32_t holdObjectID = soundObjectID;
    if(mouseHandled)
        return;
    mouseHandled = true;
    int last = 0;
    for(int i = 0; i < owner->soundObjects.size(); i ++)
    {
        if(owner->soundObjects[i]->soundDefs.size() > 0)
            last = i;
    }
    
    random_shuffle(owner->soundObjects.begin(), owner->soundObjects.begin() + last);
    
    AttachToSoundObjectByID( soundObjectID );

}

void cSoundEditor::onNewSlotEvent(ofxDatGuiButtonEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    int32_t newSlotIndex = soundObject -> soundDefs.size();
    if( soundObject )
    {
        soundObject -> AddSoundDefSlot( newSlotIndex );
        selectedSoundDefIndex = newSlotIndex;
        soundObject -> currentSoundDefIndex = newSlotIndex;
        if( player && player -> active )
            player -> Stop();
        
        ((ofxDatGuiNumericInput *)guiComponents_[soundSlotIndex]) -> setValue(newSlotIndex);
        soundObject -> currentSoundDefIndex = newSlotIndex;
        if( player && player -> active )
            player -> Stop();
        int selectedSoundDef = soundObject -> CurrentSoundDefID();
        soundObject -> SetSoundDefByID( selectedSoundDef, newSlotIndex );
        ExtractSoundObjectValuesForDisplay();
        ExtractSoundDefValuesForDisplay();
    }
}

void cSoundEditor::onDistributeRandomEvent(ofxDatGuiButtonEvent e)
{
    for(cSoundObject *o:owner -> soundObjects)
    {
        if(o)
            o -> mix.Randomize(owner->outChannelCount);
    }
    ExtractSoundObjectValuesForDisplay();
}

void cSoundEditor::onChangeNormalizeBalanceEvent(ofxDatGuiSliderEvent e)
{
    owner -> normBalance = e.value;
    if( owner -> fftsReady && owner -> normalizeReady )
        owner -> NormalizeSamples();
    ExtractSoundObjectValuesForDisplay();
}

void cSoundEditor::onChangeComplexityBalanceEvent(ofxDatGuiSliderEvent e)
{
    owner -> complexityWeight = e.value;
    if( owner -> fftsReady && owner -> normalizeReady )
        owner -> NormalizeSamples();
    ExtractSoundObjectValuesForDisplay();
}

void cSoundEditor::onChangeComplexitySpectrumBalanceEvent(ofxDatGuiSliderEvent e)
{
    owner -> complexitySpectrumWeight = e.value;
    if( owner -> fftsReady && owner -> normalizeReady )
        owner -> NormalizeSamples();
    ExtractSoundObjectValuesForDisplay();
}


void cSoundEditor::onNormalizeSamplesEvent(ofxDatGuiButtonEvent e)
{
    owner -> NormalizeSamples();
    ExtractSoundObjectValuesForDisplay();
}


void cSoundEditor::onSaveFFTEvent(ofxDatGuiButtonEvent e)
{
    owner -> SaveFFT();
}


void cSoundEditor::onFFTNormalizeSamplesEvent(ofxDatGuiButtonEvent e)
{
    owner -> FFTNormalizeSamples();
    
}


void cSoundEditor::onDistributeLinearEvent(ofxDatGuiButtonEvent e)
{
    int which = 0;
    for(cSoundObject *o:owner -> soundObjects)
    {
        if(o)
            o -> mix.DistributeLinear(which, leftRightCount, topBottomCount, frontBackCount, owner->outChannelCount);
        which ++;
    }
    ExtractSoundObjectValuesForDisplay();
}

void cSoundEditor::onSoundDefIndexSelectEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    selectedSoundDefIndex = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
    if(soundObject)
    {
        if( soundObject -> soundDefs.size() <= selectedSoundDefIndex )
        {
            selectedSoundDefIndex = soundObject -> soundDefs.size() - 1;
        }
        soundObject -> currentSoundDefIndex = selectedSoundDefIndex;
        if( player && player -> active )
            player -> Stop();
        int selectedSoundDef = soundObject -> CurrentSoundDefID();
        soundObject -> SetSoundDefByID( selectedSoundDef, selectedSoundDefIndex );
        ExtractSoundObjectValuesForDisplay();
        ExtractSoundDefValuesForDisplay();
        }
}

void GetOSCPortFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    ((ofxDatGuiNumericIntInput *)numberBox) -> setValue( editor->owner->oscPort );
}


void cSoundEditor::onOSCPortEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    int32_t port = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
    if(owner)
        owner -> oscPort = port;
}

void cSoundEditor::onLeftRightEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    leftRightCount = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
}

void cSoundEditor::onTopBottomEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    topBottomCount = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
}

void cSoundEditor::onFrontBackEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    frontBackCount = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
}

void cSoundEditor::onNumOutChannelsSelectEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	int numChannelsOut = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
    owner -> outChannelCount = numChannelsOut;
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.SetOutChannelCount( numChannelsOut );
    }
    else if( soundObject )
		{
		soundObject -> mix.SetOutChannelCount( numChannelsOut );
		}
}

void cSoundEditor::onChannelOffsetChangedEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    int channelOffset = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.channelOffset = channelOffset;
    }
    else if( soundObject )
    {
        soundObject -> mix.channelOffset = channelOffset;
    }
}

void cSoundEditor::onLeftChannelAssignEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	int leftChan = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.leftFrontOutChannel = leftChan;
    }
    else if( soundObject )
		{
		soundObject -> mix.leftFrontOutChannel = leftChan;
		}
}

void GetLeftChannelFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    if(editor -> soundObject)
        ((ofxDatGuiNumericInput *)numberBox) -> setValue( editor -> soundObject -> mix.leftFrontOutChannel );
}


void cSoundEditor::onRightChannelAssignEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
	int rightChan = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.rightFrontOutChannel = rightChan;
    }
    else if( soundObject )
		{
		soundObject -> mix.rightFrontOutChannel = rightChan;
		}
}

void GetRightChannelFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    if(editor -> soundObject)
        ((ofxDatGuiNumericInput *)numberBox) -> setValue( editor -> soundObject -> mix.rightFrontOutChannel );
}


void cSoundEditor::onLeftBackChannelAssignEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    int leftChan = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.leftBackOutChannel = leftChan;
    }
    else if( soundObject )
    {
        soundObject -> mix.leftBackOutChannel = leftChan;
    }
}

void GetLeftBackChannelFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    if(editor -> soundObject)
        ((ofxDatGuiNumericInput *)numberBox) -> setValue( editor -> soundObject -> mix.leftBackOutChannel );
}


void cSoundEditor::onRightBackChannelAssignEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    int rightChan = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.rightBackOutChannel = rightChan;
    }
    else if( soundObject )
    {
        soundObject -> mix.rightBackOutChannel = rightChan;
    }
}

void GetRightBackChannelFunction(cSoundEditor *editor, ofxDatGuiComponent *numberBox)
{
    if(editor -> soundObject)
        ((ofxDatGuiNumericInput *)numberBox) -> setValue( editor -> soundObject -> mix.rightBackOutChannel );
}



void GetSampleAmpFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( editor -> SoundDef() -> amplitude );
}


void cSoundEditor::onSampleAmpEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float amp = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundDefinition *d: owner -> soundDefs)
            d -> amplitude = amp;
    }
    else if( SoundDef() )
        SoundDef() -> amplitude = amp;

}

void GetSamplePitchFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( editor -> SoundDef() -> pitch );
}


void cSoundEditor::onSamplePitchEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float pitch = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundDefinition *d: owner -> soundDefs)
            d -> pitch = pitch;
    }
    else if( SoundDef() )
        SoundDef() -> pitch = pitch;

}


void GetSampleStartFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>(editor -> SoundDef() -> startSample) / static_cast<double>(editor -> SoundDef() -> sampleCount) );
}


void cSoundEditor::onSampleStartEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float startSample = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetSampleStart( static_cast<uint64_t>(startSample * static_cast<double>(SoundDef() -> sampleCount)));

}


void GetSampleEndFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>(editor -> SoundDef() -> endSample) / static_cast<double>(editor -> SoundDef() -> sampleCount) );
}


void cSoundEditor::onSampleEndEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float endSample = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetSampleEnd( static_cast<uint64_t>(endSample * static_cast<double>(SoundDef() -> sampleCount)));

}


void GetSampleAttackFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( editor -> SoundDef() -> attackTime );
}


void cSoundEditor::onSampleAttackEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float attack = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundDefinition *d: owner -> soundDefs)
            d -> SetAttackTime(attack);
    }
    else if( SoundDef() )
        SoundDef() -> SetAttackTime(attack);

}

void GetSampleDecayFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( editor -> SoundDef() -> releaseTime );
}


void cSoundEditor::onSampleDecayEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float release = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundDefinition *d: owner -> soundDefs)
            d -> SetReleaseTime(release);
    }
    else if( SoundDef() )
        SoundDef() -> SetReleaseTime(release);

}


void GetSampleDoLoopFunction(cSoundEditor *editor, ofxDatGuiComponent *toggle)
{
    if(editor -> SoundDef())
        ((cToggle *)toggle) -> setChecked( editor -> SoundDef() -> loop );
}


void cSoundEditor::onSampleDoLoopEvent(ofxDatGuiToggleEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    bool loop = e.target -> getChecked();
    if(globalEdit)
    {
        for(cSoundDefinition *d: owner -> soundDefs)
            d -> loop = loop;
    }
    else if( SoundDef() )
        SoundDef() -> loop = loop;

}


void GetSampleLoopStartFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>(editor -> SoundDef() -> startLoopSample) / static_cast<double>(editor -> SoundDef() -> sampleCount) );
}


void cSoundEditor::onSampleLoopStartEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float startLoopSample = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetLoopStart( static_cast<uint64_t>(startLoopSample * static_cast<double>(SoundDef() -> sampleCount)));

}


void GetSampleLoopEndFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>(editor -> SoundDef() -> endLoopSample) / static_cast<double>(editor -> SoundDef() -> sampleCount) );
}


void cSoundEditor::onSampleLoopEndEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    float endLoopSample = e.target -> getValue();
    if( SoundDef() )
        SoundDef() -> SetLoopEnd( static_cast<uint64_t>(endLoopSample * static_cast<double>(SoundDef() -> sampleCount)));

}


void GetSampleCrossFadeFunction(cSoundEditor *editor, ofxDatGuiComponent *slider)
{
    if(editor -> SoundDef())
        ((cFocussedSlider *)slider) -> setValue( static_cast<double>( editor -> SoundDef() -> loopCrossFadeFraction ) );
}


void cSoundEditor::onSampleCrossFadeEvent(ofxDatGuiSliderEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;

    float loopCrossFadeFraction = e.target -> getValue();
    if(globalEdit)
    {
        for(cSoundDefinition *d: owner -> soundDefs)
            d -> SetLoopCrossFadeFraction( loopCrossFadeFraction );
    }
    else if( SoundDef() )
        SoundDef() -> SetLoopCrossFadeFraction( loopCrossFadeFraction );
}

void cSoundEditor::onSoundChoiceSelectEvent(ofxDatGuiDropdownEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;

    int modeChoice = e.child;
    
    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> soundDefChoiceMode = modeChoice;
    }
    else if(soundObject)
    {
        soundObject -> soundDefChoiceMode = modeChoice;
    }
}

void cSoundEditor::onQuadChannelsDropdownEvent(ofxDatGuiDropdownEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    int modeChoice = e.child;
    T_QuadChannelSet set = t_QuadChannels_1_2_3_4;
    switch(modeChoice)
    {
        case 1:
            set = t_QuadChannels_3_4_5_6;
            break;
            
        case 2:
            set = t_QuadChannels_5_6_7_8;
            break;
    }

    if(globalEdit)
    {
        for(cSoundObject *o: owner -> soundObjects)
            o -> mix.SetQuadSet(set);
    }
    else if(soundObject)
        soundObject -> mix.SetQuadSet(set);
    ExtractSoundObjectValuesForDisplay();
}


void cSoundEditor::onSelectDeviceDropdownEvent(ofxDatGuiDropdownEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    
    int deviceChoice = e.child;
    owner -> SelectDevice( deviceChoice );

}


void cSoundEditor::onGlobalEditEvent(ofxDatGuiToggleEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;
    
    bool glob = e.target -> getChecked();
    globalEdit = glob;
    if( glob )
    {
        for( ofxDatGuiComponent *c: globalEditGUIList)
        {
            float width = c -> getWidth();
            c -> setTheme( &globalEditTheme );
            c -> setWidth( width, globalEditTheme.layout.labelWidth );
        }
    }
    else
    {
        for( ofxDatGuiComponent *c: globalEditGUIList)
        {
            float width = c -> getWidth();
            c -> setTheme( &guiTheme );
            c -> setWidth( width, globalEditTheme.layout.labelWidth );
        }
    }
}


void cSoundEditor::onMixModeDropdownEvent(ofxDatGuiDropdownEvent e)
{
    if(mouseHandled)
        return;
    mouseHandled = true;

    int modeChoice = e.child;
    if(globalEdit)
    {
        switch( modeChoice )
        {
            case 0:
                for(cSoundObject *o: owner -> soundObjects)
                    o -> mix.InitStereo();
                break;
                
            case 1:
                for(cSoundObject *o: owner -> soundObjects)
                    o -> mix.InitStereoToMulti( 2 );
                break;
                
            case 2:
                for(cSoundObject *o: owner -> soundObjects)
                    o -> mix.InitStereoToQuadInMulti( 2 );
                break;
        }
    }
    
    else if( soundObject )
		{
		switch( modeChoice )
			{
				case 0:
					soundObject -> mix.InitStereo();
					break;
				
				case 1:
					soundObject -> mix.InitStereoToMulti( 2 );
					break;
				
				case 2:
					soundObject -> mix.InitStereoToQuadInMulti( 2 );
					break;
			}
		}
}


void cSoundEditor::onSoundDefSelectEvent(ofxDatGuiTextInputEvent e)
{
    if(mouseHandled)
        return;
    
	int selectedSoundDef = static_cast<int>(static_cast<ofxDatGuiNumericInput *>(e.target) -> getValue());
	if( player && player -> active )
		player -> Stop();
    if(selectedSoundDef >= 0)
    {
        if( soundObject )
            soundObject -> SetSoundDefByID( selectedSoundDef, selectedSoundDefIndex );
        ExtractSoundDefValuesForDisplay();
    }
    mouseHandled = true;
}

	
/*void cSoundEditor::onToggleLoopEvent(ofxDatGuiToggleEvent e)
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return;
	bool loop = e.target -> getChecked();
	soundDef -> loop = loop;
	
}*/


bool cSoundEditor::LoadSampleAction( void )
{
	ofFileDialogResult openFileResult= ofSystemLoadDialog("select an audio sample");
	//Check if the user opened a file
	if (openFileResult.bSuccess)
		{
		if( player && player -> active )
			player -> Stop();
		string filepath = openFileResult.getPath();
		cSoundDefinition *soundDef = SoundDef();
		if( soundDef )
			{
			if( soundDef -> LoadSample( filepath, true ) )
				{
                ExtractSoundDefValuesForDisplay();
                waveRange = 1.0;
				}
			}
		}
	return false;
}


void cSoundEditor::onLoadSampleButtonEvent(ofxDatGuiButtonEvent e)
{
    if(mouseHandled)
        return;
    
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return;
	LoadSampleAction();
    mouseHandled = true;
}


void cSoundEditor::onLoadSynthEvent(ofxDatGuiButtonEvent e)
{
    if(mouseHandled)
        return;
    
    owner->GetLoadSynthFile("Load Synth File:");
    AttachToSoundObjectByID(0);
    mouseHandled = true;
}


void cSoundEditor::onCreateSynthFromFolderEvent(ofxDatGuiButtonEvent e)
{
    if(mouseHandled)
        return;
    
    owner->CreateFromFolder();
    mouseHandled = true;
}

void cSoundEditor::onSaveSynthEvent(ofxDatGuiButtonEvent e)
{
    if(mouseHandled)
        return;
    
   owner->GetSaveSynthFile("Save Synth File:");
    mouseHandled = true;
}


void cSoundEditor::onLoadSoundObjectEvent(ofxDatGuiButtonEvent e)
{
    if(mouseHandled)
        return;
    
    soundObject -> GetLoadSoundObjectFile("Load SoundObject");
    ExtractSoundDefValuesForDisplay();
    mouseHandled = true;
}


void cSoundEditor::onSaveSoundObjectEvent(ofxDatGuiButtonEvent e)
{
    if(mouseHandled)
        return;
    
    soundObject -> GetSaveSoundObjectFile("Save SoundObject As");
    mouseHandled = true;

}

void cSoundEditor::onLoadSoundDefEvent(ofxDatGuiButtonEvent e)
{
//  load the file into the current soundDef
    ofxJSON json;
    Json::Value def;
    if(mouseHandled)
        return;
    
    if(SoundDef())
    {
        SoundDef() -> GetLoadSoundDefFile("Load SoundDef");
        ExtractSoundDefValuesForDisplay();
    }
    mouseHandled = true;
}


void cSoundEditor::onSaveSoundDefEvent(ofxDatGuiButtonEvent e)
{
    if(mouseHandled)
        return;
    SoundDef() -> GetSaveSoundDefFile("Save SoundDef As");
    mouseHandled = true;
}


void cSoundEditor::MouseReleased(int x, int y, int button)
{
	selectedParam = -1;
}


void cSoundEditor::MousePressed(int x, int y, int button)
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return;
	
	if( y > waveHeight )
		return;
	int64_t mouseSample = static_cast<uint64_t>( static_cast<float>(x) / static_cast<float>( ofGetWidth() ) * static_cast<float>(soundDef -> sampleCount ) );
    uint64_t diffs[ loopEndIndex + 5 ];
    for( int i = 0; i <= loopEndIndex + 4; i ++ )
		diffs[ i ] = 10000000000;
    diffs[ sampleStartIndex ] = abs( static_cast<int64_t>(soundDef -> startSample) - mouseSample );
    diffs[ sampleEndIndex ] = abs( static_cast<int64_t>(soundDef -> endSample) - mouseSample );
    diffs[ loopStartIndex ] = abs( static_cast<int64_t>(soundDef -> startLoopSample) - mouseSample );
    diffs[ loopEndIndex ] = abs( static_cast<int64_t>(soundDef -> endLoopSample) - mouseSample );
    diffs[ loopEndIndex + 1 ] = abs( static_cast<int64_t>(soundDef -> enterLoopCrossFadeSample) - mouseSample );
    diffs[ loopEndIndex + 2 ] = abs( static_cast<int64_t>(soundDef -> exitLoopCrossFadeSample) - mouseSample );
    diffs[ loopEndIndex + 3 ] = abs( static_cast<int64_t>(soundDef -> startSample + soundDef->attackSamples) - mouseSample );
    diffs[ loopEndIndex + 4 ] = abs( static_cast<int64_t>(soundDef -> endSample - soundDef->releaseSamples) - mouseSample );
    uint64_t leastDiff = 10000000000;
	int closestIndex = -1;
	
    for( int i = sampleStartIndex; i <= loopEndIndex + 4; i ++ )
		{
		if( diffs[ i ] <= leastDiff )
			{
			leastDiff = diffs[ i ];
			closestIndex = i;
			}
		}
	if( closestIndex != -1 )
		selectedParam = closestIndex;
    if(selectedParam == sampleStartIndex && y < waveHeight / 4)
        selectedParam = loopEndIndex + 3;
    else if(selectedParam == sampleEndIndex && y < waveHeight / 4)
        selectedParam = loopEndIndex + 4;
}


void cSoundEditor::MouseScrolled( int yScroll )
{
    if(yScroll < 0 )
        waveRange *= 1.1;
    else if(yScroll > 0)
        waveRange /= 1.1;

}


void cSoundEditor::MouseDragged( int x, int y, int button )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return;
    double mouseSample = ( static_cast<double>(x) / static_cast<double>( ofGetWidth() ) * static_cast<double>(soundDef -> sampleCount ) );
    double sliderVal = mouseSample / static_cast<double>(soundDef -> sampleCount );
	
    if( selectedParam == sampleStartIndex)
    {
        SoundDef() -> SetSampleStart( static_cast<uint64_t>(sliderVal * static_cast<double>(SoundDef() -> sampleCount)));
		GetSampleStartFunction(this, guiComponents_[sampleStartIndex]);
	}
    else if( selectedParam == sampleEndIndex)
    {
        SoundDef() -> SetSampleEnd( static_cast<uint64_t>(sliderVal * static_cast<double>(SoundDef() -> sampleCount)));
		GetSampleEndFunction(this, guiComponents_[sampleEndIndex]);
    }
    else if( selectedParam == loopStartIndex)
    {
        SoundDef() -> SetLoopStart( static_cast<uint64_t>(sliderVal * static_cast<double>(SoundDef() -> sampleCount)));
		GetSampleLoopStartFunction(this, guiComponents_[loopStartIndex]);
	}
    else if( selectedParam == loopEndIndex)
    {
        SoundDef() -> SetLoopEnd( static_cast<uint64_t>(sliderVal * static_cast<double>(SoundDef() -> sampleCount)));
		GetSampleLoopEndFunction(this, guiComponents_[loopEndIndex]);
    }
    else if( selectedParam == loopEndIndex + 1)
    {
        uint64_t loopSamples = soundDef -> endLoopSample - soundDef -> startLoopSample;
		uint64_t fadeSamples = mouseSample - soundDef -> startLoopSample;
		float crossfadeFraction = static_cast<float>(fadeSamples) / static_cast<float>(loopSamples);
        SoundDef() -> SetLoopCrossFadeFraction(crossfadeFraction);
		GetSampleCrossFadeFunction(this, guiComponents_[loopEndIndex + 1]);
    }
    else if( selectedParam == loopEndIndex + 2)
    {
        uint64_t loopSamples = soundDef -> endLoopSample - soundDef -> startLoopSample;
		uint64_t fadeSamples = soundDef -> endLoopSample - mouseSample;
		float crossfadeFraction = static_cast<float>(fadeSamples) / static_cast<float>(loopSamples);
        SoundDef() -> SetLoopCrossFadeFraction(crossfadeFraction);
		GetSampleCrossFadeFunction(this, guiComponents_[loopEndIndex + 1]);
	}
    else if( selectedParam == loopEndIndex + 3)
    {
        uint64_t attackSamples = mouseSample - soundDef -> startSample;
        float attackTime = static_cast<double>(attackSamples) / static_cast<double>(soundDef -> sampleRate);
        SoundDef() -> SetAttackTime(attackTime);
		GetSampleAttackFunction( this, guiComponents_[attackIndex]);
    }
    else if( selectedParam == loopEndIndex + 4)
    {
        uint64_t decaySamples = soundDef -> endSample - mouseSample;
        float decayTime = static_cast<double>(decaySamples) / static_cast<double>(soundDef -> sampleRate);
        SoundDef() -> SetReleaseTime(decayTime);
		GetSampleDecayFunction( this, guiComponents_[decayIndex]);
   }
}


bool cSoundEditor::TogglePlay( void )
{
	if( player && player -> active )
		{
		player -> SetRelease();
		return false;
		}
	else
		{
		if( player )
			player -> Release();
        player = owner -> Trigger( soundObjectID, true );
		return true;
		}
	return false;
}


bool cSoundEditor::ToggleUI( void ){ visible = !visible; return visible; }


void cSoundEditor::AttachToSoundObjectByID( int inSoundObjectID )
{
	if( owner )
		{
		soundObjectID = inSoundObjectID;
		soundObject = owner -> GetSoundObjectByID( inSoundObjectID );
        if(soundObject)
            selectedSoundDefIndex = soundObject -> currentSoundDefIndex;
        ExtractSoundObjectValuesForDisplay();
		}
}

//bool cSoundEditor::FillBuffer( ofSoundBuffer& buffer ){ if( player.active ) player.active = player.FillBuffer( buffer ); }
