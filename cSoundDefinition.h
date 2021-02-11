#pragma once

#ifndef CSOUNDDEFINITION_H
#define CSOUNDDEFINITION_H

#include "ofxAudioFile.h"
#include "ofSoundBuffer.h"
#include "ofxDatGui.h"
#include "ofxJSON.h"
#include "ofxFft.h"
//#include "cSoundEditor.hpp"

class cPlayer;

enum T_EnvelopeMode
{
    t_EnvelopeIdle,
    t_EnvelopeAttackMode,
    t_EnvelopeSustainMode,
    t_EnvelopeReleaseMode,
	t_EnvelopeInteractiveMode,
    t_EnvelopeInteractiveReleaseMode
};


class cSoundSettings
{
public:
	string pathName;
    float startFraction;
	float endFraction;
    float amplitude;
    float pitch;
    float loopStartSeconds; //change to seconds?
    float loopEndSeconds; //change to seconds?
    float loopCrossFadeSeconds; //change to seconds?
    float attack;
    float decay;
	bool loop;

	cSoundSettings( void ){ startFraction = 0; endFraction = 1.0; loopStartSeconds = 0.1; loopEndSeconds = 0.1; pitch = 1.0; amplitude = 1.0; loop = false; loopCrossFadeSeconds = .1; attack = .1; decay = .1; }
    void Init( float start, float end, float amp, float pitch_, bool _loop, float _loopStart, float _loopEnd, float _loopCrossFadeSeconds, float _attack, float _decay ) { startFraction = start; endFraction = end; amplitude = amp; pitch = pitch_; loop = _loop; loopStartSeconds = _loopStart; loopEndSeconds = _loopEnd; loopCrossFadeSeconds = _loopCrossFadeSeconds; attack = _attack; decay = _decay; }
	
};

class cSoundEditor;

class cSoundDefinition
{
public:
    static ofxFft *fft;
    cSoundEditor *editor;
    string pathName;
    string sampleName;
    float *buffer;
    uint64_t channelCount;
    uint64_t sampleCount;
    uint64_t sampleRate;
    double duration;
    uint64_t allocatedSize;

    float pan;
    float pitch;
    float amplitude;
    uint64_t startSample;
    uint64_t endSample;
    float attackTime;
    float releaseTime;
    float releaseSamples;
    float releaseRate;
    float attackRate;
    float attackSamples;

    uint64_t startLoopSample;
    uint64_t enterLoopCrossFadeSample;
    uint64_t exitLoopCrossFadeSample;
    uint64_t endLoopSample;

    bool loop;
    T_EnvelopeMode envelopeMode;

    float loopCrossFadeFraction;
    uint64_t crossFadeEndSample;
    uint64_t crossFadeStartSample;
    uint64_t crossFadeSamples;
    
    float normalizeGain;
    float energyGain;
    float spread;
    float averageBin;

    string currentSoundDefFilePath;
    
    std::vector<float> spectrumBuffer;

    cSoundDefinition();
    ~cSoundDefinition();
    void ReleaseBuffer();

    void Init( ofxAudioFile& file, cSoundSettings* settings, cSoundDefinition* defaultDef = nullptr );
    bool LoadSample( string samplePath, bool init = false, cSoundDefinition* defaultDef = nullptr );
    bool SampleLoaded( void ){ return buffer != nullptr; }
    uint64_t MillisecondsToSample( float ms ){ return static_cast<uint64_t>(static_cast<double>(ms) / 1000. * static_cast<double>(sampleRate) ); }
    uint64_t SecondsToSample( float ms ){ return static_cast<uint64_t>(static_cast<double>(ms) * static_cast<double>(sampleRate) ); }
    uint64_t FractionToSample( float ms ){ return static_cast<uint64_t>(static_cast<double>(ms) * static_cast<double>(sampleCount) ); }
    uint64_t SamplesToBytes( uint64_t samples ){ return sizeof(float) * channelCount * samples; }
    uint64_t SamplesToFloats( uint64_t samples ){ return channelCount * samples; }
    void SetExtent(uint64_t start, uint64_t end );
    void SetLoop( uint64_t loopStart, uint64_t loopEnd, float crossFadeTime, bool startChanged = false );
    void ClearLoop( void ){ loop = false; }
    void CalculateLoopCrossFade( void );
    void CalculateAttackDecay( void );

    void SetSampleEnd( uint64_t end ){ SetExtent( startSample, end ); }
    void SetSampleStart( uint64_t start ){ SetExtent( start, endSample ); }
    void SetLoopEnd( uint64_t end ){ SetLoop( startLoopSample, end, loopCrossFadeFraction, false ); }
    void SetLoopStart( uint64_t start ){ SetLoop( start, endLoopSample, loopCrossFadeFraction, true ); }
    void SetLoopCrossFadeFraction( float fraction ){ SetLoop( startLoopSample, endLoopSample, fraction, false ); }

    bool FillBuffer( ofSoundBuffer& destBuffer, double *sourceSampleIndex, bool sustain, float inPitch = 1.0 );
    void FillWithSamples( ofSoundBuffer& destBuffer, uint64_t *destSampleIndex, double *sourceSampleIndex, uint64_t sourceSampleStop, float inPitch );
    void FillWithSamplesPadSilence( ofSoundBuffer& destBuffer, uint64_t *destSampleIndex, double *sourceSampleIndex, uint64_t sourceSampleStop, float inPitch );
    void FillWithSilence( ofSoundBuffer& destBuffer, uint64_t *destSampleStart );
    void InsertLoopCrossFadeStartTransition( ofSoundBuffer& destBuffer, uint64_t *destSampleIndex, double *sourceSampleIndex, float inPitch );
    void InsertLoopCrossFadeEndTransition( ofSoundBuffer& destBuffer, uint64_t *destSampleIndex, double *sourceSampleIndex, float inPitch );

    bool FillBuffer( float *destBuffer, uint64_t destBufferSize, double *sourceSampleIndex, bool sustain, float inPitch = 1.0 );
    void FillWithSamples( float *destBuffer, uint64_t *destSampleStart, uint64_t destBufferSize, double *sourceSampleStart, uint64_t sourceSampleStop, float inPitch );
    void FillWithSamplesPadSilence( float *destBuffer, uint64_t *destSampleIndex, uint64_t destBufferSize, double *sourceSampleIndex, uint64_t sourceSampleStop, float inPitch );
    void FillWithSilence( float *destBuffer, uint64_t *destSampleStart, uint64_t destBufferSize );
    void InsertLoopCrossFadeStartTransition( float *destBuffer, uint64_t *destSampleIndex, uint64_t destBufferSize, double *sourceSampleIndex, float inPitch );
    void InsertLoopCrossFadeEndTransition( float *destBuffer, uint64_t *destSampleIndex, uint64_t destBufferSize, double *sourceSampleIndex, float inPitch );
    double SwitchToCrossFade( double *sourceSampleIndex );
    double SwitchFromCrossFade( double *sourceSampleIndex );
    uint64_t LoopCrossFadeSamples( float fraction );

    void Trigger( void ){ envelopeMode = t_EnvelopeAttackMode; }
    void Release( void ){ envelopeMode = t_EnvelopeReleaseMode; }
    void Sustain( void ){ envelopeMode = t_EnvelopeSustainMode; }
    void Idle( void ){ envelopeMode = t_EnvelopeIdle; }

    void SetAttackTime( float aTime ){ attackTime = aTime; attackSamples = aTime * sampleRate; attackRate = 1.0 / attackSamples; }
    void SetReleaseTime( float aTime ){ releaseTime = aTime; releaseSamples = aTime * sampleRate; releaseRate = 1.0 / releaseSamples; }

    float ApplyEnvelope( cPlayer *player, ofSoundBuffer& buffer, float masterVolume, float slewRate, int32_t *silenceCount = nullptr );

//    float ApplyEnvelope( ofSoundBuffer& buffer, double position, float inEnvelope, T_EnvelopeMode *envelopeMode, float currentLevel = 1.0, float slewRate = .0001, float targetLevel = 1.0, int32_t *silenceCount = nullptr, float releasePower = 1.0 );
    void ScaleBufferRange( ofSoundBuffer& buffer, size_t start, size_t end, float scaler );
    void ZeroBufferRange( ofSoundBuffer& buffer, size_t start, size_t end );

    void LoadFromJSONSettings( const Json::Value &soundDefJSON, string samplePathBase );
    void UpdateJSONSettings( Json::Value& container, int soundDefIndex, bool useFullPath, string synthBase );

    void GetSaveSoundDefFile(string prompt);
    void SaveSelfToJSONFile(string path);
    void GetLoadSoundDefFile(string prompt);
    void LoadSelfFromJSONFile(string path);
    
    float FFTNormalize( float bias = 1.0, float maximum = 1.0, float weight = 1.0 );
    float DiffNormalize( float scaler, float normBalance );
};


/*class cSliderAmp : public cSliderParam
{
public:
    virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; slider -> setValue( SoundDef() -> amplitude ); }
    virtual void ParameterChanged(float & f){ if( SoundDef() == nullptr ) return; SoundDef() -> amplitude = f; }
};*/

/*class cSliderPitch : public cSliderParam
{
public:
    virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; slider -> setValue( SoundDef() -> pitch ); }
    virtual void ParameterChanged(float & f){ if( SoundDef() == nullptr ) return; SoundDef() -> pitch = f; }
};*/

/*class cSliderLoopStart : public cSliderParam
{
public:
    virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; slider -> setValue( static_cast<double>( SoundDef() -> startLoopSample ) / static_cast<double>( SoundDef() -> sampleCount ) ); }
	virtual void ParameterChanged(float & f){ if( SoundDef() == nullptr ) return; uint64_t start = static_cast<uint64_t>( f * static_cast<float>( SoundDef() -> sampleCount ) ); SoundDef() -> SetLoopStart( start ); }
};*/


/*class cSliderLoopEnd : public cSliderParam
{
public:
    virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; slider -> setValue( static_cast<double>( SoundDef() -> endLoopSample ) / static_cast<double>( SoundDef() -> sampleCount ) ); }
	virtual void ParameterChanged(float & f){ if( SoundDef() == nullptr ) return; uint64_t end = static_cast<uint64_t>( f * static_cast<float>( SoundDef() -> sampleCount ) ); SoundDef() -> SetLoopEnd( end ); }
};*/

/*class cSliderLoopCrossFade : public cSliderParam
{
public:
    virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; slider -> setValue( static_cast<double>( SoundDef() -> loopCrossFadeFraction ) ); }
        virtual void ParameterChanged(float & f){ if( SoundDef() == nullptr ) return; SoundDef() -> SetLoopCrossFadeFraction( f ); }
};*/

/*class cSliderSampleStart : public cSliderParam
{
public:
    virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; slider -> setValue( static_cast<double>( SoundDef() -> startSample ) / static_cast<double>( SoundDef() -> sampleCount ) ); }
	virtual void ParameterChanged(float & f){ if( SoundDef() == nullptr ) return; uint64_t start = static_cast<uint64_t>( f * static_cast<float>( SoundDef() -> sampleCount ) ); SoundDef() -> SetSampleStart( start ); }
};*/

/*class cSliderSampleEnd : public cSliderParam
{
public:
    virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; slider -> setValue( static_cast<double>( SoundDef() -> endSample ) / static_cast<double>( SoundDef() -> sampleCount ) ); }
	virtual void ParameterChanged(float & f){ if( SoundDef() == nullptr ) return; uint64_t end = static_cast<uint64_t>( f * static_cast<float>( SoundDef() -> sampleCount ) ); SoundDef() -> SetSampleEnd( end ); }
};*/

/*class cSliderAttack : public cSliderParam
{
public:
    virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; slider -> setValue( static_cast<double>( SoundDef() -> attackTime ) ); }
    virtual void ParameterChanged(float & f){ if( SoundDef() == nullptr ) return; SoundDef() -> SetAttackTime(f); }
};*/

/*class cSliderDecay : public cSliderParam
{
public:
    virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; slider -> setValue( static_cast<double>( SoundDef() -> releaseTime ) ); }
    virtual void ParameterChanged(float & f){ if( SoundDef() == nullptr ) return; SoundDef() -> SetReleaseTime(f);}
};*/

/*class cToggleLoop : public cToggleParam
{
public:
	virtual void ExtractValue(void){ if( SoundDef() == nullptr ) return; toggle -> setChecked( SoundDef() -> loop ); }
	virtual void ParameterChanged(bool & f){ if( SoundDef() == nullptr ) return; SoundDef() -> loop = f;}
};*/



#endif // CSOUNDDEFINITION_H
