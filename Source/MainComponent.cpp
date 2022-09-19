#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent()
{
    getLookAndFeel().setColour (juce::Slider::thumbColourId, juce::Colours::red);
    getLookAndFeel().setColour (juce::ResizableWindow::backgroundColourId, juce::Colour(0x22U, 0x20U, 0x2AU));
    
    addAndMakeVisible(bp);
    
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    
}

void MainComponent::resized()
{
    bp.setBounds (0, 50, getWidth(), getHeight() - 50);
    bp.resized();
    
    
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    //freqProcessor.setSavedSize ({ getWidth(), getHeight() });

    //socialButtons.setBounds (plotFrame.removeFromBottom (35));
}
