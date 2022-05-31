//
//  ADSRWidget.h
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/13/22.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Constants.h"
#include "CustomLookAndFeel.h"
#include "DraggablePoint.h"
#include "ValuePoint.h"
#include "ADSRModel.h"

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
    
    const ValuePoint<float>& getAttackRate();

    const ValuePoint<float>& getDecayRate();
    
    const ValuePoint<float>& getReleaseRate();
    
    void recalculateBounds();

    // reposition the ADSR control points based on the widget bounds and ADSR values
    void repositionPoints();
    
    // resize and reposition the ADSR segment rectangles based on the control points
    void resizeSegments();
    
    // draw the ADSR envelope path filled with a gradient
    void drawGraph (juce::Graphics& g);

    // update model and repaint
    void update();
    
    void constrainPointToArea (
        const juce::Point<float>& draggedPoint,
        const juce::Rectangle<float>& area,
        juce::Point<float>& controlPoint
    );
    
    void constrainPointToModel (
        const ValuePoint<float>& modelPoint, // center of the model point
        const float& modelLimitMin,
        const float& modelLimitMax,
        const juce::Rectangle<float>& area,
        juce::Point<float>& controlPoint
    );

private:
    
    ADSRModel model; // limits and parameters defaults initialized from Constants.h
    
    juce::Rectangle<int> bounds;
    int topEdgeY;
    int bottomEdgeY;
    int leftEdgeX;
    int rightEdgeX;
    int width;
    int height;
    int equalSegmentWidth;

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
    juce::Colour controlPointColour;  // magenta #82003D rgb(130, 0, 61)
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
        with XY coordinates
    */
    
    juce::Point<float> attackRatePoint;         // attack rate draggable graph control point
    juce::Point<float> decayRatePoint;          // decay rate draggable graph control point
    juce::Point<float> releaseRatePoint;        // release rate draggable graph control point
    
    juce::Point<float> attackDurationPoint;     // horizontal drag to adjust attack duration
    juce::Point<float> decayDurationPoint;      // horizontal drag to adjust decay duration
    juce::Point<float> sustainLevelPoint;       // drag to adjust sustain level
    juce::Point<float> releaseDurationPoint;    // horizontal drag to adjust release duration
    
    juce::Rectangle<float> attackRateArea;      // attack rate drag point bounds
    juce::Rectangle<float> decayRateArea;       // decay rate drag point bounds
    juce::Rectangle<float> releaseRateArea;     // release rate drag point bounds
    
    juce::Rectangle<float> attackDurationArea;  // attack duration drag point bounds
    juce::Rectangle<float> decayDurationArea;   // decay duration drag point bounds
    juce::Rectangle<float> sustainLevelArea;    // sustain level drag point bounds
    juce::Rectangle<float> releaseDurationArea; // release duration drag point bounds

    juce::Path graphPath;   // graphed ADSR segments filled with vertical gradient
    juce::Path framePath;   // frame border bottom edge of the graph
    
    juce::OwnedArray<DraggablePoint> draggablePoints;

    juce::ComponentDragger dragger;
    
    juce::Component* draggedComponent = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRWidget)
};
