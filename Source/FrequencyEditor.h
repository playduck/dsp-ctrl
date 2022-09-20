/*
  ==============================================================================

    BodePlot.h
    Created: 15 Sep 2022 3:14:12pm
    Author:  Robin Prillwitz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define MAG_LEN (500)

const float maxDB  = 24.0f;
const int numBands = 6;
const int clickRadius = 10;

//==============================================================================
/*
*/
class FrequencyEditor  : public juce::Component
{
public:
    FrequencyEditor();
    ~FrequencyEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    
    class BandEditor : public juce::Component,
                           public juce::Button::Listener
        {
        public:
            BandEditor (size_t i, float startF, juce::Colour c);

            void paint(juce::Graphics& g) override;
            
            void resized () override;
            void updateControls ();
            void updateSoloState (bool isSolo);
            void setFrequency (float frequency);
            void setGain (float gain);
            void setType (int type);
            void buttonClicked (juce::Button* b) override;
            
            float getGain();
            float getFrequncy();
            bool getChannel();
            
            void calc();
            
            std::function<void()> event;
            
            float magnitudes[MAG_LEN];
            juce::Path frequencyResponse;
            
            juce::Colour colour;
            
        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandEditor)

            size_t index;
            
            juce::GroupComponent    frame;
            juce::ComboBox          filterType;
            juce::Slider            frequency { juce::Slider::LinearBar, juce::Slider::TextBoxAbove };
            juce::Slider            quality   { juce::Slider::LinearBar, juce::Slider::TextBoxAbove };
            juce::Slider            gain      { juce::Slider::LinearBar, juce::Slider::TextBoxAbove };
            
            juce::TextButton        solo      { TRANS ("S") };
            juce::TextButton        activate  { TRANS ("A") };
            juce::TextButton        channel   { TRANS ("L") };
            
            juce::Label             typeLabel { TRANS ("Filter Type"), TRANS ("TYPE") };
            juce::Label             freqLabel { TRANS ("Frequency"),  TRANS ("FREQ") };
            juce::Label             gainLabel { TRANS ("Gain"), TRANS ("GAIN") };
            juce::Label             qualityLabel { TRANS ("Quality"), TRANS ("Q") };
            
            typedef enum {
                lowpass = 1,
                highpass,
                peq
            } filter_type_t;
            filter_type_t type;
        };
    
private:
    int draggingBand = -1;
    bool draggingGain = false;
    
    juce::Rectangle<int>          plotFrame;
    
    juce::OwnedArray<BandEditor>  bandEditors;
    
    juce::SharedResourcePointer<juce::TooltipWindow> tooltipWindow;

    float magnitudes_l[MAG_LEN];
    juce::Path frequencyResponse_l;
    
    float magnitudes_r[MAG_LEN];
    juce::Path frequencyResponse_r;

    float getFrequencyForPosition (float pos);
    float getPositionForFrequency (float freq);
    float getPositionForGain (float gain, float top, float bottom);
    float getGainForPosition (float pos, float top, float bottom);
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyEditor)
};
