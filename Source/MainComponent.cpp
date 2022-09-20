#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent()
{
    getLookAndFeel().setColour (juce::ResizableWindow::backgroundColourId, juce::Colour(0x0FU, 0x0FU, 0x14U));
    getLookAndFeel().setColour (juce::Slider::trackColourId, juce::Colour(0x36U, 0x36U, 0x3FU));
    getLookAndFeel().setColour (juce::PopupMenu::backgroundColourId, juce::Colour(0x36U, 0x36U, 0x3FU));
    getLookAndFeel().setColour (juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
        
    appNameText.setText("dsp-ctrl", juce::dontSendNotification);
    appNameText.setFont (juce::Font (24.0f, juce::Font::bold));
    addAndMakeVisible(appNameText);
    
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
    auto bounds = getLocalBounds();
    
    appNameText.setBounds(bounds.removeFromTop(50));
    bp.setBounds(bounds);
    
    //bp.setBounds (0, 50, getWidth(), getHeight() - 50);
    bp.resized();
    
    
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    //freqProcessor.setSavedSize ({ getWidth(), getHeight() });

    //socialButtons.setBounds (plotFrame.removeFromBottom (35));
}
