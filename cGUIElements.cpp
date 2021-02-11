#include "cGUIElements.h"
#include "cSoundObject.hpp"
#include "cSoundEditor.hpp"
#include "cSampleSynth.hpp"

cFocussedSlider *cFocussedSlider::currentSlider = nullptr;
bool cFocussedSlider::newClick = false;
bool cFocussedSlider::previousMousePressed = false;


void cFocussedSlider::onMousePress(ofPoint m)
{
    if(owner && owner -> menuExpanded)
        return;
    bool mousePressed = ofGetMousePressed();
    
    if( mousePressed != previousMousePressed )
    {
        if(mousePressed )
            newClick = true;
    }
    else
        newClick = false;
    previousMousePressed = mousePressed;
    
    if(newClick)
    {
        currentSlider = this;
        currentSlider -> ReferredPress(m);
    }
    else if(currentSlider)
        currentSlider -> ReferredDrag(m);
}


void cSoundEditor::onSliderChangedEvent(ofxDatGuiSliderEvent e)
{
    float value = e.target -> getValue();
    cFocussedSlider *slider = (cFocussedSlider *)e.target;
    if(slider -> valuePtr)
    {
        switch( slider -> valueType )
        {
            case t_ValueTypeInt16:
                *(int16_t *)slider->valuePtr = static_cast<int16_t>(value);
                break;

            case t_ValueTypeInt32:
                *(int32_t *)slider->valuePtr = static_cast<int32_t>(value);
                break;

            case t_ValueTypeFloat32:
                *(float *)slider->valuePtr = value;
                break;
        }
    }
}


int32_t cSoundEditor::CreateLabel(string label, int width, int32_t x_off, int32_t offset)
{
    ofxDatGuiLabel *guiLabel = new ofxDatGuiLabel("");
    guiLabel -> setTheme(&guiTheme);
    guiLabel -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    guiLabel -> setWidth(width, sliderLabelWidth);
    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(guiLabel));
    offset ++;
    return offset;
}


int32_t cSoundEditor::CreatePlotter(string label, int32_t min, int32_t max, int width, int32_t x_off, int32_t offset)
{
    ofxDatGuiValuePlotter *plotter = new ofxDatGuiValuePlotter( label, (float)min, (float)max);
    ((ofxDatGuiComponent *)plotter) -> setTheme(&guiTheme);
    plotter -> setSpeed(1.0f);
    plotter -> setDrawMode(ofxDatGuiGraph::LINES);
    ((ofxDatGuiComponent *)plotter) -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    ((ofxDatGuiComponent *)plotter) -> setWidth(width, sliderLabelWidth);
    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(plotter));
    offset ++;
    return offset;
}


int32_t cSoundEditor::CreateDropDown(string label, vector<string> options, int32_t *intVariablePtr, int width, int32_t x_off, int32_t offset )
{
    cDropDown *dropDown = new cDropDown(label, options);
    dropDown -> setTheme(&guiTheme);
    dropDown -> SetValuePtr(intVariablePtr);
    dropDown->select(*intVariablePtr);
    dropDown->setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    dropDown -> setWidth( width );
    dropDown -> onDropdownEvent(this, &cSoundEditor::onDropdownEvent);

    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(dropDown));
    offset ++;
    return offset;
}


int32_t cSoundEditor::CreateDropDown(string label, vector<string> options, _get_value_function getValueFunction, int width, int32_t x_off, int32_t offset )
{
    cDropDown *dropDown = new cDropDown(label, options);
    dropDown -> setTheme(&guiTheme);
    dropDown -> SetGetValueFunction(this, getValueFunction);
    dropDown->setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    dropDown -> setWidth( width );
    dropDown -> onDropdownEvent(this, &cSoundEditor::onDropdownEvent);

    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(dropDown));
    offset ++;
    return offset;
}


int32_t cSoundEditor::CreateButton(string label, int width, int32_t x_off, int32_t offset )
{
    ofxDatGuiButton *button = new ofxDatGuiButton(label);
    button -> setTheme(&guiTheme);
    button -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    button -> setWidth( width, sliderLabelWidth );
    button -> onButtonEvent( this, &cSoundEditor::onButtonClickedEvent );
    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(button));
    offset ++;
    return offset;
}


int32_t cSoundEditor::CreateToggle(string label, bool *boolVariablePtr, int width, int32_t x_off, int32_t offset )
{
    cToggle *toggle = new cToggle(label, *boolVariablePtr );
    toggle -> setTheme(&guiTheme);
    toggle -> SetValuePtr(boolVariablePtr);
    toggle -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    toggle -> setWidth( width, sliderLabelWidth );
    toggle -> onToggleEvent( this, &cSoundEditor::onToggleChangedEvent );
    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(toggle));
    offset ++;
    return offset;
}


int32_t cSoundEditor::CreateToggle(string label, _get_value_function getValueFunction, int width, int32_t x_off, int32_t offset )
{
    cToggle *toggle = new cToggle(label, false );
    toggle -> setTheme(&guiTheme);

    toggle -> SetGetValueFunction(this, getValueFunction);
    toggle -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    toggle -> setWidth( width, sliderLabelWidth );
    toggle -> onToggleEvent( this, &cSoundEditor::onToggleChangedEvent );
    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(toggle));
    offset ++;
    return offset;
}



int32_t cSoundEditor::CreateNumericInput( string label, float *ptr, int width, int32_t x_off, int offset )
{
    ofxDatGuiNumericInput *numberBox = new ofxDatGuiNumericInput( label, *ptr );
    int labelWidth = sliderLabelWidth;
    numberBox -> setTheme(&guiTheme);
    numberBox -> setValuePtr(ptr);
    numberBox -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    numberBox -> setWidth( width, labelWidth );
    numberBox -> setPrecision( 3 );
    numberBox -> onTextInputEvent( this, &cSoundEditor::onTextInputChangedEvent );

    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(numberBox));
    offset ++;
    return offset;
}

int32_t cSoundEditor::CreateNumericInput( string label, _get_value_function getValueFunction, int width, int32_t x_off, int offset )
{
    ofxDatGuiNumericIntInput *numberBox = new ofxDatGuiNumericIntInput( label, 0 );
    int labelWidth = sliderLabelWidth;
    numberBox -> setTheme(&guiTheme);
    numberBox -> SetGetValueFunction(this, getValueFunction);
    numberBox -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    numberBox -> setWidth( width, labelWidth );
    numberBox -> setPrecision( 3 );
    numberBox -> onTextInputEvent( this, &cSoundEditor::onTextInputChangedEvent );

    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(numberBox));
    offset ++;
    return offset;
}

int32_t cSoundEditor::CreateNumericInputInt( string label, int32_t *ptr, int width, int32_t x_off, int offset )
{
    ofxDatGuiNumericIntInput *numberBox = new ofxDatGuiNumericIntInput( label, *ptr );
    int labelWidth = sliderLabelWidth;
    numberBox -> setTheme(&guiTheme);
    numberBox -> setValuePtr(ptr);
    numberBox -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    numberBox -> setWidth( width, labelWidth );
    numberBox -> setPrecision( 0 );
    numberBox -> onTextInputEvent( this, &cSoundEditor::onTextInputChangedEvent );

    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(numberBox));
    offset ++;
    return offset;
}


int32_t cSoundEditor::CreateNumericInputInt( string label, _get_value_function getValueFunction, int width, int32_t x_off, int offset )
{
    ofxDatGuiNumericIntInput *numberBox = new ofxDatGuiNumericIntInput( label, 0 );
    int labelWidth = sliderLabelWidth;
    numberBox -> setTheme(&guiTheme);
    numberBox -> SetGetValueFunction(this, getValueFunction);
    numberBox -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    numberBox -> setWidth( width, labelWidth );
    numberBox -> setPrecision( 0 );
    numberBox -> onTextInputEvent( this, &cSoundEditor::onTextInputChangedEvent );

    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(numberBox));
    offset ++;
    return offset;
}


int32_t cSoundEditor::CreateInt32Slider( string label, int32_t min, int32_t max, int32_t *ptr, int width, int32_t x_off, int offset )
{
    cFocussedSlider *slider = new cFocussedSlider( label, min, max, *ptr );
    int labelWidth = sliderLabelWidth;
    slider -> setTheme(&guiTheme);
    slider -> SetValuePtr(ptr, t_ValueTypeInt32);
    slider -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    if(labelWidth > width / 2.5)
        labelWidth = width / 2.5;
    slider -> setWidth( width, labelWidth );
    slider -> setPrecision( 0 );
    slider -> onSliderEvent( this, &cSoundEditor::onSliderChangedEvent );

    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(slider));
    offset ++;
    return offset;
}

int32_t cSoundEditor::CreateInt32Slider( string label, int32_t min, int32_t max, _get_value_function getValueFunction, int width, int32_t x_off, int offset)
{
    cFocussedSlider *slider = new cFocussedSlider( label, min, max, min );
    int labelWidth = sliderLabelWidth;
    slider -> setTheme(&guiTheme);
    slider -> SetGetValueFunction(this, getValueFunction);
    slider -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    if(labelWidth > width / 2.5)
        labelWidth = width / 2.5;
    slider -> setWidth( width, labelWidth );
    slider -> setPrecision( 0 );
    slider -> onSliderEvent( this, &cSoundEditor::onSliderChangedEvent );
    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(slider));
    offset ++;
    return offset;
}


// we need a version that can have a getter function
int32_t cSoundEditor::CreateFloatSlider( string label, float min, float max, float *ptr, int width, int32_t x_off, int offset )
{
    cFocussedSlider *slider = new cFocussedSlider( label, min, max, *ptr );
    int labelWidth = sliderLabelWidth;
    slider -> setTheme(&guiTheme);
    slider -> SetValuePtr(ptr, t_ValueTypeFloat32);
    slider -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    if(labelWidth > width / 2.5)
        labelWidth = width / 2.5;
    slider -> setWidth( width, labelWidth );
    slider -> setPrecision( 3 );
    slider -> onSliderEvent( this, &cSoundEditor::onSliderChangedEvent );
    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(slider));
    offset ++;
    return offset;
}


int32_t cSoundEditor::CreateFloatSlider( string label, float min, float max, _get_value_function getValueFunction, int width, int32_t x_off, int offset)
{
    cFocussedSlider *slider = new cFocussedSlider( label, min, max, min );
    int labelWidth = sliderLabelWidth;
    slider -> setTheme(&guiTheme);
//    slider -> SetValuePtr(ptr, t_ValueTypeFloat32);
    slider -> SetGetValueFunction(this, getValueFunction);
    slider -> setPosition(currentGUIXStart + x_off, offset * guiElementHeight + currentGUIYStart);
    if(labelWidth > width / 2.5)
        labelWidth = width / 2.5;
    slider -> setWidth( width, labelWidth );
    slider -> setPrecision( 3 );
    slider -> onSliderEvent( this, &cSoundEditor::onSliderChangedEvent );
    guiComponents_.push_back(static_cast<ofxDatGuiComponent *>(slider));
    offset ++;
    return offset;
}

/*void cSoundEditor::ExtractValuesForDisplay( void )
{
    for(ofxDatGuiComponent *comp : guiComponents)
    {
        switch(comp->getType())
        {
        case ofxDatGuiType::TOGGLE:
            ((cToggle *)comp) -> UpdateValueFromSource();
            break;

        case ofxDatGuiType::SLIDER:
            ((cFocussedSlider *)comp) -> UpdateValueFromSource();
            break;

        case ofxDatGuiType::TEXT_INPUT:
            ((ofxDatGuiNumericInput *)comp) -> UpdateValueFromSource();
            break;
        }
    }
}*/


void cSoundEditor::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    int modeChoice = e.child;
    cDropDown *dropDown = (cDropDown *)e.target;
    if(dropDown -> valuePtr)
    {
         *dropDown -> valuePtr = modeChoice;
    }
}


void cSoundEditor::onToggleChangedEvent(ofxDatGuiToggleEvent e)
{
    float value = e.target -> getChecked();
    cToggle *toggle = (cToggle *)e.target;
    if(toggle -> valuePtr)
    {
         *toggle -> valuePtr = value;
    }
}


void cSoundEditor::onButtonClickedEvent(ofxDatGuiButtonEvent e)
{
    string label = e.target -> getLabel();
    ofxDatGuiButton *button = (ofxDatGuiButton *)e.target;
}


void cSoundEditor::onTextInputChangedEvent(ofxDatGuiTextInputEvent e)
{
    ofxDatGuiNumericInput *numberBox = (ofxDatGuiNumericInput *)e.target;
    numberBox -> UpdateSourceFromValue();
}


void cSoundEditor::ShowGUIElements( void )
{
    ofSetColor(255, 255, 255);
    ofFill();
    int32_t index = 0;
    bool drawQuadMenu = false;
    for(ofxDatGuiComponent *comp : guiComponents_)
    {
        string label = comp->getLabel();
        bool draw = true;

        if(index > soundDefGUIComponentsStartIndex)
        {
            if( SoundDef() )
            {
                if(SoundDef() -> loop == false)
                {
                    if( index > loopIndex )
                    {
                        draw = false;
                    }
                }
            }
            else
                draw = false;
        }

        if( soundObject )
            {
            switch( soundObject -> mix.mode )
                {
                case t_MixSimpleStereo:
                    if(index >= leftFrontChannelIndex && index <= rightBackChannelIndex )
                        draw = false;
                    else if(index==frontBackIndex)
                        draw = false;
                    else if(index==leftChannelIndex)
                        draw=false;
                    else if(index==rightChannelIndex)
                        draw=false;
                    else if(index==quadChannelsMenuIndex)
                        draw = false;
                    break;

                case t_MixStereoToMulti:
                    if(index >= leftFrontChannelIndex && index <= rightBackChannelIndex )
                        draw = false;
                    else if(index==frontBackIndex)
                        draw = false;
                    else if(index==quadChannelsMenuIndex)
                        draw = false;
                    break; //channel count

                case t_MixStereoToQuadInMulti:
                    if(index==leftChannelIndex)
                        draw=false;
                    else if(index==rightChannelIndex)
                        draw=false;
                    drawQuadMenu = true;
                    break;
                }
            }

        index ++;

        switch(comp->getType())
        {
        case ofxDatGuiType::BUTTON:
            if( draw )
                ((ofxDatGuiButton *)comp) -> draw();
            break;

        case ofxDatGuiType::DROPDOWN:
            if( draw )
                ((cDropDown *)comp) -> draw();
            break;

        case ofxDatGuiType::TOGGLE:
            if( draw )
                ((cToggle *)comp) -> draw();
            break;

        case ofxDatGuiType::TEXT_INPUT:
            if( draw )
                ((ofxDatGuiNumericInput *)comp) -> draw();
            break;

        case ofxDatGuiType::SLIDER:
            if( draw )
                ((cFocussedSlider *)comp) -> draw();
            break;

        case ofxDatGuiType::VALUE_PLOTTER:
            if( draw )
                comp -> draw();
            break;

        case ofxDatGuiType::LABEL:
            if( draw )
                comp -> draw();
            break;
        }
    }
    if(owner -> receivedOSC)
    {
        ofSetColor(192, 96, 0);
        owner -> receivedOSC = false;
    }
    else
        ofSetColor(32, 32, 0);
    ofDrawRectangle(rxIndicatorRect);

    ofSetColor(255, 255, 255);
    index = 0;
    for(ofxDatGuiComponent *comp : guiComponents_)
    {
        switch(comp->getType())
        {
            case ofxDatGuiType::DROPDOWN:
                if( index == quadChannelsMenuIndex && drawQuadMenu == false )
                    break;
                ((cDropDown *)comp) -> draw();
                break;
        }
        index ++;
    }

}



void cSoundEditor::UpdateGUIElements( void )
{
    mouseHandled = false;
    menuExpanded = false;
    for(int i = 0; i < guiComponents_.size(); i ++ )
    {
        ofxDatGuiComponent *comp =guiComponents_[ i ];
        switch(comp->getType())
        {
        case ofxDatGuiType::BUTTON:
            ((ofxDatGuiButton *)comp) -> update();
            break;

        case ofxDatGuiType::TOGGLE:
            ((cToggle *)comp) -> update();
            break;

        case ofxDatGuiType::DROPDOWN:
            {
            cDropDown *d = (cDropDown *)comp;
            d -> update();
            if( d -> getIsExpanded())
                menuExpanded = true;
            }
            break;

        case ofxDatGuiType::SLIDER:
            ((cFocussedSlider *)comp) -> update();
            break;

        case ofxDatGuiType::VALUE_PLOTTER:
            ((ofxDatGuiValuePlotter *)comp) -> update(true);
            break;

        case ofxDatGuiType::TEXT_INPUT:
            if(((ofxDatGuiNumericInput *)comp) -> isInt)
            {
                if( ((ofxDatGuiNumericIntInput *)comp) -> lostFocusOnReturn )
                {
                    ((ofxDatGuiNumericIntInput *)comp) -> regainFocus();
                }
                ((ofxDatGuiNumericIntInput *)comp) -> update();
            }
            else
            {
                if( ((ofxDatGuiNumericInput *)comp) -> lostFocusOnReturn )
                {
                    ((ofxDatGuiNumericInput *)comp) -> regainFocus();
                }
                ((ofxDatGuiNumericInput *)comp) -> update();
            }
            break;

        case ofxDatGuiType::LABEL:
            if(SoundDef())
                ((ofxDatGuiLabel*)comp) ->setLabel(SoundDef()->sampleName);
            ((ofxDatGuiLabel*)comp) -> update();
            break;

        }
    }
}
