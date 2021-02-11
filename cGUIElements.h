#ifndef CGUIELEMENTS_H
#define CGUIELEMENTS_H


#include "ofxDatGuiButton.h"
#include "ofxDatGuiSlider.h"
#include "ofxDatGuiGroups.h"
#include "ofxDatGuiComponent.h"
#include "ofxDatGuiTextInputField.h"
#include "ofxDatGuiTextInput.h"
#include "ofxDatGuiTheme.h"

class cSoundEditor;

typedef void (*_get_value_function)(cSoundEditor *editor, ofxDatGuiComponent *);

class azureKinectTheme : public ofxDatGuiTheme{
public:
    azureKinectTheme(){
        font.size = 12;
        color.slider.text = ofColor::fromHex(0xFFFFFF);
        layout.iconSize = 14;
        layout.graph.height = 256;
        layout.width = 480;
        init();
    }
};


class azureKinectGlobalTheme : public ofxDatGuiTheme{
public:
    azureKinectGlobalTheme(){
        font.size = 12;
        color.slider.text = ofColor::fromHex(0xFFFFFF);
        layout.iconSize = 14;
        layout.graph.height = 256;
        color.background = ofColor::fromHex(0x505000);
        layout.width = 480;
        layout.labelWidth = 160;
        init();
    }
};

class cToggle : public ofxDatGuiToggle
{
public:
    bool *valuePtr;
    _get_value_function getValueFunction;
    cSoundEditor *owner;

    cToggle(string label, bool val) : ofxDatGuiToggle(label, val){
        valuePtr = nullptr;
        getValueFunction = nullptr;
        owner = nullptr;
    }

    void SetGetValueFunction(cSoundEditor *inOwner, _get_value_function inGetValueFunction){ owner = inOwner; getValueFunction=inGetValueFunction; }

    void SetValuePtr(bool *valuePtrIn)
    {
        valuePtr = valuePtrIn;
    }

    void UpdateValueFromSource(void)
    {
        if(valuePtr)
        {
            setChecked( *valuePtr );
        }
        else if(getValueFunction && owner)
        {
            getValueFunction(owner, this);
        }

    }
};


enum T_ValueType
{
    t_ValueTypeInt16,
    t_ValueTypeInt32,
    t_ValueTypeFloat32
};


class cFocussedSlider : public ofxDatGuiSlider
{
public:
    static bool newClick;
    static bool previousMousePressed;
    static cFocussedSlider *currentSlider;

    T_ValueType valueType;
    void *valuePtr;
    _get_value_function getValueFunction;
    cSoundEditor *owner;
    bool lostFocusOnReturn;

    cFocussedSlider(string label, float min, float max, float val) : ofxDatGuiSlider(label, min, max, val){valuePtr = nullptr; getValueFunction = nullptr; owner = nullptr; lostFocusOnReturn = false; }

    void SetGetValueFunction(cSoundEditor *inOwner, _get_value_function inGetValueFunction){ owner = inOwner; getValueFunction=inGetValueFunction; }
    void SetValuePtr( void *valuePtrIn, T_ValueType valueTypeIn)
    {
        valuePtr = valuePtrIn;
        valueType = valueTypeIn;
    }

    void UpdateValueFromSource(void)
    {
        if(valuePtr)
        {
            switch(valueType)
            {
            case t_ValueTypeInt16:
                setValue( static_cast<float>(*(int16_t *)valuePtr), false);
               break;

            case t_ValueTypeInt32:
                setValue( static_cast<float>(*(int32_t *)valuePtr), false);
               break;

            case t_ValueTypeFloat32:
                setValue( *(float *)valuePtr, false);
                break;
            }
        }
        else if(getValueFunction && owner)
        {
            getValueFunction(owner, this);
        }
    }

    void onMousePress(ofPoint m);

    void ReferredPress(ofPoint m)
    {
        ofxDatGuiSlider::onMousePress(m);
    }

    void ReferredDrag(ofPoint m)
    {
        ofxDatGuiSlider::onMouseDrag(m);
    }

    void ReferredRelease(ofPoint m)
    {
        ofxDatGuiSlider::onMouseRelease(m);
    }


    void onMouseDrag(ofPoint m)
    {
        previousMousePressed = true;
        if(currentSlider)
            currentSlider -> ReferredDrag(m);
    }

    void onMouseRelease(ofPoint m)
    {
        bool mousePressed = ofGetMousePressed();

        if(mousePressed == false )
        {
            previousMousePressed = false;
            if(currentSlider)
                currentSlider -> ReferredRelease(m);
            currentSlider = nullptr;
        }
    }
//    ofCoreEvents coreEvents;

};



class cDropDown : public ofxDatGuiDropdown
{
public:
    int32_t *valuePtr;
    _get_value_function getValueFunction;
    cSoundEditor *owner;

    cDropDown(string label, vector<string> options) : ofxDatGuiDropdown(label, options){
        valuePtr = nullptr;
        owner = nullptr;
        getValueFunction = nullptr;
    }

    void SetGetValueFunction(cSoundEditor *inOwner, _get_value_function inGetValueFunction){ owner = inOwner; getValueFunction=inGetValueFunction; }

    void SetValuePtr(int32_t *valuePtrIn)
    {
        valuePtr = valuePtrIn;
    }

    void UpdateValueFromSource(void)
    {
        if(valuePtr)
        {
            select( *valuePtr );
        }
        else if(getValueFunction && owner)
        {
            getValueFunction(owner, this);
        }
    }
};


class ofxDatGuiNumericInput : public ofxDatGuiTextInput
{
public:
    static const int MAX_PRECISION = 4;
    float *valuePtr;
    bool isInt;
    _get_value_function getValueFunction;
    cSoundEditor *owner;
    bool lostFocusOnReturn;

    ofxDatGuiNumericInput(string label, float inValue = 0.0f ) : ofxDatGuiTextInput(label)
    {
    setValue(inValue);
    mInput.setTextInputFieldType(ofxDatGuiInputType::NUMERIC);
    setPrecision(2);
    valuePtr = nullptr;
    owner = nullptr;
    getValueFunction = nullptr;
    isInt = false;
    lostFocusOnReturn = false;
    }

    void setPrecision(int precision, bool truncateValue = true)
    {
    mPrecision = precision;
    mTruncateValue = truncateValue;
    if (mPrecision > MAX_PRECISION) mPrecision = MAX_PRECISION;
    }

    void SetGetValueFunction(cSoundEditor *inOwner, _get_value_function inGetValueFunction){ owner = inOwner; getValueFunction=inGetValueFunction; }

    void UpdateValueFromSource(void)
    {
        if(valuePtr != nullptr)
            setValue(*valuePtr);
        else if(getValueFunction && owner)
        {
            getValueFunction(owner, this);
        }
    }

    void UpdateSourceFromValue(void)
    {
        if(valuePtr != nullptr)
            *valuePtr = getValue();
    }


    void setValuePtr(float *inPtr){valuePtr = inPtr;}

    void setValue( float value )
    {
    ostringstream convert;   // stream used for the conversion
    convert << value;      // insert the textual representation of 'Number' in the characters in the stream
    mInput.setText(convert.str());
    }

    float getValue()
    {
    float result = 0;
    istringstream convert(mInput.getText());
    if ( !(convert >> result) ) //give the value to 'Result' using the characters in the stream
        result = 0;             //if that fails set 'Result' to 0
    return result;
    }
    
    virtual void onKeyPressed(int key)
    {
        switch(key)
        {
            case OF_KEY_UP:
                setValue( getValue() + .1 );
                fakeUpdate();
                break;
                
            case OF_KEY_DOWN:
                setValue( getValue() - .1 );
                UpdateSourceFromValue();
                fakeUpdate();
                break;
                
            case OF_KEY_RETURN:
                lostFocusOnReturn = true;
                break;
                
            default:
                ofxDatGuiTextInput::onKeyPressed(key);
                break;
        }
    }
    
    virtual void onFocusLost()
    {
        ofxDatGuiTextInput::onFocusLost();
        //       mInput.onFocus();
    }
    
    void fakeUpdate()
    {
        mInput.onFocusLost();
        mInput.onFocus();
        //        mInput.mHighlightText = true;
    }
    
    void regainFocus()
    {
        mInput.onFocus();
        onFocus();
        lostFocusOnReturn = false;
    }

    int     mPrecision;
    bool    mTruncateValue;

};


class ofxDatGuiNumericIntInput : public ofxDatGuiTextInput
{
public:
    static const int MAX_PRECISION = 4;
    int32_t *valuePtr;
    bool isInt;
    _get_value_function getValueFunction;
    cSoundEditor *owner;
    bool lostFocusOnReturn;

    ofxDatGuiNumericIntInput(string label, int32_t inValue = 0 ) : ofxDatGuiTextInput(label)
    {
    setValue(inValue);
    mInput.setTextInputFieldType(ofxDatGuiInputType::NUMERIC);
    setPrecision(0);
    valuePtr = nullptr;
    owner = nullptr;
    getValueFunction = nullptr;
    isInt = true;
    lostFocusOnReturn = false;
    }

    void setPrecision(int precision, bool truncateValue = true)
    {
    mPrecision = precision;
    mTruncateValue = truncateValue;
    if (mPrecision > MAX_PRECISION) mPrecision = MAX_PRECISION;
    }

    void SetGetValueFunction(cSoundEditor *inOwner, _get_value_function inGetValueFunction){ owner = inOwner; getValueFunction=inGetValueFunction; }

    void UpdateValueFromSource(void)
    {
        if(valuePtr != nullptr)
            setValue(*valuePtr);
        else if(getValueFunction && owner)
        {
            getValueFunction(owner, this);
        }
    }

    void UpdateSourceFromValue(void)
    {
        if(valuePtr != nullptr)
            *valuePtr = getValue();
    }


    void setValuePtr(int32_t *inPtr){valuePtr = inPtr;}

    void setValue( int32_t value )
    {
        ostringstream convert;   // stream used for the conversion
        convert << value;      // insert the textual representation of 'Number' in the characters in the stream
        mInput.setText(convert.str());
    }

    void setValue( float value )
    {
        ostringstream convert;   // stream used for the conversion
        convert << value;      // insert the textual representation of 'Number' in the characters in the stream
        mInput.setText(convert.str());
    }
    
    int32_t getValue()
    {
        int32_t result = 0;
        istringstream convert(mInput.getText());
        if ( !(convert >> result) ) //give the value to 'Result' using the characters in the stream
            result = 0;             //if that fails set 'Result' to 0
        return result;
    }

    virtual void onKeyPressed(int key)
    {
        switch(key)
        {
            case OF_KEY_UP:
                setValue( getValue() + 1 );
                UpdateSourceFromValue();
                fakeUpdate();
                break;
                
            case OF_KEY_DOWN:
                setValue( getValue() - 1 );
                UpdateSourceFromValue();
                fakeUpdate();
                break;
                
            case OF_KEY_RETURN:
                lostFocusOnReturn = true;
                UpdateSourceFromValue();
                break;
                
            default:
                ofxDatGuiTextInput::onKeyPressed(key);
                break;
        }
    }
    
    virtual void onFocusLost()
    {
        ofxDatGuiTextInput::onFocusLost();
 //       mInput.onFocus();
    }
    
    void fakeUpdate()
    {
        mInput.onFocusLost();
        mInput.onFocus();
//        mInput.mHighlightText = true;
    }

    void regainFocus()
    {
        mInput.onFocus();
        onFocus();
        lostFocusOnReturn = false;
    }

    int     mPrecision;
    bool    mTruncateValue;

};


#endif // CGUIELEMENTS_H
