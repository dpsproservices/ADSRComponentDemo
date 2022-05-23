//
//  DraggablePoint.cpp
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/16/22.
//

#include "DraggablePoint.h"

DraggablePoint::DraggablePoint() : isMouseHover (false), isDrawGuides (true) { }

void DraggablePoint::paint (juce::Graphics &g)
{
    if (isVisible)
    {
        // draw the outer circle to indicate point is selected
        if (isDrawGuides)
        {
            if (isMouseHover)
            {
                g.setOpacity (0.15f);
                g.setColour (outerFillColour);
                g.fillEllipse (outerX, outerY, outerWidth, outerWidth);
            }
        }
        
        // draw the inner circle which will be painted inside on top of the outer circle
        g.setOpacity (0.75f);
        g.setColour (innerFillColour);
        g.fillEllipse (innerX, innerY, innerWidth, innerWidth);
        
        // draw the label
        g.setOpacity (1.f);
        g.setColour (fontColour);
        g.setFont (fontSize);
        g.drawText (label, innerX, innerY, innerWidth, innerHeight, juce::Justification::centred);
    }
}

void DraggablePoint::resized()
{
    outerBounds = getLocalBounds();
    outerX = outerBounds.getX();
    outerY = outerBounds.getY();
    outerWidth = outerBounds.getWidth();
    outerHeight = outerBounds.getHeight();
    
    outerRadius = static_cast<float> (outerWidth / 2.f);

    centerPoint.setXY (outerRadius, outerRadius);
    
    innerBounds = outerBounds.reduced (5);
    innerX = innerBounds.getX();
    innerY = innerBounds.getY();
    innerWidth = innerBounds.getWidth();
    innerHeight = innerBounds.getHeight();
    
    innerRadius = static_cast<float> (innerWidth / 2.f);
}

//==============================================================================

void DraggablePoint::mouseEnter (const juce::MouseEvent& mouseEvent)
{
    isMouseHover = true;
    repaint();
}

void DraggablePoint::mouseExit (const juce::MouseEvent& mouseEvent)
{
    isMouseHover = false;
    repaint();
}

//==============================================================================

void DraggablePoint::setLabel (const juce::String& pointLabel) { label = pointLabel; }

void DraggablePoint::setFontSize (const int& size) { fontSize = size; }

void DraggablePoint::setFontColour (const juce::Colour& colour) { fontColour = colour; }

void DraggablePoint::setOuterFillColour (const juce::Colour& colour) { outerFillColour = colour; }

void DraggablePoint::setInnerFillColour (const juce::Colour& colour) { innerFillColour = colour; }

void DraggablePoint::setVisible ( const bool& visible) { isVisible = visible; }

void DraggablePoint::setDrawGuides ( const bool& drawGuides) { isDrawGuides = drawGuides; }
