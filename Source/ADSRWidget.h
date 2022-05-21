//
//  ADSRWidget.h
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/13/22.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "DraggablePoint.h"

#define PADDING 40

#define MIN_ADSR_DURATION 0.1f
#define MAX_ADSR_DURATION 1.0f

#define MIN_SUSTAIN_LEVEL 0.0f
#define MAX_SUSTAIN_LEVEL 1.0f
#define DEFAULT_SUSTAIN_LEVEL 0.5f  // half way some decay, some release

#define ADSR_POINT_SIZE 40 // width of the draggable point component
#define OFFSET 20 // half the width of the draggable point component
#define ADSR_FONT_SIZE 16

class ADSRWidget : public juce::Component
{
public:
    
    ADSRWidget();
    
    void paint (juce::Graphics &g) override;
    
    void resized() override;
    
    //==============================================================================

    void mouseDown (const juce::MouseEvent& mouseEvent) override;

    void mouseDrag (const juce::MouseEvent& mouseEvent) override;

    //==============================================================================

    float getAttackDuration();
    
    float getDecayDuration();

    float getSustainLevel();
    
    float getReleaseDuration();
    
    float getAttackRate();

    float getDecayRate();
    
    float getReleaseRate();
    
    void recalculateBounds();

    // reposition the ADSR control points based on the widget bounds and ADSR values
    void repositionPoints();
    
    // resize and reposition the ADSR segment rectangles based on the control points
    void resizeSegments();
    
    void drawGraph (juce::Graphics& g);

    // update model and repaint
    void update();

private:
    
    juce::Rectangle<int> bounds;
    int topEdgeY;
    int bottomEdgeY;
    int leftEdgeX;
    int rightEdgeX;
    int width;
    int height;
    int equalSegmentWidth;
    
    // ADSR parameters ranging [0..1]
    juce::Value attackDurationValue;
    juce::Value decayDurationValue;
    juce::Value sustainLevelValue;
    juce::Value releaseDurationValue;
    
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
    juce::Colour selectedColour;      // grey
    
    juce::ColourGradient gradient;    // light to dark teal
    
    /*
        ADSR graph Path line segment bezier curve start, control, end points
        sustain is flat line segment only horizontal time value
        no sustain bezier curve control point
     
        control point will only appear on the bezier curve
        while holding mouse down on the draggable segment area to adjust it
     
        the control point will disappear on mouse up off the segment area
     
        model control points bound inside the segment area rectangle
        with XY coordinates ranging { [0.f ... 0.9f] , [0.f ... 0.9f] }
    */

    juce::Point<float> attackRatePoint;      // attack rate XY model
    juce::Point<float> attackControlPoint;   // attack rate draggable graph control point
    juce::Point<float> decayStartPoint;      // decay starts where attack ends
    juce::Point<float> decayRatePoint;       // decay rate XY model
    juce::Point<float> decayControlPoint;    // decay rate draggable graph control point
    juce::Point<float> sustainStartPoint;    // sustain starts where decay ends, vertically draggable to adjust sustain level
    juce::Point<float> releaseStartPoint;    // release starts where sustain ends, also vertically draggable to adjust sustain level
    juce::Point<float> releaseRatePoint;     // release rate XY model
    juce::Point<float> releaseControlPoint;  // release rate draggable graph control point
    juce::Point<float> releaseEndPoint;      // release duration draggable graph control point

    juce::Path path;
    juce::Path framePath;
    
    juce::OwnedArray<DraggablePoint> draggablePoints;
    
    juce::ComponentBoundsConstrainer constrainer;

    juce::ComponentDragger dragger;
    
    juce::Component* draggedComponent = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRWidget)
};
