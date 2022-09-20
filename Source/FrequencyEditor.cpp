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
    juce::Colours::red,
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
    
    frequencyResponse.preallocateSpace(MAG_LEN);

    float startF = 100;
    for (size_t i=0; i < numBands; ++i) {
        auto* bandEditor = bandEditors.add(
           new BandEditor (i, startF, colors[i])
       );
 
        bandEditor->event = [&]() {
            repaint();
        };
        
        addAndMakeVisible (bandEditor);
        
        startF += startF;
    }
}

FrequencyEditor::~FrequencyEditor()
{
}

void FrequencyEditor::paint (juce::Graphics& g)
{
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setFont (12.0f);
    g.setColour (juce::Colours::silver);
    g.drawRoundedRectangle (plotFrame.toFloat(), 5, 2); // background
    

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
    g.drawHorizontalLine (juce::roundToInt (plotFrame.getY() + 0.625 * plotFrame.getHeight()), float (plotFrame.getX()), float (plotFrame.getRight()));
    g.drawHorizontalLine (juce::roundToInt (plotFrame.getY() + 0.75 * plotFrame.getHeight()), float (plotFrame.getX()), float (plotFrame.getRight()));

    g.setColour (juce::Colours::greenyellow.withAlpha (0.5f));
    auto zeroDbY = plotFrame.getY() + 0.50 * plotFrame.getHeight();
    g.drawHorizontalLine (juce::roundToInt (zeroDbY), float (plotFrame.getX()), float (plotFrame.getRight()));

    g.reduceClipRegion (plotFrame);
    
    frequencyResponse.clear();
    for(size_t i = 0; i < MAG_LEN; ++i) {
        magnitudes[i] = 0.0;
    }

    // draw band frequencys
    for (size_t i=0; i < numBands; ++i) {
        auto* bandEditor = bandEditors.getUnchecked (int (i));

        g.setColour(bandEditor->colour);
        bandEditor->frequencyResponse.applyTransform (juce::AffineTransform::scale (3*dw, -maxDB/2));
        bandEditor->frequencyResponse.applyTransform (juce::AffineTransform::translation(2+padding, zeroDbY));
        g.strokePath (bandEditor->frequencyResponse, juce::PathStrokeType (2.0));

        for(size_t j = 0; j < MAG_LEN; ++j) {
            magnitudes[j] = magnitudes[j] + bandEditor->magnitudes[j];
            if(isnan(magnitudes[j]))    {
                magnitudes[j] = 0.0;
            }
        }
    }
    
    for(size_t i = 0; i < MAG_LEN; ++i) {
        frequencyResponse.lineTo((float)i / MAG_LEN, magnitudes[i]);
    }
    
    frequencyResponse.applyTransform (juce::AffineTransform::scale (3*dw, -maxDB/2));
    frequencyResponse.applyTransform (juce::AffineTransform::translation(2+padding, zeroDbY));

    g.setColour(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.strokePath (frequencyResponse, juce::PathStrokeType (6.0));
    g.setColour(juce::Colours::ghostwhite);
    g.strokePath (frequencyResponse, juce::PathStrokeType (2.5));

    // draw editor handles
    for(size_t i = 0; i < numBands; ++i)    {
        auto* bandEditor = bandEditors.getUnchecked(int (i));
        
        auto x = juce::roundToInt (plotFrame.getX() + plotFrame.getWidth() * getPositionForFrequency (float (bandEditor->getFrequncy())) + (padding / 2.0f));
        auto y = juce::roundToInt (getPositionForGain (float (bandEditor->getGain()),
                                                       float (plotFrame.getY()),
                                                       float (plotFrame.getBottom()) ));

        float radius = 10;
        
        g.setColour(bandEditor->colour);
        g.drawVerticalLine (x, float (plotFrame.getY()), float (y - (radius - 1)));
        g.drawVerticalLine (x, float (y + (radius - 1)), float (plotFrame.getBottom()));
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
    
    auto width = juce::roundToInt (plotFrame.getWidth()) / (bandEditors.size());
    auto bandSpace = plotFrame.removeFromBottom (250.0);
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
                if (std::abs (getPositionForGain (float (band->getGain()), float (plotFrame.getY()), float (plotFrame.getBottom()))
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
            float gain = getGainForPosition (e.position.getY(), float (plotFrame.getY()), float (plotFrame.getBottom()));
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
