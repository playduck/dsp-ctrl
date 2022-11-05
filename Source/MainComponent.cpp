#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent()
{
    getLookAndFeel().setColour (juce::ResizableWindow::backgroundColourId, juce::Colour(0x18U, 0x18U, 0x18U));
    getLookAndFeel().setColour (juce::Slider::trackColourId, juce::Colour(0x36U, 0x36U, 0x3FU));
    getLookAndFeel().setColour (juce::PopupMenu::backgroundColourId, juce::Colour(0x36U, 0x36U, 0x3FU));
    getLookAndFeel().setColour (juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
        
    appNameText.setText("dsp-ctrl", juce::dontSendNotification);
    appNameText.setFont (juce::Font (24.0f, juce::Font::bold));
    addAndMakeVisible(appNameText);
    
    tabbar.addTab("CONFIG", getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId), 0);
    tabbar.addTab("EQ", getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId), 1);
    tabbar.setCurrentTabIndex(0);
    tabbar.addChangeListener(this);
    
    for(int i = 0; i < tabbar.getNumTabs(); ++i)    {
        juce::TabBarButton* tab = tabbar.getTabButton(i);
        
    }
    
    addAndMakeVisible(tabbar);
    
    addAndMakeVisible(config);
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
    auto topbar = bounds.removeFromTop(50);
    
    appNameText.setBounds(topbar.removeFromLeft(100));
    tabbar.setBounds(topbar);
    
    config.setBounds(bounds);
    config.resized();
    
    bp.setBounds(bounds);
    bp.resized();
    
    switch( tabbar.getCurrentTabIndex() )  {
        case 0:
            printf("CONFIG\n");
            config.setVisible(true);
            bp.setVisible(false);
            break;
        case 1:
            printf("EQ\n");
            config.setVisible(false);
            bp.setVisible(true);
            break;
        default:
            printf("%d\n", tabbar.getCurrentTabIndex());
            config.setVisible(false);
            bp.setVisible(false);
            break;
    }
    
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    //freqProcessor.setSavedSize ({ getWidth(), getHeight() });

    //socialButtons.setBounds (plotFrame.removeFromBottom (35));
}

void MainComponent::changeListenerCallback( juce::ChangeBroadcaster* source)   {
    resized();
}
