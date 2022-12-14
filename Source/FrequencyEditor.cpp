/*
  ==============================================================================

    BodePlot.cpp
    Created: 15 Sep 2022 3:14:12pm
    Author:  Robin Prillwitz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FrequencyEditor.h"

const juce::Colour colors[] = {
    juce::Colours::orangered,
    juce::Colours::orange,
    juce::Colours::yellow,
    juce::Colours::greenyellow,
    juce::Colours::cyan,
    juce::Colours::dodgerblue,
};

//==============================================================================
FrequencyEditor::FrequencyEditor()
{
    tooltipWindow->setMillisecondsBeforeTipAppears (1000);
    
    frequencyResponse_l.preallocateSpace(MAG_LEN);
    frequencyResponse_r.preallocateSpace(MAG_LEN);
    
    source_l.addItem("LEFT", 1);
    source_l.addItem("RIGHT", 2);
    source_l.addItem("MONO", 3);
    source_l.setSelectedId(1);
    addAndMakeVisible(source_l);
    
    source_r.addItem("LEFT", 1);
    source_r.addItem("RIGHT", 2);
    source_r.addItem("MONO", 3);
    source_r.setSelectedId(2);
    addAndMakeVisible(source_r);
    
    source_label_l.attachToComponent(&source_l, false);
    addAndMakeVisible(source_label_l);
    source_label_r.attachToComponent(&source_r, false);
    addAndMakeVisible(source_label_r);
    
    gain_l.setRange(-maxDB, maxDB, 0.1);
    gain_r.setRange(-maxDB, maxDB, 0.1);
    gain_l.setValue(0.0);
    gain_r.setValue(0.0);
    gain_l.setTextValueSuffix(" dB");
    gain_r.setTextValueSuffix(" dB");
    gain_l.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::blue);
    gain_r.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::red);
    gain_l.setDoubleClickReturnValue(true, 0.0);
    gain_r.setDoubleClickReturnValue(true, 0.0);
    addAndMakeVisible(gain_l);
    addAndMakeVisible(gain_r);
    
    gain_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gain_label);

    float startF = 100;
    for (size_t i=0; i < numBands; ++i) {
        auto* bandEditor = bandEditors.add(
           new BandEditor (i, startF, colors[i])
       );
 
        bandEditor->event = [&]() {
            updateFrequencyResponse();
            repaint();
        };
        
        addAndMakeVisible (bandEditor);
        
        startF += startF;
    }
}

FrequencyEditor::~FrequencyEditor()
{
}

void FrequencyEditor::updateFrequencyResponse() {
    for(size_t i = 0; i < MAG_LEN; ++i) {
        magnitudes_l[i] = 0;
        magnitudes_r[i] = 0;
    }

    for (size_t i=0; i < numBands; ++i) {
        auto* bandEditor = bandEditors.getUnchecked (int (i));

        for(size_t j = 0; j < MAG_LEN; ++j) {
            if(bandEditor->getChannel())    {
                magnitudes_r[j] += bandEditor->magnitudes[j];
                if(isnan(magnitudes_r[j]))  {
                    magnitudes_r[j] = magnitudes_r[j-1];
                }
            }   else    {
                magnitudes_l[j] += bandEditor->magnitudes[j];
                if(isnan(magnitudes_l[j]))  {
                    magnitudes_l[j] = magnitudes_l[j-1];
                }
            }
        }
    }
    
    frequencyResponse_l.clear();
    frequencyResponse_r.clear();

    frequencyResponse_l.startNewSubPath(0, magnitudes_l[0]);
    frequencyResponse_r.startNewSubPath(0, magnitudes_r[0]);
    for(size_t i = 0; i < MAG_LEN; ++i) {
        frequencyResponse_l.lineTo((float)i / MAG_LEN, magnitudes_l[i]);
        frequencyResponse_r.lineTo((float)i / MAG_LEN, magnitudes_r[i]);
    }
}

void FrequencyEditor::paint (juce::Graphics& g)
{
    g.setFont (12.0f);
    g.setColour (juce::Colours::silver);
    g.drawRoundedRectangle (plotFrame.toFloat(), 5, 2); // background
    
    g.setColour(juce::Colour(0x29U, 0x2AU, 0x2FU));
    g.fillRoundedRectangle(plotFrame.toFloat(), 5);
    

    // calculate major vertical positions
    // dw: decade width
    float dw = plotFrame.getWidth() / 3.0;
    float padding = 10.0f;
    float xL[3];
    xL[0] = dw - (std::log10(2) * dw) + padding; // 100 Hz
    xL[1] = xL[0] + dw;                // 1 kHz
    xL[2] = xL[1] + dw;                // 10 kHz
    
    float x20k = xL[2] + (std::log10(2) * dw); // 20 kHz
    
    // draw major gridlines
    float majorGridLineAlpha = 0.5f;
    g.setColour (juce::Colours::silver.withAlpha (majorGridLineAlpha));
    g.drawVerticalLine (juce::roundToInt (xL[0]), float (plotFrame.getY()), float (plotFrame.getBottom()));
    g.drawVerticalLine (juce::roundToInt (xL[1]), float (plotFrame.getY()), float (plotFrame.getBottom()));
    g.drawVerticalLine (juce::roundToInt (xL[2]), float (plotFrame.getY()), float (plotFrame.getBottom()));

    // draw minor gridlines
    float minorGridLineAlpha = 0.2f;
    g.setColour (juce::Colours::silver.withAlpha (minorGridLineAlpha));
    // draw 20k seperately
    g.drawVerticalLine (juce::roundToInt (x20k), float (plotFrame.getY()), float (plotFrame.getBottom()));
    
    int minorGridLines = 10;
    g.setColour (juce::Colours::silver.withAlpha (minorGridLineAlpha));
    for(auto x : xL)    {
        for(int i = 1; i < minorGridLines; ++i) {
            auto offset = (std::log10(i) * dw) - dw;
            g.drawVerticalLine (juce::roundToInt (x + offset), float (plotFrame.getY()), float (plotFrame.getBottom()));
        }
    }

    // draw vertical lines
    g.setColour (juce::Colours::silver.withAlpha (0.3f));
    g.drawHorizontalLine (juce::roundToInt (plotFrame.getY() + 0.25 * plotFrame.getHeight()), float (plotFrame.getX()), float (plotFrame.getRight()));
    g.drawHorizontalLine (juce::roundToInt (plotFrame.getY() + 0.375 * plotFrame.getHeight()), float (plotFrame.getX()), float (plotFrame.getRight()));
    g.drawHorizontalLine (juce::roundToInt (plotFrame.getY() + 0.4375 * plotFrame.getHeight()), float (plotFrame.getX()), float (plotFrame.getRight()));
    g.drawHorizontalLine (juce::roundToInt (plotFrame.getY() + 0.5625 * plotFrame.getHeight()), float (plotFrame.getX()), float (plotFrame.getRight()));
    g.drawHorizontalLine (juce::roundToInt (plotFrame.getY() + 0.625 * plotFrame.getHeight()), float (plotFrame.getX()), float (plotFrame.getRight()));
    g.drawHorizontalLine (juce::roundToInt (plotFrame.getY() + 0.75 * plotFrame.getHeight()), float (plotFrame.getX()), float (plotFrame.getRight()));

    g.setColour (juce::Colours::greenyellow.withAlpha (0.5f));
    auto zeroDbY = plotFrame.getY() + 0.50 * plotFrame.getHeight();
    g.drawHorizontalLine (juce::roundToInt (zeroDbY), float (plotFrame.getX()), float (plotFrame.getRight()));

    g.reduceClipRegion (plotFrame);

    // draw band frequencys
    auto gain_l_offset = getPositionForGain((float) gain_l.getValue(),
                                                      (float) plotFrame.getY(),
                                                      (float) plotFrame.getBottom());
    auto gain_r_offset = getPositionForGain((float) gain_r.getValue(),
                                                      (float) plotFrame.getY(),
                                                      (float) plotFrame.getBottom());
    auto gainFactor = -((float)plotFrame.getBottom() - (float)plotFrame.getY()) / (2.0 * maxDB);
    
    for (size_t i=0; i < numBands; ++i) {
        auto* bandEditor = bandEditors.getUnchecked (int (i));

        g.setColour(bandEditor->colour);
        bandEditor->frequencyResponse.applyTransform (juce::AffineTransform::scale (plotFrame.getWidth(), gainFactor));
        if(bandEditor->getChannel())    {
            bandEditor->frequencyResponse.applyTransform (juce::AffineTransform::translation(padding, gain_r_offset));
        }   else    {
            bandEditor->frequencyResponse.applyTransform (juce::AffineTransform::translation(padding, gain_l_offset));
        }
        g.strokePath (bandEditor->frequencyResponse, juce::PathStrokeType (2.0));

    }
    
    frequencyResponse_l.applyTransform (juce::AffineTransform::scale (plotFrame.getWidth(), gainFactor));
    frequencyResponse_l.applyTransform (juce::AffineTransform::translation(plotFrame.getX() + (padding / 2.0) + 1, gain_l_offset));
    frequencyResponse_r.applyTransform (juce::AffineTransform::scale (plotFrame.getWidth(), gainFactor));
    frequencyResponse_r.applyTransform (juce::AffineTransform::translation(plotFrame.getX() + (padding / 2.0) + 1, gain_r_offset));
    
    g.setColour(juce::Colours::blue);
    g.strokePath (frequencyResponse_l, juce::PathStrokeType (6.0));
    g.setColour(juce::Colours::white);
    g.strokePath (frequencyResponse_l, juce::PathStrokeType (2.5));
    
    g.setColour(juce::Colours::red);
    g.strokePath (frequencyResponse_r, juce::PathStrokeType (6.0));
    g.setColour(juce::Colours::white);
    g.strokePath (frequencyResponse_r, juce::PathStrokeType (2.5));
    
    // draw editor handles
    for(size_t i = 0; i < numBands; ++i)    {
        auto* bandEditor = bandEditors.getUnchecked(int (i));
        
        auto x = juce::roundToInt (plotFrame.getX() + plotFrame.getWidth() * getPositionForFrequency (float (bandEditor->getFrequncy())) + (padding / 2.0f));
        auto y = juce::roundToInt(getPositionForGain(bandEditor->getGain() + (bandEditor->getChannel() ? gain_r.getValue() : gain_l.getValue()), plotFrame.getY(), plotFrame.getBottom()));

        float radius = 10;
        
        g.setColour(bandEditor->colour);
        g.setOpacity(0.4);
        g.drawVerticalLine (x, float (plotFrame.getY()), float (y - (radius - 1)));
        g.drawVerticalLine (x, float (y + (radius - 1)), float (plotFrame.getBottom()));
        
        g.setOpacity(0.4);
        g.fillEllipse (float (x - ((radius + clickRadius) / 2)), float (y - ((radius + clickRadius) / 2)), radius + clickRadius, radius + clickRadius);
        g.setOpacity(1.0);
        g.fillEllipse (float (x - (radius / 2)), float (y - (radius / 2)), radius, radius);
    }
    
    // draw vertical labels
    g.setColour (juce::Colours::silver);
    g.drawFittedText (juce::String (maxDB) + " dB", plotFrame.getX() + 3, plotFrame.getY() + 2, 50, 14, juce::Justification::left, 1);
    g.drawFittedText (juce::String (maxDB / 2) + " dB", plotFrame.getX() + 3, juce::roundToInt (plotFrame.getY() + 2 + 0.25 * plotFrame.getHeight()), 50, 14, juce::Justification::left, 1);
    g.drawFittedText (juce::String (- maxDB / 2) + " dB", plotFrame.getX() + 3, juce::roundToInt (plotFrame.getY() + 2 + 0.75 * plotFrame.getHeight()), 50, 14, juce::Justification::left, 1);
    
    g.setColour (juce::Colours::greenyellow);
    g.drawFittedText (" 0 dB", plotFrame.getX() + 3, juce::roundToInt (plotFrame.getY() + 2 + 0.5  * plotFrame.getHeight()), 50, 14, juce::Justification::left, 1);
    
    // draw horizontal labels
    for(auto x : xL)    {
        for(int i = 1; i < minorGridLines; ++i) {
            auto offset = (std::log10(i) * dw) - dw;
 
            if( i % 2 == 0) {
                g.setColour (juce::Colours::silver);
                auto freq = getFrequencyForPosition ((x+offset - padding) / (3.0 * dw));
                auto freqText = (freq < 1000) ? juce::String (freq) + " Hz" : juce::String (freq / 1000, 1) + " kHz";
                                        
                g.drawFittedText(freqText, juce::roundToInt (x + offset + 3), plotFrame.getBottom() - 18, 50, 15, juce::Justification::left, 1);
                
            }
        }
        
        g.setColour (juce::Colours::silver);
        auto freq = getFrequencyForPosition ((x - padding) / (3.0 * dw));
        auto freqText = (freq < 1000) ? juce::String (freq) + " Hz" : juce::String (freq / 1000, 1) + " kHz";
                
        g.drawFittedText(freqText, juce::roundToInt (x + 3), plotFrame.getBottom() - 18, 50, 15, juce::Justification::left, 1);
    }
    
    
    
}

void FrequencyEditor::resized()
{
    plotFrame = getLocalBounds().reduced (3, 3);
    updateFrequencyResponse();
    
    auto bandSpace = plotFrame.removeFromBottom (150.0);
    
    auto sourceSpace = bandSpace.removeFromLeft(90);
    sourceSpace.reduce(5, 30);
    source_l.setBounds(sourceSpace.removeFromTop(30));
    sourceSpace.removeFromTop(30);
    source_r.setBounds(sourceSpace.removeFromTop(30));
    
    auto gainSpace = bandSpace.removeFromLeft(90);
    gain_label.setBounds(gainSpace.removeFromTop(18));
    gain_l.setBounds(gainSpace.removeFromTop(gainSpace.getHeight() / 2));
    gain_r.setBounds(gainSpace);
    
    auto width = juce::roundToInt (bandSpace.getWidth() / bandEditors.size());
    width = width >= 200.0 ? 200.0 : width;

    for (auto* bandEditor : bandEditors)
        bandEditor->setBounds (bandSpace.removeFromLeft (width));
    
    plotFrame.reduce (3, 3);
}

void FrequencyEditor::mouseMove (const juce::MouseEvent& e)
{
    if (plotFrame.contains (e.x, e.y))
    {
        for (int i=0; i < bandEditors.size(); ++i)
        {
            auto* band = bandEditors.getUnchecked(i);
            
            auto pos = plotFrame.getX() + getPositionForFrequency (float (band->getFrequncy())) * plotFrame.getWidth();

            if (std::abs (pos - e.position.getX()) < clickRadius)
            {
                                                                  
                auto gain = band->getGain() + (band->getChannel() ? gain_r.getValue() : gain_l.getValue());
                if (std::abs(getPositionForGain(gain, plotFrame.getY(), plotFrame.getBottom())
                            - e.position.getY()) < clickRadius)
                {
                    draggingGain = true;
                    setMouseCursor (juce::MouseCursor (juce::MouseCursor::UpDownLeftRightResizeCursor));
                }
                else
                {
                    draggingGain = false;
                    setMouseCursor (juce::MouseCursor (juce::MouseCursor::LeftRightResizeCursor));
                }

                if (i != draggingBand)
                {
                    draggingBand = i;
                    repaint (plotFrame);
                }
                return;
            }
            
        }
    }
    draggingBand = -1;
    draggingGain = false;
    setMouseCursor (juce::MouseCursor (juce::MouseCursor::NormalCursor));
}

void FrequencyEditor::mouseDrag (const juce::MouseEvent& e)
{
    if (juce::isPositiveAndBelow (draggingBand, bandEditors.size()))
    {
        auto pos = (e.position.getX() - plotFrame.getX()) / plotFrame.getWidth();
        bandEditors [draggingBand]->setFrequency (getFrequencyForPosition (pos));
        if (draggingGain)   {
            auto bandOffset = bandEditors[draggingBand]->getChannel() ? gain_r.getValue() : gain_l.getValue();
            auto gain = getGainForPosition (e.position.getY(), plotFrame.getY(), plotFrame.getBottom()) - bandOffset;
            bandEditors [draggingBand]->setGain(gain);
        }
    }
}

float FrequencyEditor::getFrequencyForPosition (float pos)
{
    return 20.0f * std::pow (10.0f, 3.0f * pos);
}

float FrequencyEditor::getPositionForFrequency (float freq)
{
    return std::log10 (freq / 20.0f) / 3.0f;
}

float FrequencyEditor::getPositionForGain (float gain, float top, float bottom)
{
    return juce::jmap(gain, -maxDB, maxDB, bottom, top);
}

float FrequencyEditor::getGainForPosition (float pos, float top, float bottom)
{
    return juce::jmap (pos, bottom, top, -maxDB, maxDB);
}
