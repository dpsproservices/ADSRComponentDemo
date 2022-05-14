//
//  ADSRWidget.h
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/13/22.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ADSRWidget : public juce::Component
{
public:
    
    ADSRWidget();
    
    void paint (juce::Graphics &g) override;
    
    void resized() override;

    //==============================================================================
    
    void drawGraph (juce::Graphics& g);
    
    void drawPoints (juce::Graphics& g);
    
    void drawControlPoint (juce::Graphics& g, const juce::Point<float>& controlPoint);
    
private:

    /*
        A D S R duration time values in seconds 0.1 to 1.0
    */

    juce::Value attackDurationValue;
    juce::Value decayDurationValue;
    juce::Value sustainDurationValue;
    juce::Value releaseDurationValue;
    
    /*
        A D R curvature values range from +50% to -50%
     
        sustain is flat line segment only horizontal time value
        no sustain curve value
    */

    juce::Value attackCurveValue;
    juce::Value decayCurveValue;
    juce::Value releaseCurveValue;
    
    float pointDiameter;
    int pointFontSize;
    
    /*
        Line segment and gradient colors looked up from mockup image
     
        use html image color lookup --> https://html-color-codes.info/colors-from-image/
     
        convert hex color code to rgb --> https://www.rapidtables.com/convert/color/hex-to-rgb.html
    */

    juce::Colour backgroundColour;    // dark slate graphite #2B2B2B rgb(43, 43, 43)
    juce::Colour pathColour;          // bright neon blue #58FAFF rgb(88, 250, 255)
    juce::Colour gradientStartColour; // light teal #449C9E rgb(68, 156, 158)  #53E7EB rgb(83, 231, 235)
    juce::Colour gradientMidColour;   // teal 449C9E rgb(68, 156, 158)
    juce::Colour gradientEndColour;   // dark teal #3E4747 rgb(62, 71, 71)
    juce::Colour pointColour;         // royal blue #003AA0 rgb(0, 58, 160)
    juce::Colour controlPointColour;  // magenta #82003D rgb(0, 58, 160)
    
    juce::ColourGradient gradient;    // light to dark teal
    
    /*
        ADSR graph Path line segment bezier curve start, control, end points
        sustain is flat line segment only horizontal time value
        no sustain bezier curve control point
     
        control point will only appear on the bezier curve
        while holding mouse down on the draggable segment area to adjust it
     
        the control point will disappear on mouse up off the segment area
    */

    juce::Point<float> attackStartPoint;
    juce::Point<float> attackControlPoint;

    juce::Point<float> decayStartPoint;      // decay starts where attack ends
    juce::Point<float> decayControlPoint;
    
    juce::Point<float> sustainStartPoint;    // sustain starts where decay ends
    
    juce::Point<float> releaseStartPoint;    // release starts where sustain ends
    juce::Point<float> releaseControlPoint;
    juce::Point<float> releaseEndPoint;
    
    juce::Rectangle<float> attackRectangle;
    juce::Rectangle<float> decayRectangle;
    juce::Rectangle<float> sustainRectangle;
    juce::Rectangle<float> releaseRectangle;

    juce::Path path;
    juce::Path framePath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRWidget)
};
