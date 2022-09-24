#pragma once

#include <JuceHeader.h>

#include "FrequencyEditor.h"

#include "Components/juce_serialport/juce_serialport.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component, public juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void changeListenerCallback( juce::ChangeBroadcaster* source) override;
        
private:
    //==============================================================================
    FrequencyEditor bp;
    
    juce::Label  appNameText;
    juce::TabbedButtonBar tabbar { juce::TabbedButtonBar::TabsAtTop };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
