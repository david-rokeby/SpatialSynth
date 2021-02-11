//
//  cSampleSynth.hpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-25.
//

#ifndef cSampleSynth_hpp
#define cSampleSynth_hpp
#pragma once
#include <stdio.h>
#include "cSoundDefinition.h"
#include "cPlayer.h"
#include "cSoundEditor.hpp"
#include "ofxJsonSettings.h"
#include "ofxOsc.h"
//#include "vns.hpp"

class cSoundObject;

class cOSCSender
{
public:
    string ipAddress;
    int32_t port;
    bool connected;
    ofxOscSender sender;
    
    cOSCSender(){Reset();}
    cOSCSender(string inIP, int32_t inPort, bool inConnected){ ipAddress = inIP; port = inPort; connected = inConnected; }
    void Reset(void){ipAddress = "127.0.0.1"; port = 2501; connected = false;}
    void Clear(void){ipAddress = "127.0.0.1"; port = 2501; connected = false;}
    bool Connect(void){ sender.clear(); connected = false; if(sender.setup(ipAddress, port)){ connected = true; } return connected;}
    void SendMessage(ofxOscMessage& m){ sender.sendMessage(m);}
    void Disconnect(void){ sender.clear(); connected = false; }
};

class cSampleSynth
{
public:
	string deviceName;
	int outChannelCount;
	int channelOffset;
    int availableChannelCount;
    bool readyToReceive;

    string currentSynthFilePath;
	
	vector<cPlayer *> players;
	vector<cPlayer *> assignedPlayers;
	vector<cSoundDefinition *> soundDefs;
	vector<cSoundObject *> soundObjects;
	cSoundEditor editor;
    cSoundObject *defaultSoundObject;
    cSoundDefinition *defaultSoundDef;

	string samplePathBase;
	ofxAudioFile audiofile;
	float masterVolume;
    float impulseLimit;
    float normBalance;
    float complexityWeight;
    float complexitySpectrumWeight;
    bool fftsReady;
    bool normalizeReady;
    float inactiveThreshold;

    vector<ofSoundDevice> devices;
    vector<string> deviceNames;
    int32_t currentDevice;
    ofxOscReceiver receiver;
    int32_t oscPort;
    bool receivedOSC;
    cOSCSender *startStopOSC;

	cSampleSynth( int numSoundDefs, int numPlayers, int numSoundObjects );
	~cSampleSynth( void );
	void CreateFromFolder(void);
	void Clear(void);
	void Setup( void );
	void Draw( void );
	void Update( void );
	void FillBuffer( ofSoundBuffer& buffer );
//	bool LoadSoundDefinition( cSoundSettings *soundSpec );
    int SeekSimilarSampleNames(string testName);

    cSoundDefinition *GetSoundDefByID( int id ){ if(id == -1) return nullptr; if( id < soundDefs.size() ) return soundDefs[ id ]; return nullptr; }
	cSoundObject *GetSoundObjectByID( int id ){ if( id < soundObjects.size() ) return soundObjects[ id ]; return nullptr; }
	cPlayer *ChooseIdlePlayer( void );
	cPlayer *Trigger( int soundObjectID, bool forceCurrentSoundDefChoice = false ){ return Trigger( soundObjectID, 1.0, forceCurrentSoundDefChoice ); }
	cPlayer *Trigger( int soundObjectID, float level, bool forceCurrentSoundDefChoice = false );
	cPlayer *PlayInteractive( int soundObjectID ){ return PlayInteractive( soundObjectID, 1.0 ); }
	cPlayer *PlayInteractive( int soundObjectID, float level );

	void GetLoadSynthFile(string prompt);
	void GetSaveSynthFile(string prompt);
	void SaveToJSONSettings( string path );
	void UpdateJSONSettings(Json::Value& settings);

	void LoadFromJSONSettings( string path );
	void HandleKeys( int key );
	void MouseDragged( int x, int y, int button );
	void MousePressed( int x, int y, int button );
	void MouseReleased( int x, int y, int button );
	void MouseScrolled( int scrollY );
	void audioOut(ofSoundBuffer & buffer);
//	void ReceiveImpulses( vector<cImpulse *>impulses );
	void onMessageReceived(ofxOscMessage &msg);

	int32_t FindEmptySoundDef(void);
	void ReleasePlayer(cPlayer *inPlayer);
	void UpdatePlayers(uint64_t currentTime);
    
    void NormalizeSamples( void );
    bool ExtractCoordinatedFromSampleName( string sampleName, int32_t *coords );
    void SelectDevice( int32_t deviceChoice );
    void SelectDeviceByName( string deviceName );
    void FFTNormalizeSamples( void );
    void SaveFFT( void );

};
	
	

#endif /* cSampleSynth_hpp */
