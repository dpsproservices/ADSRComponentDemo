//
//  DraaggablePoint.hpp
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/16/22.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"

class DraggablePoint : public juce::Component
{
public:

    DraggablePoint();
    
    void paint (juce::Graphics &g) override;
    
    void resized() override;
    
    //==============================================================================
    
    void mouseEnter (const juce::MouseEvent& mouseEvent) override;

    void mouseExit (const juce::MouseEvent& mouseEvent) override;
    
    //==============================================================================
    
    void setLabel (const juce::String& pointLabel);
    
    void setFontSize (const int& size);
    
    void setFontColour (const juce::Colour& colour);
    
    void setOuterFillColour (const juce::Colour& colour);
    
    void setInnerFillColour (const juce::Colour& colour);
    
    void setVisible ( const bool& visible);
    
    void setDrawGuides ( const bool& isDrawGuides);
    
    //==============================================================================
    
    juce::Point<float> modelPoint, centerPoint;

private:

    juce::String label; // "A", "D", "S", "R"
    
    juce::Rectangle<int> outerBounds, innerBounds;
    
    int outerX, outerY, outerWidth, outerHeight;
    
    int innerX, innerY, innerWidth, innerHeight;
    
    int outerRadius, innerRadius, fontSize;
    
    bool isMouseHover, isVisible, isDrawGuides;
    
    juce::Colour outerFillColour, innerFillColour, fontColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DraggablePoint)
};
