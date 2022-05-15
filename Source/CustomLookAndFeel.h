//
//  CustomLookAndFeel.h
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/14/22.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

struct CustomLookAndFeel : juce::LookAndFeel_V4
{
    CustomLookAndFeel()
    {
        juce::Colour pointColour (0, 58, 160); // royal blue
        juce::Colour controlPointColour (0, 58, 160); // magenta
        
        setColour (juce::Slider::thumbColourId, juce::Colours::white); // Slider thumb color
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    }

    void drawLinearSlider (
        juce::Graphics& g,
        int   x,
        int   y,
        int   width,
        int   height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const juce::Slider::SliderStyle style,
        juce::Slider& slider
    ) override
    {
        if (style == juce::Slider::SliderStyle::LinearBarVertical)
        {
            g.setColour (slider.findColour (juce::Slider::thumbColourId));
            g.drawHorizontalLine (sliderPos, x, width);
        }
        else if (style == juce::Slider::SliderStyle::LinearHorizontal)
        {
            g.setColour (slider.findColour (juce::Slider::thumbColourId));
            g.drawVerticalLine (sliderPos, y, height);
        }
        else
        {
            juce::LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
    }
    
};
