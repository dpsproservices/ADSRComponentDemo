//
//  ADSRWidget.h
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/13/22.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#define PADDING 20

#define MIN_ADSR_DURATION 0.0f
#define MAX_ADSR_DURATION 1.0f

#define MIN_SUSTAIN_LEVEL 0.0f
#define MAX_SUSTAIN_LEVEL 1.0f
#define DEFAULT_SUSTAIN_LEVEL 0.5f  // half way some decay, some release

class ADSRWidget : public juce::Component
{
public:
    
    ADSRWidget();
    
    void paint (juce::Graphics &g) override;
    
    void resized() override;

    //==============================================================================

    float getAttackDuration();
    
    float getDecayDuration();

    float getSustainLevel();
    
    float getReleaseDuration();
    
    float getAttackRate();

    float getDecayRate();
    
    float getReleaseRate();

    // reposition the ADSR control points based on the widget bounds and ADSR values
    void repositionPoints (const juce::Rectangle<int>& bounds);
    
    // resize and reposition the ADSR segment recetangles based on the control points
    void resizeSegments (const juce::Rectangle<int>& bounds);
    
    void drawGraph (juce::Graphics& g);
    
    void drawPoints (juce::Graphics& g);
    
    void drawControlPoint (juce::Graphics& g, const juce::Point<float>& controlPoint);

private:
    
    // ADSR parameters ranging [0..1]
    juce::Value attackDurationValue;
    juce::Value decayDurationValue;
    juce::Value sustainLevelValue;
    juce::Value releaseDurationValue;
    
    float pointDiameter;
    int pointFontSize;
    
    /*
        Line segment and gradient colors looked up from mockup image
     
        use html image color lookup --> https://html-color-codes.info/colors-from-image/
     
        convert hex color code to rgb --> https://www.rapidtables.com/convert/color/hex-to-rgb.html
    */

    juce::Colour backgroundColour;    // dark slate graphite #2B2B2B rgb(43, 43, 43)
    juce::Colour pathColour;          // bright neon blue #58FAFF rgb(88, 250, 255)
    juce::Colour gradientStartColour; // dark teal #3E4747 rgb(62, 71, 71)
    juce::Colour gradientMidColour;   // teal 449C9E rgb(68, 156, 158)
    juce::Colour gradientEndColour;   // light teal #449C9E rgb(68, 156, 158)
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

    juce::Point<float> attackRatePoint;      // attack rate XY model { [0.f ... 0.9f] , [0.f ... 0.9f] }
    juce::Point<float> attackControlPoint;   // attack rate draggable graph control point
    
    juce::Point<float> decayStartPoint;      // decay starts where attack ends
    juce::Point<float> decayRatePoint;       // decay rate XY model { [0.f ... 0.9f] , [0.f ... 0.9f] }
    juce::Point<float> decayControlPoint;    // decay rate draggable graph control point
    
    juce::Point<float> sustainStartPoint;    // sustain starts where decay ends, vertically draggable to adjust sustain level
    
    juce::Point<float> releaseStartPoint;    // release starts where sustain ends, also vertically draggable to adjust sustain level
    juce::Point<float> releaseRatePoint;     // release rate XY model { [0.f ... 0.9f] , [0.f ... 0.9f] }
    juce::Point<float> releaseControlPoint;  // release rate draggable graph control point
    juce::Point<float> releaseEndPoint;      // release duration draggable graph control point
    
    juce::Rectangle<int> attackRectangle;
    juce::Rectangle<int> decayRectangle;
    juce::Rectangle<int> sustainRectangle;
    juce::Rectangle<int> releaseRectangle;

    juce::Path path;
    juce::Path framePath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRWidget)
};
