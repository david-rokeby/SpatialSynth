//
//  cMix.hpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-26.
//

#ifndef cMix_hpp
#define cMix_hpp

#include <stdio.h>
#include "ofSoundBuffer.h"
#include "ofxJsonSettings.h"


enum T_MixMode
{
	t_MixSimpleStereo,
	t_MixStereoToMulti,
	t_MixStereoToQuadInMulti,
	t_MixFullStereo
};


enum T_QuadChannelSet
{
    t_QuadChannels_1_2_3_4,
    t_QuadChannels_3_4_5_6,
    t_QuadChannels_5_6_7_8,
    t_QuadChannels_Open
};

class cMix
{
public:
	// map source channels to output channels with weights
    
	int inputChannelCount;
	int outputChannelCount;
	float globalLevel;
	float pan;
	float frontBackPan;
	int leftFrontOutChannel;
	int rightFrontOutChannel;
	int leftBackOutChannel;
	int rightBackOutChannel;
    T_QuadChannelSet quadSet;
	int channelOffset; // offset to real channels (MOTU ultralite Analog out 1-8 are channels 3-10
	T_MixMode mode;
	std::vector<int> channelMatrix; //input channel to output channel(s)
	std::vector<float> levels; // level to output channels
	
    cMix( void ){ channelOffset = 0; globalLevel = 1.0; inputChannelCount = 0; outputChannelCount = 0; pan = 0.5; frontBackPan = 0.5; leftFrontOutChannel = 1; rightFrontOutChannel = 2; leftBackOutChannel = 2; rightBackOutChannel = 3; InitStereo(); quadSet = t_QuadChannels_1_2_3_4;}

	void InitStereo( void )
	{
		mode = t_MixSimpleStereo;
		leftFrontOutChannel = 1;
		rightFrontOutChannel = 2;
		pan = 0.5;
		inputChannelCount = outputChannelCount = 2;
		levels.clear();
		levels.push_back( 0.5 );
		levels.push_back( 0.0 );
		levels.push_back( 0.0 );
		levels.push_back( 0.5 );
	}
	
	void InitStereoToMulti( int outChannels )
	{
		mode = t_MixStereoToMulti;
		pan = 0.5;
		leftFrontOutChannel = 1;
		rightFrontOutChannel = 2;
		inputChannelCount = 2;
		outputChannelCount = outChannels;
		levels.clear();
		levels.push_back( 0.5 );
		levels.push_back( 0.0 );
		levels.push_back( 0.0 );
		levels.push_back( 0.5 );
	}
	
	void InitStereoToQuadInMulti( int outChannels )
	{
		mode = t_MixStereoToQuadInMulti;
		leftFrontOutChannel = 1;
		rightFrontOutChannel = 2;
		leftBackOutChannel = 3;
		rightBackOutChannel = 4;
		pan = 0.5;
		frontBackPan = 0.0;
		inputChannelCount = 2;
		outputChannelCount = outChannels;
		levels.clear();
		levels.push_back( 0.5 );
		levels.push_back( 0.5 );
		levels.push_back( 0.0 );
		levels.push_back( 0.0 );
	}

    void SetQuadSet( T_QuadChannelSet set )
    {
        quadSet = set;
        
        switch(quadSet)
        {
            case t_QuadChannels_1_2_3_4:
                leftFrontOutChannel = 1;
                rightFrontOutChannel = 2;
                leftBackOutChannel = 3;
                rightBackOutChannel = 4;
                break;
                
            case t_QuadChannels_3_4_5_6:
                leftFrontOutChannel = 3;
                rightFrontOutChannel = 4;
                leftBackOutChannel = 5;
                rightBackOutChannel = 6;
                break;
                
            case t_QuadChannels_5_6_7_8:
                leftFrontOutChannel = 5;
                rightFrontOutChannel = 6;
                leftBackOutChannel = 7;
                rightBackOutChannel = 8;
                break;
        }
    }
    
	void SetStereoPan( float panPos ){ pan = panPos; levels[ 0 ] = 1.0 - panPos; levels[ 1 ] = 0.0; levels[ 2 ] = 0.0; levels[ 3 ] = panPos; }
	void SetStereoRotateMulti( float rotatePanPos );
	void PlaceStereoInMulti( int left, int right ){ leftFrontOutChannel = left + channelOffset; rightFrontOutChannel = right + channelOffset; }
	void SetStereoPlacedInQuad( float leftRight, float frontBack );
	int RotationMappedChannel( int inChan );
	void SetPan( float p );
	void SetFrontBackPan( float p );
	void SetOutChannelCount( int count );
    
    void Randomize( int32_t outChannels );
    void DistributeLinear( int32_t which , int32_t x, int32_t y, int32_t z, int32_t outChannels );

	float StereoLeftGain( void ){ return levels[ 0 ] * globalLevel; }
	float StereoRightGain( void ){ return levels[ 3 ] * globalLevel; }
	float QuadLeftFrontGain( void ){ return levels[ 0 ] * globalLevel; }
	float QuadLeftBackGain( void ){ return levels[ 2 ] * globalLevel; }
	float QuadRightFrontGain( void ){ return levels[ 1 ] * globalLevel; }
	float QuadRightBackGain( void ){ return levels[ 3 ] * globalLevel; }
	
	void AddMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );
	void AddSimpleStereoMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );
	void AddFullStereoMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );
	void AddPlacedStereoQuadToMultiMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );
	void AddPlacedStereoToMultiMix( ofSoundBuffer& inBuffer, ofSoundBuffer& outBuffer );

        void UpdateJSONSettings( string heirarchy );
        void UpdateJSONSettings( Json::Value& heirarchy );

	void LoadFromJSONSettings( const Json::Value &mixJSON );

};

#endif /* cMix_hpp */
