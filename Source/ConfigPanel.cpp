/*
  ==============================================================================

    ConfigPanel.cpp
    Created: 24 Sep 2022 10:09:50am
    Author:  Robin Prillwitz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ConfigPanel.h"

//==============================================================================
ConfigPanel::ConfigPanel()
{
    addAndMakeVisible(connectionBox);
    addAndMakeVisible(updateButton);

    connectionLabel.attachToComponent(&connectionBox, false);
    addAndMakeVisible(connectionLabel);
    
    updateLabel.attachToComponent(&updateButton, false);
    addAndMakeVisible(updateLabel);
}

ConfigPanel::~ConfigPanel()
{
}

void ConfigPanel::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("ConfigPanel", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void ConfigPanel::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(10,50);
    auto columnWidth = bounds.getWidth() / 4;
    auto column = bounds.removeFromLeft(columnWidth);
    
    connectionBox.setBounds(column.removeFromTop(30));
    column.removeFromTop(30);
    updateButton.setBounds(column.removeFromTop(30));
    
}

void ConfigPanel::buttonClicked (juce::Button* b)
{
    
}
