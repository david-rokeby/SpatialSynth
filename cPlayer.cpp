 #include "cPlayer.h"
#include "ofSoundBuffer.h"
#include "cSampleSynth.hpp"

#include <math.h>
// add another layer that allows channel assign and mix...

/*bool cPlayer::SetSoundObject( cSoundObject *inSoundObject )
{
	soundObject = inSoundObject;
    return true;
}*/


bool cPlayer::AddToBuffer( ofSoundBuffer& buffer, float masterVolume )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr )
		return false;
	
    int32_t frameCount = buffer.getNumFrames();
    int32_t silenceTail = 0;
    
    if( localBuffer.getNumFrames() != buffer.getNumFrames() )
		localBuffer.allocate( buffer.getNumFrames(), 2 );
	if( localBuffer.getNumFrames() != buffer.getNumFrames() )
		return false;
	
    if( envelopeMode == t_EnvelopeIdle )
    {
        if( targetLevel > 0.0 )
        {
            switch( playMode )
            {
                case t_triggeredPlayMode:
                    envelopeMode = t_EnvelopeAttackMode;
                    break;
                    
                case t_collisionListPlayMode:
                    inactiveDuration = 0;
                    envelopeMode = t_EnvelopeInteractiveMode;
                    break;
            }
        }
        else
            silenceTail = frameCount;
    }
    if( envelopeMode != t_EnvelopeIdle )
    {
        if( active && soundDef && soundDef -> SampleLoaded() )
            {
            if( soundDef -> amplitude == 0 )
            {
                envelopeMode = t_EnvelopeIdle;
                targetLevel = 0;
            }
            if( position <= soundDef -> endSample && position > soundDef -> endSample - soundDef -> releaseSamples )
                {
                    envelopeMode = t_EnvelopeReleaseMode;
                    decayRate = soundDef -> releaseRate;
                    currentDecayRate = decayRate * targetLevel; //currentLevel?
                }

            active = soundDef -> FillBuffer( localBuffer, &position, sustain, soundObject -> pitch );
            float levelChange = targetLevel - currentLevel;
            size_t destBufferSize = localBuffer.getNumFrames();
            float rate = 0.0f;
            switch( envelopeMode )
                {
                case t_EnvelopeInteractiveMode:
                    // for interactive mode, the target may be higher than 1.0 so the rate is scaled to be able to achieve the target in the appropriate time
                    if( levelChange < 0 )
                        rate = - currentInteractiveDecayRate; //!!
                    else
                        rate = currentInteractiveAttackRate; //!!
                    break;
                        
                case t_EnvelopeInteractiveReleaseMode:
                    rate = - currentDecayRate;
                    break;
                    
                case t_EnvelopeAttackMode:
                    rate = currentAttackRate;
                    break;
                    
                case t_EnvelopeReleaseMode:
                    rate = - currentDecayRate;
                    break;
                    
                case t_EnvelopeSustainMode:
                    rate = 0;
                    break;
                    
                case t_EnvelopeIdle:
                    currentLevel = 0.0;
                    break;
                }
            float newLevel = soundDef -> ApplyEnvelope(this, localBuffer, masterVolume, rate, &silenceTail );
//            float newLevel = soundDef -> ApplyEnvelope(localBuffer, position, envelope, &envelopeMode, currentLevel * masterVolume, rate * masterVolume, targetLevel * masterVolume, &silenceTail, soundObject -> releasePower, releaseFromLevel * masterVolume );
 //           if( masterVolume != 0 )
 //               newLevel /= masterVolume;
            
            switch( envelopeMode )
                {
                case t_EnvelopeInteractiveReleaseMode:
//                    envelope = newLevel / currentLevel; //currentLevel is level we are releasing from
//                    if( envelope <= 0 )
//                        currentLevel = 0;
                    break;
                    
                case t_EnvelopeInteractiveMode:
                    if( currentLevel <= releaseThreshold )
                        currentLevel = 0;
                    break;
                    
                default:
//                    currentLevel = newLevel;
//                    if( targetLevel > 0)
//                        envelope = currentLevel / targetLevel;
//                    else
//                        envelope = 0;
                    if( targetLevel == 0.0 )
                    {
                        envelope = 0;
                        currentLevel = 0;
                    }
                    break;
                }
            
            if( currentLevel <= 0.0)
                {
                currentLevel = 0.0;
                envelopeMode = t_EnvelopeIdle;
                }
            soundObject -> mix.AddMix( localBuffer, buffer );
            }
        }
    else
        active = true;
    
    if( envelopeMode == t_EnvelopeIdle )
    {
        if( playMode == t_triggeredPlayMode )
            active = false;
        else
        {
            inactiveDuration += (float)silenceTail / (float)soundDef -> sampleRate;
            if( inactiveDuration > soundObject -> synth -> inactiveThreshold )
                active = false;
        }
    }
	
    return active;
}


void cPlayer::SetLoop( float in, float out, float fade )
{
	cSoundDefinition *soundDef = SoundDef();
    Stop();
	if( soundDef && soundDef -> SampleLoaded() )
		{
		float samples = static_cast<float>(soundDef -> sampleCount);
		soundDef -> SetLoop( in * samples, out * samples, fade );
		}
}


void cPlayer::SetSpeed( float inSpeed )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef )
		soundDef -> pitch = inSpeed;
}


void cPlayer::SetVolume( float inVolume )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef )
		soundDef -> amplitude = inVolume;
}


void cPlayer::ShowPlayPosition( bool editorVisible )
{
	cSoundDefinition *soundDef = SoundDef();
	if( soundDef == nullptr || soundDef -> SampleLoaded() == false )
		return;
	ofNoFill();
	ofColor white( 255, 255, 255 );
	ofSetColor( white );
	float height = ofGetHeight();
	if( editorVisible )
		height -= 300;

	if(	active)
		{
		uint64_t pos = static_cast<uint64_t>(position);
		if( pos > soundDef -> sampleCount )
			pos = pos - soundDef -> sampleCount + soundDef -> exitLoopCrossFadeSample;
		float phx = ofMap( pos, 0, soundDef -> sampleCount, 0, ofGetWidth() );
		ofDrawLine( phx, 0, phx, ofGetHeight());
		}
}


void cPlayer::SmoothUpdateLevel(float inLevel)
{
    // TARGET LEVEL ADJUSTS, but achieving target level still involves moving at currentAttackRate | currentDecayRate
	if( soundObject)
		targetLevel = targetLevel * soundObject -> smoothing + inLevel * (1.0 - soundObject -> smoothing);
	if( targetLevel > 10000)
		targetLevel = 10000;
}


// called only when player received impulse

void cPlayer::UpdateLevel( float newLevel, uint64_t inTime)
{
	if( soundObject)
    {
        if(soundObject -> triggerMode == t_ObjectInteractive)
		{
            if( envelopeMode == t_EnvelopeInteractiveReleaseMode )
            {
                maxTarget = 0;
                envelopeMode = t_EnvelopeInteractiveMode;
                currentInteractiveAttackRate = attackRate * newLevel;
                currentLevel = pow(envelope, releasePower) * releaseFromLevel;
            }
            
            if( newLevel > 0 )
                lastTouch = inTime;
            SmoothUpdateLevel( newLevel );
            if(targetLevel > maxTarget)
            {
                maxTarget = targetLevel;
                
                currentInteractiveDecayRate = decayRate * targetLevel;
                currentInteractiveAttackRate = attackRate * targetLevel;
            }
            else
            {
            //    currentDecayRate = currentDecayRate * fadeRateSmoothing + decayRate * targetLevel * ( 1.0 - fadeRateSmoothing );
            //    currentAttackRate = currentAttackRate * fadeRateSmoothing + attackRate * targetLevel * ( 1.0 - fadeRateSmoothing );
           }
        }
    }
}


//called on all active players only on receipt of interactions

void cPlayer::UpdateInteraction(uint64_t currentTime)
{
    if(active)
    {
        if(lastTouch != currentTime)
        {
            if( envelopeMode == t_EnvelopeInteractiveMode)
            {
                targetLevel = 0;
                envelopeMode = t_EnvelopeInteractiveReleaseMode;
                envelope = 1.0;
                releaseFromLevel = currentLevel;
            }
            
			if(currentLevel <= releaseThreshold)
            {
                currentLevel = 0;
                envelope = 0;
			// Release will be called by Release();
            // Release from here should be shaped by releasePower of soundObj
            }
        }
    }
}
