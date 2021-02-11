#ifndef CPLAYER_H
#define CPLAYER_H
#pragma once
#include "cSoundDefinition.h"
#include "cSoundObject.hpp"
#include "cMix.hpp"

enum T_PlayMode
{
	t_triggeredPlayMode,
	t_collisionListPlayMode
};

const int t_RandomChoice = -2;
const int t_CurrentChoice = -1;

class cPlayer
{
public:
    double position;
    int soundObjectID;
    cSoundObject *soundObject;
    int soundDefIndex;
    cSoundDefinition *soundDef;
    bool claimed;
    bool active;
    bool looping;
    bool sustain;
    bool detached;
    float envelope;
    float currentLevel;
	float targetLevel;
    float releaseFromLevel;
    float releasePower;
	float attackRate;
	float decayRate;
    float currentDecayRate;
	float currentAttackRate;
    float currentInteractiveDecayRate;
    float currentInteractiveAttackRate;
	float fadeRateSmoothing;
    float age;
	float maxTarget;
    float inactiveDuration;
    uint64_t lastTouch;
	float frameTimeSpan;
    float releaseThreshold;
    T_EnvelopeMode envelopeMode;
	T_PlayMode playMode;
    ofSoundBuffer localBuffer;

    cSoundDefinition *SoundDef( void ){ if( soundObject == nullptr ) return nullptr; return soundObject -> soundDefs[soundDefIndex]; }

    cPlayer(){ maxTarget = 0.0; frameTimeSpan = 1.0 / 44100.0; fadeRateSmoothing = 0.95; attackRate = 0.1; decayRate = currentDecayRate = 0.1; targetLevel = 0.0; soundObjectID = 0; soundDefIndex = 0; currentLevel = 1.0; releaseFromLevel = 0.0; releaseThreshold = 0.00001; position = 0.0; age = 0.0; claimed = false; active = false; looping = true; sustain = false; envelopeMode = t_EnvelopeIdle; envelope = 0.0; soundObject = nullptr; detached = false; inactiveDuration = 0.0;}
 //   bool SetSoundObject( cSoundObject *inSoundObject );
    bool AddToBuffer( ofSoundBuffer& buffer, float masterVolume );
    void SetSustain( bool inSustain ){ sustain = inSustain;}
   
    void SetRelease( void )
    {
        envelopeMode = t_EnvelopeReleaseMode;
        if( soundDef != nullptr && position <= soundDef -> endSample && position > soundDef -> endSample - soundDef -> releaseSamples )
        {
            decayRate = soundDef -> releaseRate;
            currentDecayRate = decayRate * targetLevel;
        }
    }

    void Stop( void ){ active = false; }
    void Release( void ){ active = false; claimed = false; soundObject = nullptr; }

	void PlayFromCollisionList( cSoundObject *inSoundObject, int inSoundObjectID, float inTargetLevel = 1.0 )
	{
		if( inSoundObject == nullptr )
			return;
		SetSoundObject( inSoundObject, inSoundObjectID );
		cSoundDefinition *aSoundDef = SoundDef();
		if( aSoundDef == nullptr || aSoundDef -> SampleLoaded() == false )
			return;
        switch(inSoundObject -> triggerMode)
        {
            case t_ObjectOneShot:
                playMode = t_triggeredPlayMode;
                envelopeMode = t_EnvelopeAttackMode;
                break;
                
            case t_ObjectInteractive:
                playMode = t_collisionListPlayMode;
                envelopeMode = t_EnvelopeInteractiveMode;
                break;
        }
		
        float attackSamples = (float)aSoundDef -> sampleRate * soundObject -> fadeInRate;
        float objectFadeInRate = 1.0 / attackSamples;
        if( aSoundDef -> attackRate < objectFadeInRate )
            attackRate = aSoundDef -> attackRate;
        else
            attackRate = objectFadeInRate;
        
		float decaySamples = (float)aSoundDef -> sampleRate * soundObject -> fadeOutRate;
		float objectDecayRate = 1.0 / decaySamples;
        if( aSoundDef -> releaseRate < objectDecayRate )
            decayRate = aSoundDef -> releaseRate;
        else
            decayRate = objectDecayRate;
		Play( inSoundObject, inTargetLevel, aSoundDef );
	}
	
	void SetSoundObject( cSoundObject *inSoundObject, int inSoundObjectID, bool forceCurrentSoundDefChoice = false )
	{
		soundObject = inSoundObject;
		soundObjectID = inSoundObjectID;
        if(soundObject)
        {
            if( !forceCurrentSoundDefChoice )
            {
                soundObject -> ChooseSoundDef();
            }
            soundDefIndex = soundObject -> currentSoundDefIndex;
        }
	}
	
    void Trigger( cSoundObject *inSoundObject, int inSoundObjectID, float inTargetLevel = 1.0, bool forceCurrentSoundDefChoice = false )
	{
		if( inSoundObject == nullptr )
			return;
		SetSoundObject( inSoundObject, inSoundObjectID, forceCurrentSoundDefChoice );
		cSoundDefinition *aSoundDef = SoundDef();
		if( aSoundDef == nullptr || aSoundDef -> SampleLoaded() == false )
			return;
		playMode = t_triggeredPlayMode;
        envelopeMode = t_EnvelopeAttackMode;

        //		attackRate = aSoundDef -> attackRate;

        float attackSamples = (float)aSoundDef -> sampleRate * soundObject -> fadeInRate;
        float objectFadeInRate = 1.0 / attackSamples;
        if( aSoundDef -> attackRate < objectFadeInRate )
            attackRate = aSoundDef -> attackRate;
        else
            attackRate = objectFadeInRate;

        float decaySamples = (float)aSoundDef -> sampleRate * soundObject -> fadeOutRate;
        float objectDecayRate = 1.0 / decaySamples;
        if( aSoundDef -> releaseRate < objectDecayRate )
            decayRate = aSoundDef -> releaseRate;
        else
            decayRate = objectDecayRate;
        Play( inSoundObject, inTargetLevel, aSoundDef );
	}

    void Play( cSoundObject *inSoundObject, float inTargetLevel, cSoundDefinition *inSoundDef )
    {
        Stop();
		maxTarget = 0.0;
        soundDef = inSoundDef;
		position = soundDef -> startSample;
		currentAttackRate = attackRate;
        currentInteractiveAttackRate = attackRate * inTargetLevel;
		currentDecayRate = decayRate;
        currentInteractiveDecayRate = decayRate * inTargetLevel;
        envelope = 0.0;
        currentLevel = 0;
		targetLevel = inTargetLevel;
        releaseFromLevel = inTargetLevel;
        releasePower = inSoundObject -> releasePower;
        sustain = true;
        active = true;
        detached = false;
        inactiveDuration = 0;
    }
	
    void Detach(void){ detached = true; }
    void SetLoop( float in, float out, float fade );
    void SetSpeed( float inSpeed );
    void SetVolume( float inVolume );
    void ScaleBufferRange( ofSoundBuffer& buffer, size_t start, size_t end, float scaler );
    void ZeroBufferRange( ofSoundBuffer& buffer, size_t start, size_t end );
    void ShowPlayPosition( bool editorVisible );

    void SmoothUpdateLevel(float inLevel);
    void UpdateLevel( float newLevel, uint64_t inTime);;
    void UpdateInteraction(uint64_t currentTime);

};

#endif // CPLAYER_H
