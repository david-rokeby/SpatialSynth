//
//  cSoundObject.hpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-26.
//
#pragma once
#ifndef cSoundObject_hpp
#define cSoundObject_hpp

#include <stdio.h>
#include "cSoundDefinition.h"
#include "cMix.hpp"
class cSampleSynth;
class cSoundDefinition;

enum T_TriggerMode
{
    t_ObjectOneShot,
    t_ObjectInteractive
};

class cSoundObject
{
public:
	cSampleSynth *synth;
    std::vector<int> soundDefIDs;
	std::vector<cSoundDefinition *>soundDefs;
    int32_t soundDefChoiceMode;
    int32_t currentSoundDefIndex;
    T_TriggerMode triggerMode;
    
	cMix mix;
	float pitch;
	float retriggerDelay;
	float fadeOutRate;
    float fadeInRate;
    float releasePower;
	float sampleDecayRate;
	float smoothing;
	float sensitivity;
	string currentSoundObjectFilePath;
    float lastTriggeredTime;

	cSoundObject( cSampleSynth *inSynth )
	{
		synth = inSynth;
        soundDefChoiceMode = 0;
        currentSoundDefIndex = -1;
//		soundDefID = -1;
//		soundDef = nullptr;
		mix.InitStereo();
		retriggerDelay = 0.0;
		pitch = 1.0;
        releasePower = 2.0;
		smoothing = 0.925;
		fadeInRate = 0.3;
        fadeOutRate = 0.5;
		sampleDecayRate = fadeOutRate / 44100.0f;
		sensitivity = 128.0f;
        lastTriggeredTime = 0;
        triggerMode = t_ObjectInteractive;
	}
    

    void ChooseSoundDef( void );
	void SetSoundDefByID( int id, int index = 0 );
	cSoundDefinition *SoundDef( void );
    int CurrentSoundDefID(void);
    void AddSoundDefSlot( int index );

	void SetPan( float inPan ){ mix.SetStereoPan( inPan ); }
	void SetVolume( float inVolume ){ mix.globalLevel = inVolume; }
	void SetRetriggerDelay( float inDelay ){ retriggerDelay = inDelay; }
	void LoadFromJSONSettings( const Json::Value &soundObjectJSON );
	void SaveSelfToJSONFile(string path);
	void GetSaveSoundObjectFile(string prompt);
	void LoadSelfFromJSONFile(string path);
	void GetLoadSoundObjectFile(string prompt);
	void UpdateJSONSettings( Json::Value& synth, int32_t index, bool includeDef );
};
#endif /* cSoundObject_hpp */
