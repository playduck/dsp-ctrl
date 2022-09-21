//
//  bandEditor.cpp
//  dsp-ctrl - App
//
//  Created by Robin Prillwitz on 19.09.22.
//

#include <JuceHeader.h>
#include "FrequencyEditor.h"

//==============================================================================
FrequencyEditor::BandEditor::BandEditor (size_t i, float startF, juce::Colour c)
  : index (i)
{
    colour = c;
    
    frequencyResponse.preallocateSpace(MAG_LEN);
 
    frame.setColour(juce::GroupComponent::outlineColourId, colour);
    frame.setText (juce::String(index) + "F");
    frame.setTextLabelPosition (juce::Justification::centred);
    addAndMakeVisible (frame);

    addAndMakeVisible (filterType);
    filterType.addItem(TRANS ("LOWPASS"),  lowpass);
    filterType.addItem(TRANS ("HIGHPASS"),  highpass);
    filterType.addItem(TRANS ("PEQ"),  peq);
    
    filterType.setSelectedId(peq);
    type = peq;
    
    addAndMakeVisible (frequency);
    frequency.setTooltip (TRANS ("Filter's frequency"));
    frequency.setTextValueSuffix (" Hz");
    frequency.setRange(20, 20000, 1);
    frequency.setSkewFactorFromMidPoint(2000);
    frequency.setValue(startF);

    addAndMakeVisible (quality);
    quality.setTooltip (TRANS ("Filter's steepness (Quality)"));
    quality.setRange(0.1, 20, 0.001);
    quality.setSkewFactorFromMidPoint(6);
    quality.setValue(2.0);

    addAndMakeVisible (gain);
    gain.setTooltip (TRANS ("Filter's gain"));
    gain.setTextValueSuffix (" dB");
    gain.setRange(-maxDB, maxDB, 0.5);
    gain.setSkewFactorFromMidPoint(0);
    gain.setValue(3.0);

    solo.setClickingTogglesState (true);
    solo.addListener (this);
    solo.setColour (juce::TextButton::buttonOnColourId, juce::Colours::yellow);
    addAndMakeVisible (solo);
    solo.setTooltip (TRANS ("Listen only through this filter (solo)"));

    activate.setClickingTogglesState (true);
    activate.setColour (juce::TextButton::buttonOnColourId, juce::Colours::green);
    addAndMakeVisible (activate);
    activate.setTooltip (TRANS ("Activate or deactivate this filter"));
    
    channel.setClickingTogglesState (true);
    channel.addListener (this);
    channel.setColour (juce::TextButton::buttonColourId, juce::Colours::blue);
    channel.setColour (juce::TextButton::buttonOnColourId, juce::Colours::red);
    addAndMakeVisible (channel);
    channel.setTooltip (TRANS ("Use band on left or right channel"));
    
    typeLabel.attachToComponent(&filterType, true);
    freqLabel.attachToComponent(&frequency, true);
    gainLabel.attachToComponent(&gain, true);
    qualityLabel.attachToComponent(&quality, true);
    
    addAndMakeVisible(typeLabel);
    addAndMakeVisible(freqLabel);
    addAndMakeVisible(gainLabel);
    addAndMakeVisible(qualityLabel);
    
}

void FrequencyEditor::BandEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // frame
    frame.setBounds(bounds);
    bounds.reduce(10, 10);
    bounds.removeFromTop(10);
    
    auto buttons = bounds.removeFromTop(20).withHeight(20);
    activate.setBounds( buttons.removeFromLeft (25).withWidth(20));
    solo.setBounds(     buttons.removeFromLeft (25).withWidth(20));
    channel.setBounds(  buttons.removeFromLeft (25).withWidth(20));

    auto controls = bounds.removeFromRight(bounds.getWidth() / 1.5);
    controls.removeFromTop(5);
    auto knobHeight = controls.getHeight() / 4;
    
    filterType.setBounds (  controls.removeFromTop(knobHeight));
    frequency.setBounds(    controls.removeFromTop(knobHeight));
    gain.setBounds(         controls.removeFromTop(knobHeight));
    quality.setBounds(      controls.removeFromTop(knobHeight));
}

void FrequencyEditor::BandEditor::paint(juce::Graphics& g) {
    calc();
    updateControls();
    event();
}

void FrequencyEditor::BandEditor::updateControls ()
{
    switch (type) {
        case lowpass:   /* fall through */
        case highpass:
            frequency.setEnabled(true);
            gain.setEnabled(false);
            quality.setEnabled(true);
            break;
        case peq:
            frequency.setEnabled(true);
            gain.setEnabled(true);
            quality.setEnabled(true);
            break;
        default:
            frequency.setEnabled(true);
            gain.setEnabled(true);
            quality.setEnabled(true);
            break;
    }
}

void FrequencyEditor::BandEditor::updateSoloState (bool isSolo)
{
    solo.setToggleState (isSolo, juce::dontSendNotification);
}

void FrequencyEditor::BandEditor::setFrequency (float freq)
{
    frequency.setValue (freq, juce::sendNotification);
}

void FrequencyEditor::BandEditor::setGain (float gainToUse)
{
    gain.setValue (gainToUse, juce::sendNotification);
}

void FrequencyEditor::BandEditor::setType (int newType)
{
    filterType.setSelectedId (newType, juce::sendNotification);
}

void FrequencyEditor::BandEditor::buttonClicked (juce::Button* b)
{
    if (b == &channel) {
        if(channel.getToggleState())    {
            channel.setButtonText("R");
        }   else    {
            channel.setButtonText("L");
        }
    }
}

float FrequencyEditor::BandEditor::getFrequncy()
{
    return (float)frequency.getValueObject().getValue();
}

float FrequencyEditor::BandEditor::getGain()
{
    return (float)gain.getValueObject().getValue();
}

bool FrequencyEditor::BandEditor::getChannel()  {
    return channel.getToggleState();
}

void FrequencyEditor::BandEditor::calc()   {
    type = (filter_type_t)filterType.getSelectedId();
    
    double omega0 = M_PI * ((float)frequency.getValue() / 44100);
    double alpha = (sin(omega0))/(2.0 * (float)quality.getValue());
    double A = pow(10.0, (float)gain.getValue() / 40.0);
    
    double a0 = 0, a1 = 0, a2 = 0;
    double b0 = 0, b1 = 0, b2 = 0;
    
    switch (type) {
        case lowpass:
            a0 = 1.0 + alpha;
            a1 = -2.0 * cos(omega0);
            a2 = 1.0 - alpha;
            
            b0 = (1.0 - cos(omega0)) / 2.0;
            b1 = (1.0 - cos(omega0));
            b2 = (1.0 - cos(omega0)) / 2.0;
            break;
        case highpass:
            a0 = 1.0 + alpha;
            a1 = -2.0 * cos(omega0);
            a2 = 1.0 - alpha;
            
            b0 = (1.0 + cos(omega0)) / 2.0;
            b1 = -(1.0 + cos(omega0));
            b2 = (1.0 + cos(omega0)) / 2.0;
            break;
        case peq:
            a0 = 1.0 + alpha / A;
            a1 = -2.0 * cos(omega0);
            a2 = 1.0 - alpha / A;
            
            b0 = 1.0 + alpha * A;
            b1 = -2.0 * cos(omega0);
            b2 = 1.0 - alpha * A;
            break;
            
        default:
            break;
    }
    
    double omega;
    for(int i = 0; i < MAG_LEN; ++i) {
        
        omega = (20.0f * std::pow (10.0f, 3.0f * ((i+1) / (float)MAG_LEN))) / 20000.0;
    
        double m = \
        10.0 * log10(pow(((b0 + b1 + b2)/2.0), 2) - ((2.0*pow(sin(omega/2.0),2)) * (4.0 * b0 * b2 * (1.0 - (2.0*pow(sin(omega/2.0),2))) + b1 * (b0 + b2)))) -\
        10.0 * log10(pow(((a0 + a1 + a2)/2.0), 2) - ((2.0*pow(sin(omega/2.0),2)) * (4.0 * a0 * a2 * (1.0 - (2.0*pow(sin(omega/2.0),2))) + a1 * (a0 + a2))));
        
        m = m * (-maxDB / 2.0);
        
        if(isinf(m) || isnan(m))    {
            m = 0.0;
        }
                
        magnitudes[i] = m;
    }
    
    frequencyResponse.clear();
    frequencyResponse.startNewSubPath(0, magnitudes[0]);
    for(int i = 0; i < MAG_LEN; ++i) {
        frequencyResponse.lineTo(((i + 1) / (float)MAG_LEN), magnitudes[i]);
    }
}
