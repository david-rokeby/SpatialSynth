//
//  cSampleSynth.cpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-25.
//

#include "cSampleSynth.hpp"
//#include "vns.hpp"
#include "ofSoundStream.h"
#include "ofSystemUtils.h"


// NOTES:
// MUSEUM INSTALL
// on slow mac mini, gui drawing takes significant cpu power
// turning off the gui is important
// also, strange crashing of this app when kinectVoxelApp is heavily processing (so we send a message to stop and start the kinect processing

cSampleSynth::cSampleSynth( int numSoundDefs, int numPlayers, int numObjects )
{
    readyToReceive = false;
    startStopOSC = new cOSCSender("127.0.0.1", 2502, false);
    startStopOSC -> Connect();
    ofxOscMessage m;
    m.setAddress("/stop"); // this is a hack to force KinectVoxelApp to pause processing to allow slow Mac mini to load samples from a slow hard-drive
    // it would be useful to always have a protocol for remote control and messaging.
    
    oscPort = 2500;
    receiver.setup(oscPort);
    receivedOSC = false;
    startStopOSC -> SendMessage(m);
	samplePathBase = "";
	for( int i = 0; i < numPlayers; i ++ )
		{
		cPlayer *tempPlayer = new cPlayer;
		players.push_back(tempPlayer);
		}
	for( int i = 0; i < numSoundDefs; i ++ )
		{
		cSoundDefinition *tempSoundDef = new cSoundDefinition;
		soundDefs.push_back(tempSoundDef);
		}
	for( int i = 0; i < numObjects; i ++ )
		{
		cSoundObject *tempSoundObject = new cSoundObject( this );
		soundObjects.push_back(tempSoundObject);
		}
	for( int i = 0; i < numObjects; i ++ )
		{
		assignedPlayers.push_back(static_cast<cPlayer *>( nullptr ) );
		}
	editor.SetOwner( this );
	masterVolume = 1.0;
	channelOffset = 0;
    LoadFromJSONSettings(string(""));
    normBalance = 1.0f;
    impulseLimit = 2.0f;
    complexityWeight = 0.0f;
    complexitySpectrumWeight = 0.0f;
    fftsReady = false;
    normalizeReady = false;
    inactiveThreshold = 0.0;
    
    defaultSoundDef = new cSoundDefinition;
    defaultSoundObject = new cSoundObject(this);
//    ofAddListener(receiver.onMessageReceived, this, &cSampleSynth::onMessageReceived);
 //   editor.AttachToSoundObjectByID(0);
}


void cSampleSynth::Clear(void)
{
    int numSoundDefs;
    int numObjects;

    numSoundDefs = soundDefs.size();
    numObjects = soundObjects.size();
    
    for( int i = 0; i < numSoundDefs; i ++ )
    {
        if( soundDefs[i] != nullptr)
            delete soundDefs[i];
    }
    for( int i = 0; i < numObjects; i ++ )
    {
        if( soundObjects[i] != nullptr)
            delete soundObjects[i];
    }

    soundDefs.clear();
    soundObjects.clear();

    for( int i = 0; i < numSoundDefs; i ++ )
        {
        cSoundDefinition *tempSoundDef = new cSoundDefinition;
        soundDefs.push_back(tempSoundDef);
        }
    for( int i = 0; i < numObjects; i ++ )
        {
        cSoundObject *tempSoundObject = new cSoundObject( this );
        soundObjects.push_back(tempSoundObject);
        }
}


cSampleSynth::~cSampleSynth( void )
{
    if( startStopOSC != nullptr)
    {
        startStopOSC -> Disconnect();
        delete startStopOSC;
    }
	ofSoundStreamClose();

	for( vector<cPlayer *>::iterator it = players.begin() ; it != players.end(); ++ it)
		{
		cPlayer *tempPlayer = *it;
		if( tempPlayer )
			{
			tempPlayer -> Stop();
			delete tempPlayer;
			}
		}
	for( vector<cSoundObject *>::iterator it = soundObjects.begin() ; it != soundObjects.end(); ++ it)
		{
		cSoundObject *tempSoundObject = *it;
		if( tempSoundObject )
			delete tempSoundObject;
		}
	for( vector<cSoundDefinition *>::iterator it = soundDefs.begin() ; it != soundDefs.end(); ++ it)
		{
		cSoundDefinition *tempSoundDef = *it;
		if( tempSoundDef )
			delete tempSoundDef;
		}
}

void cSampleSynth::Setup( void )
{
	audiofile.setVerbose(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	// we can get devices
	devices = ofSoundStreamListDevices();
	
	bool found = false;
	int chosen = 0;
	int foundChannels = 0;
    for( int i = 0; i < devices.size(); i ++ )
		{
        deviceNames.push_back( devices[ i ].name );
		if( devices[ i ].outputChannels > foundChannels )
			{
			found = true;
			foundChannels = devices[ i ].outputChannels;
			deviceName = devices[ i ].name;
			chosen = i;
			}
        }
    SelectDevice(chosen);
    
/*    found = true;
    currentDevice = chosen;
	// audio setup for testing audio file stream
	ofSoundStreamSettings settings;
	
	if( found == true )
		{
        settings.setOutDevice( devices[ chosen ] );
		settings.numOutputChannels = devices[ chosen ].outputChannels;
		if( devices[ chosen ].name == "MOTU: MOTU UltraLite mk3 Hybrid"	)
			{
			if( devices[ chosen ].outputChannels >= 10 ) // HACK!!!!
				{
				settings.numOutputChannels = 10;
				channelOffset = 2;
				}
			}
		else if( devices[ chosen ].name == "MOTU: UltraLite-mk4" )
			{
			if( devices[ chosen ].outputChannels >= 10 ) // HACK!!!!
				{
				settings.numOutputChannels = 10;
				channelOffset = 2;
				}
			}
		else
			{
			if( devices[ chosen ].outputChannels >= 8 ) // HACK!!!!
				settings.numOutputChannels = 8;
			}
		}
	else
		settings.numOutputChannels = 2;
	
	for( int i = 0; i < soundObjects.size(); i ++ )
		soundObjects[ i ] -> mix.channelOffset = channelOffset;
	
	outChannelCount = settings.numOutputChannels;
	
	float sampleRate = 44100.0;
	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.numInputChannels = 0;
	settings.bufferSize = 512;
	
	ofSoundStreamSetup(settings);*/
	
	editor.SetupGUI();
	editor.AttachToSoundObjectByID( 0 );
    ofSoundStreamStart();
}

void cSampleSynth::SelectDeviceByName( string deviceName )
{
    for( int32_t i = 0; i < devices.size(); i ++ )
    {
        if( devices[i].name == deviceName )
        {
            SelectDevice(i);
            return;
        }
    }
}

void cSampleSynth::SelectDevice( int32_t deviceChoice )
{
    int foundChannels = devices[deviceChoice].outputChannels;
    deviceName = devices[deviceChoice].name;
    currentDevice = deviceChoice;
    
    ofSoundStreamStop();
    ofSoundStreamClose();
    
    ofSoundStreamSettings settings;
    
    settings.setOutDevice( devices[ deviceChoice ] );
    settings.numOutputChannels = devices[ deviceChoice ].outputChannels;
    if( devices[ deviceChoice ].name == "MOTU: MOTU UltraLite mk3 Hybrid"    )
    {
        if( devices[ deviceChoice ].outputChannels >= 10 ) // HACK!!!!
        {
            settings.numOutputChannels = 10;
            channelOffset = 2;
        }
    }
    else if( devices[ deviceChoice ].name == "MOTU: UltraLite-mk4" )
    {
        if( devices[ deviceChoice ].outputChannels >= 10 ) // HACK!!!!
        {
            settings.numOutputChannels = 10;
            channelOffset = 2;
        }
    }
    else
    {
        if( devices[ deviceChoice ].outputChannels >= 8 ) // HACK!!!!
            settings.numOutputChannels = 8;
    }
    
    if( settings.numOutputChannels < outChannelCount )
        outChannelCount = settings.numOutputChannels;
    
    if( channelOffset + outChannelCount > settings.numOutputChannels )
        channelOffset = 0;
    
    for( int i = 0; i < soundObjects.size(); i ++ )
        soundObjects[ i ] -> mix.channelOffset = channelOffset;
    
    float sampleRate = 44100.0;
    settings.setOutListener(this);
    settings.sampleRate = sampleRate;
    settings.numInputChannels = 0;
    settings.bufferSize = 512;
    
    ofSoundStreamSetup(settings);
    editor.ExtractGlobalValuesForDisplay();
    availableChannelCount = settings.numOutputChannels;
}

bool cSampleSynth::ExtractCoordinatedFromSampleName( string sampleName, int32_t *coords)
{
    // format NAME@5_3_0.aif
    vector<string> splitString = ofSplitString( sampleName, "@");
    
    if( splitString.size() > 1 )
    {
        vector<string> coordString = ofSplitString( splitString[1], "_");

        if(coordString[0].c_str()[0] >= '0' && coordString[0].c_str()[0] <= '9')
        {
            coords[0] = stoi(coordString[0]);
        }
        else
        {
            return false;
        }
        if(coordString[1].c_str()[0] >= '0' && coordString[1].c_str()[0] <= '9')
        {
            coords[1] = stoi(coordString[1]);
        }
        else
        {
            return false;
        }
        if(coordString[2].c_str()[0] >= '0' && coordString[2].c_str()[0] <= '9')
        {
            coords[2] = stoi(coordString[2]);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}


void cSampleSynth::CreateFromFolder(void)
{
    fftsReady = normalizeReady = false;
    *defaultSoundObject = *(soundObjects[0]);
    
    //have to clear these or it really messes things up!
    defaultSoundObject -> soundDefs.clear();
    defaultSoundObject -> soundDefIDs.clear();
    
    *defaultSoundDef = *(soundDefs[0]);
    defaultSoundDef -> buffer = nullptr;

    ofFileDialogResult result = ofSystemLoadDialog("Select a folder of samples", true);
    if(result.bSuccess)
    {
        Clear();
        ofDirectory dir(result.getPath());
        samplePathBase = result.getPath();
        dir.allowExt("aif");
        dir.allowExt("wav");
//        dir.allowExt("aiff");
        dir.listDir();
        
        dir.sort();
//        vector<int>sampleIndex;
        
        // if sample names start with ? then interpret as row.column.depth
        // how to indicate addressed sample?
        // 5@5@2@ (z, y, x)
        // in this case, we populate designated samples first then fill the rest
        // in this case also, we must keep a list of allocated slots
        
        // we use the
        // int32_t leftRightCount;
        // int32_t topBottomCount;
        // int32_t frontBackCount;
        
        int32_t expectedCount = editor.leftRightCount * editor.topBottomCount * editor.frontBackCount;
        if( dir.size() > expectedCount )
            expectedCount = dir.size();
        vector<bool> assigned;
        vector<int32_t> soundObjectIDToSampleIndex;
        vector<int32_t> sampleToSoundObjectIDIndex;
        assigned.resize(expectedCount, false);
        soundObjectIDToSampleIndex.resize(expectedCount, -1);
        sampleToSoundObjectIDIndex.resize(dir.size(), -1);
        bool atAllocated = false;
        
        for(int32_t i = 0; i < dir.size(); i ++)
        {
            int32_t coords[3];
            string name = dir[i].getFileName();
            if( ExtractCoordinatedFromSampleName(name, coords) )
            {
                int32_t offset = (coords[0] * editor.topBottomCount + coords[1]) * editor.leftRightCount + coords[2];
                if(assigned[offset])
                    printf("duplicate sample coords %s", name.c_str());
                assigned[offset] = true;
                soundObjectIDToSampleIndex[offset] = i;
                sampleToSoundObjectIDIndex[i] = offset;
                atAllocated = true;
            }
        }
        
        int32_t autoAssignOffset = 0;
        
        for(int32_t i = 0; i < dir.size(); i ++)
        {
            bool didAssign = false;
            if( sampleToSoundObjectIDIndex[i] == -1 ) //sample not yet assigned
            {
                for(int32_t j = autoAssignOffset; j < expectedCount; j ++)
                {
                    if(soundObjectIDToSampleIndex[j] == -1)   // IF SLOT IS NOT YET ASSIGNED, we can use this
                    {
                        assigned[j] = true;
                        soundObjectIDToSampleIndex[j] = i;
                        sampleToSoundObjectIDIndex[i] = j;
                        autoAssignOffset = j + 1;
                        didAssign = true;
                        break;
                    }
                }
                if(didAssign == false) // no remaining slots
                    break;
            }
 //           sampleIndex.push_back( i );
        }
    
        
        if(editor.randomizeSamples)
            random_shuffle(soundObjectIDToSampleIndex.begin(), soundObjectIDToSampleIndex.end());
        
        int soundObjectID = 0;
        
        for(int32_t j = 0; j < dir.size(); j ++)
        {
            soundObjectID = sampleToSoundObjectIDIndex[j];
            if( soundObjectID >= soundObjects.size())
                soundObjects.resize(soundObjectID + 16);
            
         //   int i = soundObjectIDToSampleIndex[j];
            
         //   if(i == -1)
            //    continue;
            string path = samplePathBase;
            path.append( "/" );
            path.append( dir[j].getFileName() );
            
            // if sampleName excluding suffix matches those of existing soundObject
            // FIND SIMILAR SOUND DEF SAMPLE NAMES
            // IF FOUND, ADD TO EXISTING SOUND OBJECT THAT HOLDS THOSE EXISTING SAMPLE DEFS
            // add as slot in that SoundObject
//            soundDefs[i]->loop = true;
            
            const char *localPathName = path.c_str();
            size_t len = strlen( localPathName );
            size_t stringOffset = 0;
            for( size_t i = len - 1; i > 0; i -- )
            {
                if( localPathName[i] == '/' )
                {
                    stringOffset = i + 1;
                    break;
                }
            }
            
            int similar = SeekSimilarSampleNames(localPathName + stringOffset);
 //           soundDefs[i]-> ReleaseBuffer();
            *(soundDefs[j]) = *defaultSoundDef;
            soundDefs[j]->LoadSample(path, true, defaultSoundDef);

            if(similar == -1 || editor.stackSamples == false)
            //create new sound object
            {
                *(soundObjects[soundObjectID]) = *defaultSoundObject;
                soundObjects[soundObjectID]->SetSoundDefByID(j);
        //        soundObjectID ++;
        //        if( soundObjectID >= soundObjects.size())
        //            soundObjects.resize(soundObjectID + 16);
        //        soundObjects[soundObjectID]->SetSoundDefByID(i);
                soundObjects[soundObjectID]->currentSoundDefIndex = 0;
            }
            else
            {
                size_t currentSoundDefsInObject = soundObjects[similar]->soundDefs.size();
                soundObjects[similar]->AddSoundDefSlot(currentSoundDefsInObject);
                soundObjects[similar]->SetSoundDefByID(j, currentSoundDefsInObject);
                soundObjects[similar]->currentSoundDefIndex = 0;
            }
        }
        editor.AttachToSoundObjectByID(0);
        
        NormalizeSamples();
    }
}


void cSampleSynth::NormalizeSamples( void )
{
    if( fftsReady == false )
    {
        FFTNormalizeSamples();
        fftsReady = true;
    }
    if(normalizeReady == false)
    {
        for( int32_t i = 0; i < soundDefs.size(); i ++ )
        {
            soundDefs[i] -> amplitude = soundDefs[i] -> DiffNormalize( 4.0, normBalance );
        }
        normalizeReady = true;
    }
    for( int32_t i = 0; i < soundDefs.size(); i ++ )
    {
        float balancedGain = soundDefs[i] -> energyGain * normBalance + soundDefs[i] -> normalizeGain * (1.0 - normBalance);
        float complexityGain = (soundDefs[i] -> spread * complexityWeight + (1.0 - complexityWeight) );
        // N.B. complexitySpectrumWeight reverts to zero as complexity reduces...
        float spectrumFactor = 0.0;
        if(soundDefs[i] -> spread > 4)
        {
            spectrumFactor = (soundDefs[i] -> spread - 3) / 2.0;
            if( spectrumFactor > 1.0 )
                spectrumFactor = 1.0;
        }
        float spectralGain = ( ( soundDefs[i] -> averageBin * complexitySpectrumWeight + (1.0 - complexitySpectrumWeight) ) * spectrumFactor ) + (1.0 - spectrumFactor);
        soundDefs[i] -> amplitude = balancedGain * complexityGain * spectralGain;
    }
}


void cSampleSynth::FFTNormalizeSamples( void )
{
    for( int32_t i = 0; i < soundDefs.size(); i ++ )
    {
        if(soundDefs[i] -> duration > 0.0 )
        {
            float spread = soundDefs[i] -> FFTNormalize();
            soundDefs[i] -> spread = spread;
        }
    }
}


int cSampleSynth::SeekSimilarSampleNames(string testName)
{
    string nameBase;
    int similarSoundObject = -1;
    
    size_t pos = testName.find_last_of('_');
    if(pos != string::npos)
        nameBase = testName.substr(0, pos + 1);
    else
        nameBase = testName;
    
    int similarDef = -1;
    
    for( size_t i = 0; i < soundDefs.size(); i ++ )
    {
        if(soundDefs[i])
        {
            size_t samePos = soundDefs[i] -> sampleName.find(nameBase);
            if(samePos == 0)
            {
            //found a similar name
                similarDef = i;
                break;
            }
        }
    }
    
    if( similarDef != -1 )
    {
        for( size_t j = 0; j < soundObjects.size(); j ++ )
        {
            if(soundObjects[j])
            {
                cSoundDefinition *def = soundObjects[j] -> SoundDef();
                if(def)
                {
                    size_t samePos = def -> sampleName.find(nameBase);
                    if(samePos == 0)
                    {
                        similarSoundObject = j;
                        //add this soundDef to this soundObject
                        break;
                    }
                }
            }
        }
    }
    return similarSoundObject;

}

void cSampleSynth::Update( void )
{
    if( receiver.hasWaitingMessages())
    {
        ofxOscMessage message;
        while(receiver.getNextMessage(message))
        {
            onMessageReceived(message);
        }
    }
	editor.UpdateGUI();
    if( readyToReceive == false )
        {
        ofxOscMessage m;
        readyToReceive = true;
        m.setAddress("/start");
        startStopOSC -> SendMessage(m);
        }
    
}


void cSampleSynth::Draw( void )
{
	editor.ShowGUI();
}


void cSampleSynth::ReleasePlayer(cPlayer *inPlayer)
{
    int32_t soundObjID = inPlayer->soundObjectID;
    if(soundObjID >= 0 && soundObjID < assignedPlayers.size())
    {
        if(!inPlayer -> detached)
            assignedPlayers[soundObjID] = nullptr;
    }
    inPlayer->Release();
}

void cSampleSynth::FillBuffer( ofSoundBuffer& buffer )
{
	for( vector<cPlayer *>::iterator it = players.begin() ; it != players.end(); ++ it)
		{
		cPlayer *tempPlayer = *it;
		if( tempPlayer )
			{
			if( tempPlayer -> active && tempPlayer -> soundObject != nullptr )
				{
				tempPlayer -> AddToBuffer( buffer, masterVolume );
				if( tempPlayer -> active == false )
                    ReleasePlayer(tempPlayer);
				}
			}
		}
//	cSoundDefinition *soundDef = editor.SoundDef();
//	if( soundDef && soundDef -> SampleLoaded() )
//		editor.FillBuffer( buffer );
}


cPlayer *cSampleSynth::ChooseIdlePlayer( void )
{
	for( vector<cPlayer *>::iterator it = players.begin() ; it != players.end(); ++ it)
		{
		cPlayer *tempPlayer = *it;
		if( tempPlayer )
			{
			if( tempPlayer -> claimed == false )
				{
				tempPlayer -> claimed = true;
				return tempPlayer;
				}
			}
		}
    
    // then check for idle but not yet released...
    for( vector<cPlayer *>::iterator it = players.begin() ; it != players.end(); ++ it)
    {
        cPlayer *tempPlayer = *it;
        if( tempPlayer )
        {
            if( tempPlayer -> envelopeMode == t_EnvelopeIdle )
            {
                tempPlayer -> Release();
                tempPlayer -> claimed = true;
                return tempPlayer;
            }
        }
    }
	return nullptr;
}

cPlayer *cSampleSynth::Trigger( int inSoundObjectID, float level, bool forceCurrentSoundDefChoice )
{
	if( inSoundObjectID < soundObjects.size() )
    {
		cSoundObject *thisSoundObj = soundObjects[ inSoundObjectID ];
        float cTime = ofGetElapsedTimef();
        if(cTime - thisSoundObj -> lastTriggeredTime < thisSoundObj -> retriggerDelay)
            return nullptr;
        thisSoundObj -> lastTriggeredTime = cTime;
        if( thisSoundObj -> SoundDef() != nullptr )
        {
            cPlayer *chosenPlayer = ChooseIdlePlayer();
            if( chosenPlayer )
            {
                assignedPlayers[inSoundObjectID]=chosenPlayer;
                chosenPlayer -> Trigger( thisSoundObj, inSoundObjectID, level, forceCurrentSoundDefChoice );
                return chosenPlayer;
            }
        }

    }
	return nullptr;
}


cPlayer *cSampleSynth::PlayInteractive( int inSoundObjectID, float level )
{
	if( inSoundObjectID < soundObjects.size() )
		{
		cSoundObject *thisSoundObj = soundObjects[ inSoundObjectID ];
        float cTime = ofGetElapsedTimef();
        if(cTime - thisSoundObj -> lastTriggeredTime < thisSoundObj -> retriggerDelay)
            return nullptr;
        thisSoundObj -> lastTriggeredTime = cTime;
		if( thisSoundObj -> SoundDef() != nullptr )
			{
				cPlayer *chosenPlayer = ChooseIdlePlayer();
				if( chosenPlayer )
					{
					assignedPlayers[inSoundObjectID]=chosenPlayer;
					chosenPlayer -> PlayFromCollisionList( thisSoundObj, inSoundObjectID, level );
					return chosenPlayer;
					}
			}
		}
	return nullptr;
}


void cSampleSynth::UpdateJSONSettings(Json::Value& synthDef)
{
    bool useFullPath;

    // do sound device settings here.
    //	getOutDevice
    //	const ofSoundDevice *outDevice
    synthDef["synth"]["device_name"] = deviceName;
    synthDef["synth"]["num_output_channels"] = outChannelCount;
    synthDef["synth"]["channel_offset"] = channelOffset;
    
    synthDef["synth"]["show_gui"] = editor.visible;

    synthDef["synth"]["master_volume"] = masterVolume;
    synthDef["synth"]["impulse_limit"] = impulseLimit;
    
    synthDef["synth"]["normalization_balance"] = normBalance;
    synthDef["synth"]["complexity_factor"] = complexityWeight;
    synthDef["synth"]["complexity_spectrum_weight"] = complexitySpectrumWeight;

    synthDef["synth"]["release_hold"] = inactiveThreshold;

    synthDef["synth"]["distribution_width"] = editor.leftRightCount;
    synthDef["synth"]["distribution_height"] = editor.topBottomCount;
    synthDef["synth"]["distribution_depth"] = editor.frontBackCount;

    synthDef["synth"]["osc_port"] = oscPort;

    synthDef["synth"]["soundDefCount"] = (Json::Value::UInt64)soundDefs.size();
    synthDef["synth"]["samplePathBase"] = samplePathBase;
    
    if( samplePathBase.size() == 0 )
        useFullPath = true;

    for( size_t i = 0; i < soundDefs.size(); i ++ )
        {
        soundDefs[ i ] -> UpdateJSONSettings( synthDef["synth"], i, useFullPath, samplePathBase );
        }

    synthDef["synth"]["soundObjectCount"] = (Json::Value::UInt64)soundObjects.size();
    for( size_t i = 0; i < soundObjects.size(); i ++ )
        {
        soundObjects[ i ] -> UpdateJSONSettings( synthDef["synth"], i, false );
        }

}


void cSampleSynth::SaveToJSONSettings( string path )
{
	bool useFullPath = false;
    Json::Value synthDef;
    UpdateJSONSettings(synthDef);

    if(path.length() == 0)
        currentSynthFilePath = "synth_settings.json";
    else
        currentSynthFilePath = path;
 //   Settings::get().save(currentSynthFilePath);
    ofxJSONElement def(synthDef);
    def.save(currentSynthFilePath, true);
 }


void cSampleSynth::GetLoadSynthFile(string prompt)
{
    ofFileDialogResult result = ofSystemLoadDialog(prompt);
    if(result.bSuccess)
        LoadFromJSONSettings(result.getPath());
}


void cSampleSynth::GetSaveSynthFile(string prompt)
{
    ofFileDialogResult result = ofSystemSaveDialog(string("synth_settings.json"), prompt);
    if(result.bSuccess)
        SaveToJSONSettings(result.getPath());
}


void cSampleSynth::LoadFromJSONSettings( string path )
{
    fftsReady = normalizeReady = false;
	ofxJSON root;
	samplePathBase = "";
    bool success = false;

    if( path.length() == 0 )
        success = root.open(string("synth_settings.json"));
    else
        success = root.open(path);

	if( success )
		{
        if( path.length() == 0 )
            currentSynthFilePath = "synth_settings.json";
        else
            currentSynthFilePath = path;

		const Json::Value synth = root[ "synth" ];
		if( synth.type() != Json::nullValue )
        {
            const Json::Value deviceNameVal = synth[ "device_name" ];
            if( deviceNameVal.type() == Json::stringValue )
            {
                deviceName = deviceNameVal.asString();
                SelectDeviceByName( deviceName );
            }
            
            const Json::Value outChannelCountVal = synth[ "num_output_channels" ];
            if( outChannelCountVal.type() == Json::intValue )
            {
                int32_t desiredChannelCount = outChannelCountVal.asInt();
                if( desiredChannelCount <= availableChannelCount )
                    outChannelCount = desiredChannelCount;
                // SET !!!!
            }
            
            const Json::Value channelOffsetVal = synth[ "channel_offset" ];
            if( channelOffsetVal.type() == Json::intValue )
            {
                channelOffset = channelOffsetVal.asInt();
                if( channelOffset + outChannelCount > availableChannelCount )
                    channelOffset = 0;
                for( int i = 0; i < soundObjects.size(); i ++ )
                    soundObjects[ i ] -> mix.channelOffset = channelOffset;
            }
            
            const Json::Value showGUIVal = synth[ "show_gui" ];
            if( showGUIVal.type() == Json::booleanValue )
                editor.visible = showGUIVal.asBool();
            
            const Json::Value impulseLimitVal = synth[ "impulse_limit" ];
            if( impulseLimitVal.type() == Json::realValue )
                impulseLimit = impulseLimitVal.asFloat();
            
            const Json::Value normBalanceVal = synth[ "normalization_balance" ];
            if( normBalanceVal.type() == Json::realValue )
                normBalance = normBalanceVal.asFloat();
            
            const Json::Value complexityFactorVal = synth[ "complexity_factor" ];
            if( complexityFactorVal.type() == Json::realValue )
                complexityWeight = complexityFactorVal.asFloat();
            
            const Json::Value complexitySpectrumWeightVal = synth[ "complexity_spectrum_weight" ];
            if( complexitySpectrumWeightVal.type() == Json::realValue )
                complexitySpectrumWeight = complexitySpectrumWeightVal.asFloat();
            
            const Json::Value inactiveThresholdVal = synth[ "release_hold" ];
            if( inactiveThresholdVal.type() == Json::realValue )
                inactiveThreshold = inactiveThresholdVal.asFloat();
            
            const Json::Value distributionWidthVal = synth[ "distribution_width" ];
            if( distributionWidthVal.type() == Json::intValue )
                editor.leftRightCount = distributionWidthVal.asInt();

            const Json::Value distributionHeightVal = synth[ "distribution_height" ];
            if( distributionHeightVal.type() == Json::intValue )
                editor.topBottomCount = distributionHeightVal.asInt();

            const Json::Value distributionDepthVal = synth[ "distribution_depth" ];
            if( distributionDepthVal.type() == Json::intValue )
                editor.frontBackCount = distributionDepthVal.asInt();

            const Json::Value oscPortVal = synth[ "osc_port" ];
            if( oscPortVal.type() == Json::intValue )
                oscPort = oscPortVal.asInt();
            
            //string inPath = "";
            const Json::Value pathNameBaseValue = synth[ "samplePathBase" ];
            if( pathNameBaseValue.type() == Json::stringValue )
                samplePathBase = pathNameBaseValue.asString();

            const Json::Value soundDefCountVal = synth[ "soundDefCount" ];
			if( soundDefCountVal.type() != Json::nullValue )
				{
				uint64_t soundDefCount = soundDefCountVal.asUInt64();
				const Json::Value soundDefList = synth[ "soundDef" ];
				if( soundDefList.type() != Json::nullValue )
					{
					for( size_t i = 0; i < soundDefCount; i ++ )
						{
						const Json::Value thisSoundDef = soundDefList[ to_string( i ) ];
						if( thisSoundDef.type() != Json::nullValue )
							soundDefs[ i ] -> LoadFromJSONSettings( thisSoundDef, samplePathBase );
						}
					}
				}
			
			const Json::Value soundObjectCountVal = synth[ "soundObjectCount" ];
			if( soundObjectCountVal.type() != Json::nullValue )
				{
				uint64_t soundObjectCount = soundObjectCountVal.asUInt64();
				const Json::Value soundObjectList = synth[ "soundObject" ];
				if( soundObjectList.type() != Json::nullValue )
					{
					for( size_t i = 0; i < soundObjectCount; i ++ )
						{
						const Json::Value thisSoundObject = soundObjectList[ to_string( i ) ];
						if( thisSoundObject.type() != Json::nullValue )
							soundObjects[ i ] -> LoadFromJSONSettings( thisSoundObject );
						}
					}
				}
			}
		}
    
    editor.ExtractGlobalValuesForDisplay();
}


int32_t cSampleSynth::FindEmptySoundDef(void)
{
    for(int i = 0; i < soundDefs.size(); i ++)
    {
        if(soundDefs[i]->buffer==nullptr)
            return i;
    }
    return -1;

}

void cSampleSynth::HandleKeys( int key )
{
	if( key == ' ')
		{
		editor.TogglePlay();
		}
	if( key == '/')
		{
		Trigger( 0 );
		}
	if( key == 'd')
		{
            if( editor.visible )
            {
                editor.Hide();
            }
            else
            {
                editor.Show();
            }
              
//		if( editor.ToggleUI())
//			ofSetWindowShape(1024, windowHeight + editor.guiHeight);
//		else
//			ofSetWindowShape(1024, windowHeight);
		}
	if( key == 's')
		{
        Settings settings;
        SaveToJSONSettings(string(""));
		}
	if( key == 'r')
		{
        LoadFromJSONSettings(string(""));
		}
}

void cSampleSynth::MouseDragged( int x, int y, int button )
{
	if( editor.IsVisible() == false || x < ofGetWidth() )
		editor.MouseDragged( x, y, button );
}

void cSampleSynth::MouseScrolled( int scrollY )
{
    if( editor.IsVisible() == true )
        editor.MouseScrolled( scrollY );
}

void cSampleSynth::MousePressed( int x, int y, int button )
{
	if( editor.IsVisible() == false || x < ofGetWidth() )
		editor.MousePressed( x, y, button );
}

void cSampleSynth::MouseReleased( int x, int y, int button )
{
	if( editor.IsVisible() == false || x < ofGetWidth() )
		editor.MouseReleased( x, y, button );
}

void cSampleSynth::audioOut(ofSoundBuffer & buffer)
{
	FillBuffer( buffer );
	//	if( soundEditor )
	//	soundEditor -> FillBuffer( buffer );
}


void cSampleSynth::UpdatePlayers(uint64_t currentTime)
{
	for(cPlayer *p:players)
		{
		if(p->active)
			{
			if( p->playMode == t_collisionListPlayMode)
				p -> UpdateInteraction(currentTime);
			if(p->active==false)
				{
				p->claimed=false;
				assignedPlayers[p->soundObjectID] = nullptr;
				}
			}
		else if(p->claimed && p->soundObject)
			{
			assignedPlayers[p->soundObjectID] = nullptr;
			}
		}
}


void cSampleSynth::onMessageReceived(ofxOscMessage &msg)
{
    string addr = msg.getAddress();
    size_t numArgs = msg.getNumArgs();
    uint64_t currentTime = ofGetSystemTimeMillis();

    if( readyToReceive == false )
        return;
	if(addr.compare("/heart_beat") == 0)
	{
		UpdatePlayers(currentTime);
	}
    else if(addr.compare("/collision_list") == 0)
    {
        receivedOSC = true;
        for(int i = 0; i < numArgs; i +=2)
        {
            int32_t soundObjectID = msg.getArgAsInt32(i);
            if(soundObjectID < soundObjects.size())
            {
                float sense = 1.0 / soundObjects[soundObjectID]-> sensitivity;
                sense = sense * sense;
				float impulse = 0.0;
				if(msg.getArgType(i + 1) == OFXOSC_TYPE_FLOAT)
                	impulse = msg.getArgAsFloat( i + 1 ) * sense;
				else if(msg.getArgType(i + 1) == OFXOSC_TYPE_INT32)
					impulse = ((float)msg.getArgAsInt32( i + 1 )) * sense;
				if(impulse > impulseLimit) //clamp impulse
                    impulse = impulseLimit;
                //locate player for this object
         		if(assignedPlayers[soundObjectID] && assignedPlayers[soundObjectID] -> active )
					assignedPlayers[soundObjectID] -> UpdateLevel(impulse, currentTime);
				else if( soundObjects[soundObjectID] -> SoundDef() != nullptr && impulse > 0 )
				{
					cPlayer *p = PlayInteractive( soundObjectID, impulse );
                    if(p)
                        p -> UpdateLevel(impulse, currentTime);
				}

            }
        }
        UpdatePlayers(currentTime);
    }
    else if(addr.compare("/play") == 0)
    {
        receivedOSC = true;
        if(numArgs > 0)
        {
            int32_t soundObjectID = msg.getArgAsInt32(0);
            if(soundObjectID < soundObjects.size())
            {
                if(assignedPlayers[soundObjectID] && assignedPlayers[soundObjectID] -> active )
                {
                    assignedPlayers[soundObjectID] -> Detach();
                    assignedPlayers[soundObjectID] = nullptr;
                }
                
                float sense = 1.0 / soundObjects[soundObjectID]-> sensitivity;
                sense = sense * sense;
                float impulse = 0.0;
                
                if(numArgs > 1)
                {
                    if(msg.getArgType(1) == OFXOSC_TYPE_FLOAT)
                        impulse = msg.getArgAsFloat(1 ) * sense;
                    else if(msg.getArgType(1) == OFXOSC_TYPE_INT32)
                        impulse = ((float)msg.getArgAsInt32( 1 )) * sense;
                    if(impulse > 1000.f)
                        impulse = 1000.0f;
                    if(impulse > 1.0f)
                        impulse = 1.0f;
                }
                else
                    impulse = 1.0f;
                if( soundObjects[soundObjectID] -> SoundDef() != nullptr && impulse > 0 )
                {
                    cPlayer *p = Trigger( soundObjectID, impulse );
//                    if(p)
 //                       p -> UpdateLevel(impulse, currentTime);
                }
            }
        }
    }
    else if(addr.compare("/stop") == 0)
    {
        receivedOSC = true;
        if(numArgs > 0)
        {
            int32_t soundObjectID = msg.getArgAsInt32(0);
            if(assignedPlayers[soundObjectID] && assignedPlayers[soundObjectID] -> active )
                assignedPlayers[soundObjectID]->SetRelease();
            for(cPlayer *player:players)
            {
                if(player->soundObjectID == soundObjectID)
                {
                    player -> SetRelease();
                }
            }
        }
    }
}


void cSampleSynth::SaveFFT( void )
{
    if( fftsReady == false )
    {
        FFTNormalizeSamples();
        fftsReady = true;
    }

    ofFileDialogResult result = ofSystemSaveDialog(string("soundDefinition_FFTs.txt"), "Save Sound Definition FFTs");
    if(result.bSuccess)
    {
        string path = result.getPath();
        ofFile file(path, ofFile::WriteOnly);
        for( cSoundDefinition *soundDef: soundDefs)
        {
            if( soundDef && soundDef -> duration > 0)
            {
                file << soundDef -> sampleName;
                file << ": ";
                
                for( int32_t i = 0; i < soundDef -> spectrumBuffer.size(); i ++ )
                {
                    file << soundDef -> spectrumBuffer[i];
                    if( i < soundDef -> spectrumBuffer.size() - 1 )
                        file << ' ';
                }
                file << "\n";
            }
        }
        file.close();
    }
}

// on collisions_list
//  for each collision
//      is object active?
//          if so, set new interactiveLevel (with smoothing), adjust faderate to new level
//      if not
//          trigger object and set initial interactionLevel and fadeRate
//
//  for each active player
//      if touched by collisionList... OK
//      else
//          fade interactiveLevel by fadeRate
//          if at zero for a certain period of time, stop play
//          increase age

/*void cSampleSynth::ReceiveImpulses( vector<cImpulse *>impulses )
{
	int count = impulses.size();
	if( count > soundObjects.size() )
		count = soundObjects.size();
	
	//update active levels
	for( int32_t i = 0; i < players.size(); i ++ )
		{
		if( players[ i ] -> active )
			{
			cPlayer *player = players[ i ];
			cImpulse *impulse = impulses[ player -> soundObjectID ];
			if( impulse -> state == t_ImpulseNewRelease )
				{
				impulse -> AckOff();
				player -> SetRelease();
				player -> SetSustain( false );
				}
			player -> currentLevel = impulse -> outputVal;
			}
		}
	
	// look for ending samples
	for( int i = 0; i < count; i ++ )
		{
		cImpulse *impulse = impulses[ i ];
		switch( impulse -> state )
			{
			case t_ImpulseNewRelease:
				//missed (not active?)
				impulse -> AckOff();
				break;
				
			case t_ImpulseNewOnset:
				impulse -> AckOn();
				cSoundObject *sound = soundObjects[ i ];
				if( sound -> SoundDef() != nullptr )
					Play( i, impulse -> outputVal );
				break;
			}
		}
}*/
