//
//  cSoundObject.cpp
//  test_audio_file
//
//  Created by David Rokeby on 2018-11-26.
//

#include "cSoundObject.hpp"
#include "cSampleSynth.hpp"
#include "ofSystemUtils.h"


void cSoundObject::ChooseSoundDef( void )
{
    switch( soundDefChoiceMode )
    {
        case t_choiceModeRandom:
            currentSoundDefIndex = rand() % soundDefs.size();
            break;
            
        case t_choiceModeSequential:
            currentSoundDefIndex ++;
            if( currentSoundDefIndex >= soundDefs.size() )
                currentSoundDefIndex = 0;
            break;
            
        default:
            //leave it as it is
            break;
    }
}

void cSoundObject::SetSoundDefByID( int id, int index )
{
    if( synth == nullptr )
        return;
    
    if(index >= soundDefs.size())
    {
        soundDefIDs.resize(index + 1);
        soundDefs.resize(index + 1);
    }
    if(index < soundDefs.size())
    {
        currentSoundDefIndex = index;
        soundDefIDs[index] = id;
        soundDefs[index] = synth -> GetSoundDefByID( id );
    }
}

void cSoundObject::AddSoundDefSlot( int index )
{
    size_t currentSize = soundDefs.size();
    if( index == -1)
       index = currentSize + 1;
    
    if(index >= currentSize )
    {
       soundDefIDs.resize(index + 1);
       soundDefs.resize(index + 1);
    }
    
   int baseSoundDefID = -1;
//   if(currentSize > 0)
//       baseSoundDefID = currentSize - 1;
    
   for(int32_t i = currentSize; i < soundDefs.size(); i ++ )
   {
       soundDefIDs[index] = baseSoundDefID;
       soundDefs[index] = nullptr;
       SetSoundDefByID(baseSoundDefID, index);
   }
}

int cSoundObject::CurrentSoundDefID(void)
{
    if(currentSoundDefIndex > -1)
    {
        if(currentSoundDefIndex < soundDefIDs.size())
            return soundDefIDs[currentSoundDefIndex];
    }
    return -1;
}

cSoundDefinition *cSoundObject::SoundDef( void )
{
    if(currentSoundDefIndex > -1)
    {
        if(currentSoundDefIndex < soundDefs.size())
            return soundDefs[currentSoundDefIndex];
    }
    return nullptr;
    
}
// cSoundObject describes how a cSoundDefinition should be played
//

void cSoundObject::GetSaveSoundObjectFile(string prompt)
{
    ofFileDialogResult result = ofSystemSaveDialog(string("soundObject_settings.json"), prompt);
    if(result.bSuccess)
        SaveSelfToJSONFile(result.getPath());
}


void cSoundObject::SaveSelfToJSONFile(string path)
{
   Json::Value soundObject;

   if(path.length() == 0)
       currentSoundObjectFilePath = "soundObject_settings.json";
   else
       currentSoundObjectFilePath = path;
   UpdateJSONSettings(soundObject, 0, true);

   ofxJSONElement def(soundObject);
   def.save(currentSoundObjectFilePath, true);
}


void cSoundObject::UpdateJSONSettings( Json::Value& container, int32_t index, bool includeDef )
{
    if( soundDefIDs.size() < 1 )
        return;
    string indexStr = to_string(index);
    container["soundObject"][indexStr]["soundDefCount"] = (int64_t)soundDefIDs.size();
    
    for(int32_t i = 0; i < soundDefIDs.size(); i ++ )
        container["soundObject"][indexStr]["soundDefID"][to_string(i)] = soundDefIDs[i];
    
    container["soundObject"][indexStr]["retriggerDelay"] = retriggerDelay;
    container["soundObject"][indexStr]["simpleTrigger"] = (triggerMode == t_ObjectOneShot);
    container["soundObject"][indexStr]["pitch"] = pitch;
    container["soundObject"][indexStr]["smoothing"] = smoothing;
    container["soundObject"][indexStr]["fadeInRate"] = fadeInRate;
    container["soundObject"][indexStr]["fadeOutRate"] = fadeOutRate;
    container["soundObject"][indexStr]["fadeCurve"] = releasePower;
    container["soundObject"][indexStr]["sensitivity"] = sensitivity;
    container["soundObject"][indexStr]["soundDefChoiceMode"] = soundDefChoiceMode;
    container["soundObject"][indexStr]["currentSoundDefIndex"] = currentSoundDefIndex;
    mix.UpdateJSONSettings( container["soundObject"][indexStr] );
    if(includeDef)
    {
        for(int32_t i = 0; i < soundDefIDs.size(); i ++ )
            synth->GetSoundDefByID(soundDefIDs[i])->UpdateJSONSettings(container, soundDefIDs[i], true, synth->samplePathBase);
    }
}


void cSoundObject::GetLoadSoundObjectFile(string prompt)
{
    ofFileDialogResult result = ofSystemLoadDialog(prompt);
    if(result.bSuccess)
        LoadSelfFromJSONFile(result.getPath());
}


void cSoundObject::LoadSelfFromJSONFile(string path)
{
   ofxJSON root;
   bool success = false;

   if( path.length() == 0 )
       success = root.open(string("soundObject_settings.json"));
   else
       success = root.open(path);
   if(path.length() != 0)
       currentSoundObjectFilePath = path;

   const Json::Value soundObject = root[ "soundObject" ]["0"];
   LoadFromJSONSettings(soundObject);

   //NOTE we also have a soundDef in this file
   //we should load it as a new soundDef
   //we need to reconnect the soundDefID
   int32_t fileSoundDefID = soundDefIDs[0];
   int32_t newSoundDefID = synth -> FindEmptySoundDef();
   if( newSoundDefID != -1 && fileSoundDefID >= 0)
   {
       const Json::Value soundDef = root[ "soundDef" ][to_string(fileSoundDefID)];
       synth->soundDefs[newSoundDefID]->LoadFromJSONSettings(soundDef, "");
       SetSoundDefByID(newSoundDefID);
   }
}


void cSoundObject::LoadFromJSONSettings( const Json::Value &soundObjectJSON )
{
    const Json::Value soundDefCountValue = soundObjectJSON[ "soundDefCount" ];
    if( soundDefCountValue.type() != Json::nullValue )
    {
        int32_t soundDefCount = soundDefCountValue.asInt64();
        for(int32_t i = 0; i < soundDefCount; i ++)
        {
            const Json::Value soundDefIDValue = soundObjectJSON[ "soundDefID" ][to_string(i)];
            if( soundDefIDValue.type() != Json::nullValue )
            {
                int32_t thisSoundDefID = soundDefIDValue.asInt64();
                SetSoundDefByID( thisSoundDefID, i );
            }
        }
            
        const Json::Value simpleTriggerValue = soundObjectJSON[ "simpleTrigger" ];
        if( simpleTriggerValue.type() != Json::nullValue )
        {
            bool trig = simpleTriggerValue.asBool();
            if( trig)
                triggerMode = t_ObjectOneShot;
            else
                triggerMode = t_ObjectInteractive;
        }
        
        const Json::Value retriggerDelayValue = soundObjectJSON[ "retriggerDelay" ];
        if( retriggerDelayValue.type() != Json::nullValue )
            retriggerDelay = retriggerDelayValue.asFloat();

        const Json::Value smoothingValue = soundObjectJSON[ "smoothing" ];
        if( smoothingValue.type() != Json::nullValue )
            smoothing = smoothingValue.asFloat();

        const Json::Value fadeInRateValue = soundObjectJSON[ "fadeInRate" ];
        if( fadeInRateValue.type() != Json::nullValue )
            fadeInRate = fadeInRateValue.asFloat();
        
        const Json::Value fadeOutRateValue = soundObjectJSON[ "fadeOutRate" ];
        if( fadeOutRateValue.type() != Json::nullValue )
            fadeOutRate = fadeOutRateValue.asFloat();

        const Json::Value fadeCurveValue = soundObjectJSON[ "fadeCurve" ];
        if( fadeCurveValue.type() != Json::nullValue )
            releasePower = fadeCurveValue.asFloat();
        
        const Json::Value sensitivityValue = soundObjectJSON[ "sensitivity" ];
        if( sensitivityValue.type() != Json::nullValue )
            sensitivity = sensitivityValue.asFloat();

        const Json::Value pitchValue = soundObjectJSON[ "pitch" ];
        if( pitchValue.type() != Json::nullValue )
            pitch = pitchValue.asFloat();

        const Json::Value choiceModeValue = soundObjectJSON[ "soundDefChoiceMode" ];
        if( choiceModeValue.type() != Json::nullValue )
            soundDefChoiceMode = choiceModeValue.asInt();

        const Json::Value currentSoundDefIndexValue = soundObjectJSON[ "currentSoundDefIndex" ];
        if( currentSoundDefIndexValue.type() != Json::nullValue )
            currentSoundDefIndex = currentSoundDefIndexValue.asInt();

        const Json::Value mixJSON = soundObjectJSON[ "mix" ];
        mix.LoadFromJSONSettings( mixJSON );
    }
}
