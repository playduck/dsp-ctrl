/*
  ==============================================================================

    ConfigPanel.h
    Created: 24 Sep 2022 10:09:50am
    Author:  Robin Prillwitz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ConfigPanel  : public juce::Component, public juce::Button::Listener
{
public:
    ConfigPanel();
    ~ConfigPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked (juce::Button* b) override;

private:
    
    juce::Label         connectionLabel { TRANS ("DSP Connection Port"), TRANS ("CONNECTION") };
    juce::ComboBox      connectionBox;
    
    juce::Label         updateLabel { TRANS ("Auto-Update"), TRANS ("AUTO") };
    juce::ToggleButton  updateButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigPanel)
};
