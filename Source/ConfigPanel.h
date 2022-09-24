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
class ConfigPanel  : public juce::Component
{
public:
    ConfigPanel();
    ~ConfigPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigPanel)
};
