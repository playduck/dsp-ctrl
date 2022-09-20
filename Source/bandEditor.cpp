//
//  bandEditor.cpp
//  dsp-ctrl - App
//
//  Created by Robin Prillwitz on 19.09.22.
//

#include <JuceHeader.h>
#include "BodePlot.h"

//==============================================================================
BodePlot::BandEditor::BandEditor (size_t i, float startF, juce::Colour c)
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
    
    typeLabel.attachToComponent(&filterType, true);
    freqLabel.attachToComponent(&frequency, true);
    gainLabel.attachToComponent(&gain, true);
    qualityLabel.attachToComponent(&quality, true);
    
    addAndMakeVisible(typeLabel);
    addAndMakeVisible(freqLabel);
    addAndMakeVisible(gainLabel);
    addAndMakeVisible(qualityLabel);
    
}

void BodePlot::BandEditor::resized ()
{
    auto bounds = getLocalBounds();
    
    // frame
    frame.setBounds (bounds);
    bounds.reduce (10, 20);

    // type
    
    auto buttons = bounds.removeFromTop (20).withHeight (20);
    solo.setBounds (buttons.removeFromLeft (20));
    activate.setBounds (buttons.removeFromRight (20));

//    auto freqBounds = bounds.removeFromBottom (bounds.getHeight() * 1 / 2);
    auto right = bounds.removeFromRight(bounds.getWidth() / 1.5);

    auto knobHeight = bounds.getHeight() / 4;
    
    filterType.setBounds (right.removeFromTop (knobHeight));
    frequency.setBounds(right.removeFromTop (knobHeight));
    gain.setBounds(right.removeFromTop (knobHeight));
    quality.setBounds(right.removeFromTop (knobHeight));
    
//    freqLabel.setBounds(left.removeFromTop (knobHeight));
//    gainLabel.setBounds(left.removeFromTop (knobHeight));
//    qualityLabel.setBounds(left.removeFromTop (knobHeight));
    
}

void BodePlot::BandEditor::paint(juce::Graphics& g) {
    calc();
    updateControls();
    event();
}

void BodePlot::BandEditor::updateControls ()
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

void BodePlot::BandEditor::updateSoloState (bool isSolo)
{
    solo.setToggleState (isSolo, juce::dontSendNotification);
}

void BodePlot::BandEditor::setFrequency (float freq)
{
    frequency.setValue (freq, juce::sendNotification);
}

void BodePlot::BandEditor::setGain (float gainToUse)
{
    gain.setValue (gainToUse, juce::sendNotification);
}

void BodePlot::BandEditor::setType (int newType)
{
    filterType.setSelectedId (newType, juce::sendNotification);
}

void BodePlot::BandEditor::buttonClicked (juce::Button* b)
{
    if (b == &solo) {
//        processor.setBandSolo (solo.getToggleState() ? int (index) : -1);
    }
}

float BodePlot::BandEditor::getFrequncy()
{
    return (float)frequency.getValueObject().getValue();
}

float BodePlot::BandEditor::getGain()
{
    return (float)gain.getValueObject().getValue();
}

void BodePlot::BandEditor::calc()   {
    type = (filter_type_t)filterType.getSelectedId();
    
    frequencyResponse.clear();
    frequencyResponse.startNewSubPath(0, 0);
    
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
    
    double omega, pOmega;
        
    for(int i = 0; i < MAG_LEN; ++i) {
        
        pOmega = ((i) / (float)MAG_LEN);
        omega = (20.0f * std::pow (10.0f, 3.0f * ((i+1) / (float)MAG_LEN))) / 20000.0;
    
        double m = \
        10.0 * log10(pow(((b0 + b1 + b2)/2.0), 2) - ((2.0*pow(sin(omega/2.0),2)) * (4.0 * b0 * b2 * (1.0 - (2.0*pow(sin(omega/2.0),2))) + b1 * (b0 + b2)))) -\
        10.0 * log10(pow(((a0 + a1 + a2)/2.0), 2) - ((2.0*pow(sin(omega/2.0),2)) * (4.0 * a0 * a2 * (1.0 - (2.0*pow(sin(omega/2.0),2))) + a1 * (a0 + a2))));
        
        if(isinf(m) || isnan(m))    {
            m = 0.0;
        }
                
        magnitudes[i] = m;
        frequencyResponse.lineTo(pOmega, m);
    }
    
    
}
