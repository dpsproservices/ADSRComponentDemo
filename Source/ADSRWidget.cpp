//
//  ADSRWidget.cpp
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/13/22.
//

#include "ADSRWidget.h"

ADSRWidget::ADSRWidget() :
    model(),
    bounds(),
    backgroundColour (43, 43, 43),
    pathColour (88, 250, 255),
    gradientStartColour (62, 71, 71),
    gradientMidColour (68, 156, 158),
    gradientEndColour (83, 231, 235),
    pointColour (0, 58, 160),
    controlPointColour (130, 0, 61),
    selectedColour (127, 127, 127),
    gradient(),
    attackRatePoint(),
    decayRatePoint(),
    releaseRatePoint(),
    attackDurationPoint(),
    decayDurationPoint(),
    sustainLevelPoint(),
    releaseDurationPoint(),
    attackRateArea(),
    decayRateArea(),
    releaseRateArea(),
    attackDurationArea(),
    decayDurationArea(),
    sustainLevelArea(),
    releaseDurationArea(),
    graphPath(),
    framePath()
{
    draggedComponent = nullptr;

    for ( int i = 0; i < 7; ++i )
    {
        auto* draggablePoint = draggablePoints.add (new DraggablePoint());
        addAndMakeVisible (draggablePoint);
        
        draggablePoint->setFontColour (juce::Colours::white);
        draggablePoint->setFontSize (FONT_SIZE);
        draggablePoint->setOuterFillColour (selectedColour);

        if (i==0) // Attack
        {
            draggablePoint->setSize (POINT_SIZE, POINT_SIZE);
            draggablePoint->setLabel ("A");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else if (i==2) // Decay
        {
            draggablePoint->setSize (POINT_SIZE, POINT_SIZE);
            draggablePoint->setLabel ("D");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else if (i==4) // Sustain
        {
            draggablePoint->setSize (POINT_SIZE, POINT_SIZE);
            draggablePoint->setLabel ("S");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else if (i==6) // Release
        {
            draggablePoint->setSize (POINT_SIZE, POINT_SIZE);
            draggablePoint->setLabel ("R");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else // rate curve control points in between points A, D, R
        {
            draggablePoint->setSize (RATE_POINT_SIZE, RATE_POINT_SIZE);
            draggablePoint->setLabel ("");
            draggablePoint->setInnerFillColour (controlPointColour);
            draggablePoint->setVisible (true);
        }

        draggablePoint->addMouseListener (this, false);
    }
}
    
void ADSRWidget::paint (juce::Graphics &g)
{
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds());
    drawGraph (g);
}

void ADSRWidget::resized()
{
    recalculateBounds();
    repositionPoints();

    gradient.clearColours();

    // resize the vertical line gradient to fill underneath the graph path
    gradient.point1 = juce::Point<float> (leftEdgeX, bottomEdgeY);
    gradient.point2 = juce::Point<float> (leftEdgeX, topEdgeY);
    
    gradient.addColour (0.f, gradientStartColour);
    gradient.addColour (0.15f, gradientMidColour);
    gradient.addColour (1.f, gradientEndColour);
}

//==============================================================================

void ADSRWidget::mouseDown (const juce::MouseEvent& mouseEvent)
{
    draggedComponent = nullptr;

    for (auto* draggablePoint : draggablePoints)
    {
        if (draggablePoint->getBounds().contains (mouseEvent.getEventRelativeTo(this).getPosition()))
        {
            draggedComponent = draggablePoint;
            break;
        }
    }

    if (draggedComponent)
        dragger.startDraggingComponent (draggedComponent, mouseEvent);
}

void ADSRWidget::mouseDrag (const juce::MouseEvent& mouseEvent)
{
    if (draggedComponent != nullptr)
    {
        dragger.dragComponent (draggedComponent, mouseEvent, nullptr);
        
        // Attack
        if (draggedComponent == draggablePoints.getUnchecked (0))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), attackDurationArea, attackDurationPoint);
            
            auto duration = juce::jmap (
                attackDurationPoint.getX(),
                attackDurationArea.getX() + OFFSET,
                attackDurationArea.getRight() - OFFSET,
                MIN_DURATION,
                MAX_DURATION
            );

            duration = juce::jlimit (MIN_DURATION, MAX_DURATION, duration);
            model.setAttackDuration (duration);
        }
        // Decay
        else if (draggedComponent == draggablePoints.getUnchecked (2))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), decayDurationArea, decayDurationPoint);
            
            auto duration = juce::jmap (
                decayDurationPoint.getX(),
                decayDurationArea.getX() + OFFSET,
                decayDurationArea.getRight() - OFFSET,
                MIN_DURATION,
                MAX_DURATION
            );

            duration = juce::jlimit (MIN_DURATION, MAX_DURATION, duration);
            model.setDecayDuration (duration);
        }
        // Sustain
        else if (draggedComponent == draggablePoints.getUnchecked (4))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), sustainLevelArea, sustainLevelPoint);
            
            auto sustain = juce::jmap (
                sustainLevelPoint.getY(),
                sustainLevelArea.getY() + OFFSET,
                sustainLevelArea.getBottom() - OFFSET,
                MIN_SUSTAIN_LEVEL,
                MAX_SUSTAIN_LEVEL
            );

            sustain = juce::jlimit (MIN_SUSTAIN_LEVEL, MAX_SUSTAIN_LEVEL, sustain);
            model.setSustainLevel (sustain);
        }
        // Release
        else if (draggedComponent == draggablePoints.getUnchecked (6))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), releaseDurationArea, releaseDurationPoint);
            
            auto duration = juce::jmap (
                releaseDurationPoint.getX(),
                releaseDurationArea.getX() + OFFSET,
                releaseDurationArea.getRight() - OFFSET,
                MIN_DURATION,
                MAX_DURATION
            );

            duration = juce::jlimit (MIN_DURATION, MAX_DURATION, duration);
            model.setReleaseDuration (duration);
        }
        // Attack rate curve control point
        else if (draggedComponent == draggablePoints.getUnchecked (1))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), attackRateArea, attackRatePoint);

            auto x = juce::jmap (
                attackRatePoint.getX(),
                attackRateArea.getX() + OFFSET,
                attackRateArea.getRight() - OFFSET,
                MIN_RATE,
                MAX_RATE
            );

            auto y = juce::jmap (
                attackRatePoint.getY(),
                attackRateArea.getY() + OFFSET,
                attackRateArea.getBottom() - OFFSET,
                MIN_RATE,
                MAX_RATE
            );

            x = juce::jlimit (MIN_RATE, MAX_RATE, x);
            y = juce::jlimit (MIN_RATE, MAX_RATE, y);
            model.setAttackRate (x, y);
        }
        // Decay rate curve control point
        else if (draggedComponent == draggablePoints.getUnchecked (3))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), decayRateArea, decayRatePoint);
            
            auto x = juce::jmap (
                decayRatePoint.getX(),
                decayRateArea.getX() + OFFSET,
                decayRateArea.getRight() - OFFSET,
                MIN_RATE,
                MAX_RATE
            );

            auto y = juce::jmap (
                decayRatePoint.getY(),
                decayRateArea.getY() + OFFSET,
                decayRateArea.getBottom() - OFFSET,
                MIN_RATE,
                MAX_RATE
            );

            x = juce::jlimit (MIN_RATE, MAX_RATE, x);
            y = juce::jlimit (MIN_RATE, MAX_RATE, y);
            model.setDecayRate (x, y);
        }
        // Release rate curve control point
        else if (draggedComponent == draggablePoints.getUnchecked (5))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), releaseRateArea, releaseRatePoint);
            
            auto x = juce::jmap (
                releaseRatePoint.getX(),
                releaseRateArea.getX() + OFFSET,
                releaseRateArea.getRight() - OFFSET,
                MIN_RATE,
                MAX_RATE
            );

            auto y = juce::jmap (
                releaseRatePoint.getY(),
                releaseRateArea.getY() + OFFSET,
                releaseRateArea.getBottom() - OFFSET,
                MIN_RATE,
                MAX_RATE
            );

            x = juce::jlimit (MIN_RATE, MAX_RATE, x);
            y = juce::jlimit (MIN_RATE, MAX_RATE, y);
            model.setReleaseRate (x, y);
        }

        update();
    }
}
  
//==============================================================================

float ADSRWidget::getAttackDuration() { return model.getAttackDuration(); }

float ADSRWidget::getDecayDuration() { return model.getDecayDuration(); }

float ADSRWidget::getSustainLevel() { return model.getSustainLevel(); }

float ADSRWidget::getReleaseDuration() { return model.getReleaseDuration(); }

const ValuePoint<float>& ADSRWidget::getAttackRate() { return model.getAttackRateValuePoint(); }

const ValuePoint<float>& ADSRWidget::getDecayRate() { return model.getDecayRateValuePoint(); }

const ValuePoint<float>& ADSRWidget::getReleaseRate() { return model.getReleaseRateValuePoint(); }

//==============================================================================

void ADSRWidget::recalculateBounds()
{
    bounds = getLocalBounds().reduced (PADDING);
    topEdgeY = bounds.getY();
    bottomEdgeY = bounds.getBottom();
    leftEdgeX = bounds.getX();
    rightEdgeX = bounds.getRight();
    width = bounds.getWidth();
    height = bounds.getHeight();
    equalSegmentWidth = static_cast<float> (width / 4.f);
}

void ADSRWidget::repositionPoints()
{
    // get the model reference values set during mouse drag events
    auto attackDuration = getAttackDuration();
    auto decayDuration = getDecayDuration();
    auto sustainLevel = getSustainLevel();
    auto releaseDuration = getReleaseDuration();
    
    // rates are 2D points inside bound areas
    auto attackRate = getAttackRate();
    auto decayRate = getDecayRate();
    auto releaseRate = getReleaseRate();

    // reposition the actual segment areas proportion to the model
    auto attackSegmentWidth = static_cast<float> (equalSegmentWidth * attackDuration);
    auto decaySegmentWidth = static_cast<float> (equalSegmentWidth * decayDuration);

    auto releaseSegmentWidth = static_cast<float> (equalSegmentWidth * releaseDuration);
    
    auto attackSegmentHeight = static_cast<float> (height);
    auto sustainSegmentHeight = static_cast<float> (height * sustainLevel);

    // segment point A
    attackDurationPoint.setXY (leftEdgeX + attackSegmentWidth, topEdgeY);
    
    // segment point D
    auto decayX = attackDurationPoint.getX() + decaySegmentWidth;
    auto decayY = (topEdgeY + height) - sustainSegmentHeight;
    
    auto decaySegmentHeight = static_cast<float> (decayY - topEdgeY);

    decayDurationPoint.setXY (decayX, decayY);
    
    // segment point S
    sustainLevelPoint.setXY (decayDurationPoint.getX() + equalSegmentWidth, decayDurationPoint.getY());
    
    // segment point R
    releaseDurationPoint.setXY (sustainLevelPoint.getX() + releaseSegmentWidth, bottomEdgeY);
    
    auto releaseSegmentHeight = static_cast<float> (releaseDurationPoint.getY() - sustainLevelPoint.getY());
    
    // reposition the rate curve points based on the resized segments areas
    attackRateArea.setBounds (leftEdgeX, topEdgeY, attackSegmentWidth, attackSegmentHeight);

    decayRateArea.setBounds (attackDurationPoint.getX(), attackDurationPoint.getY(), decaySegmentWidth, decaySegmentHeight);

    releaseRateArea.setBounds (sustainLevelPoint.getX(), sustainLevelPoint.getY(), releaseSegmentWidth, releaseSegmentHeight);

    // resize and reposition the bounds of the ADSR control point draggable areas
    // Horizontal draggable areas are limited to height of the control point
    
    auto attackDurationAreaWidth = OFFSET + attackSegmentWidth + OFFSET;
    attackDurationArea.setBounds (leftEdgeX - OFFSET, topEdgeY - OFFSET, attackDurationAreaWidth, POINT_SIZE);
    
    // Vertical draggable area limit width to control point diameter
    auto sustainLevelAreaHeight = OFFSET + sustainSegmentHeight + OFFSET;
    sustainLevelArea.setBounds (sustainLevelPoint.getX() - OFFSET, topEdgeY - OFFSET, POINT_SIZE, sustainLevelAreaHeight);
    
    if (sustainLevel < MAX_SUSTAIN_LEVEL )
    {
        auto decayDurationAreaWidth = OFFSET + decaySegmentWidth + OFFSET;
        decayDurationArea.setBounds (attackDurationPoint.getX() + OFFSET, decayDurationPoint.getY() - OFFSET, decayDurationAreaWidth, POINT_SIZE);
    }
    
    if (sustainLevel > MIN_SUSTAIN_LEVEL )
    {
        auto releaseDurationAreaWidth = OFFSET + releaseSegmentWidth + OFFSET;
        releaseDurationArea.setBounds (releaseDurationPoint.getX() - OFFSET, releaseDurationPoint.getY() - OFFSET, releaseDurationAreaWidth, POINT_SIZE);
    }
    
    //
    constrainPointToModel (attackRate, MIN_RATE, MAX_RATE, attackRateArea, attackRatePoint);
    constrainPointToModel (decayRate, MIN_RATE, MAX_RATE, decayRateArea, decayRatePoint);
    constrainPointToModel (releaseRate, MIN_RATE, MAX_RATE, releaseRateArea, releaseRatePoint);

    for ( int i = 0; i < 7; ++i )
    {
        auto* draggablePoint = draggablePoints.getUnchecked (i);
        if (i==0) // Attack
        {
            draggablePoint->setCentrePosition (attackDurationPoint.getX(), attackDurationPoint.getY());
        }
        else if (i==1) // Attack rate control point
        {
            draggablePoint->setCentrePosition (attackRatePoint.getX(), attackRatePoint.getY());
        }
        else if (i==2) // Decay
        {
            draggablePoint->setCentrePosition (decayDurationPoint.getX(), decayDurationPoint.getY());
        }
        else if (i==3) // Decay rate control point
        {
            draggablePoint->setCentrePosition (decayRatePoint.getX(), decayRatePoint.getY());
        }
        else if (i==4) // Sustain
        {
            draggablePoint->setCentrePosition (sustainLevelPoint.getX(), sustainLevelPoint.getY());
        }
        else if (i==5) // Release rate control point
        {
            draggablePoint->setCentrePosition (releaseRatePoint.getX(), releaseRatePoint.getY());
        }
        else if (i==6) // Release
        {
            draggablePoint->setCentrePosition (releaseDurationPoint.getX(), releaseDurationPoint.getY());
        }
    }
}

void ADSRWidget::drawGraph (juce::Graphics& g)
{
    graphPath.clear();

    // start path at bottom left corner
    graphPath.startNewSubPath (leftEdgeX, bottomEdgeY);

    // from bottom left point bezier curve through Attack control point to Decay start point
    graphPath.quadraticTo (attackRatePoint, attackDurationPoint);
    
    // from Decay start point bezier curve through Decay control point to Sustain start point
    graphPath.quadraticTo (decayRatePoint, decayDurationPoint);
    
    // from Sustain start point horizontal line to Release start point
    graphPath.lineTo (sustainLevelPoint);
    
    // from Release start point bezier curve through Release control point to Release end point
    graphPath.quadraticTo (releaseRatePoint, releaseDurationPoint);
    
    // close path at bottom left corner
    graphPath.closeSubPath();
    
    // Fill the Path
    g.setGradientFill (gradient);
    g.setOpacity (0.25f);
    g.fillPath (graphPath);

    g.setColour (pathColour);
    g.setOpacity (0.75f);
    g.strokePath (graphPath, juce::PathStrokeType (4.f));
    
    framePath.clear();

    // start frame path at bottom left corner minus stroke width
    framePath.startNewSubPath (leftEdgeX - OFFSET, bottomEdgeY);
    
    // from bottom left line to Attack start
    framePath.lineTo (rightEdgeX + OFFSET, bottomEdgeY);
    
    // close path at bottom left corner
    framePath.closeSubPath();
    
    g.setColour (backgroundColour);
    g.setOpacity (1.f);
    g.strokePath (framePath, juce::PathStrokeType (4.f) );
}

void ADSRWidget::update()
{
    recalculateBounds();
    repositionPoints();
    repaint();
}
                        
void ADSRWidget::constrainPointToArea (
    const juce::Point<float>& draggedPoint, // center of the control point component
    const juce::Rectangle<float>& area,
    juce::Point<float>& controlPoint
)
{
    auto x = juce::jlimit (area.getX() + OFFSET, area.getRight() - OFFSET, draggedPoint.getX());
    auto y = juce::jlimit (area.getY() + OFFSET, area.getBottom() - OFFSET, draggedPoint.getY());
    controlPoint.setXY (x, y);
}

void ADSRWidget::constrainPointToModel (
    const ValuePoint<float>& modelPoint, // center of the model point
    const float& modelLimitMin,
    const float& modelLimitMax,
    const juce::Rectangle<float>& area,
    juce::Point<float>& controlPoint
)
{
    auto x = juce::jmap (
         modelPoint.getX(),
         modelLimitMin,
         modelLimitMax,
         area.getX() + OFFSET,
         area.getRight() - OFFSET
    );

    auto y = juce::jmap (
         modelPoint.getY(),
         modelLimitMin,
         modelLimitMax,
         area.getY() + OFFSET,
         area.getBottom() - OFFSET
    );

    x = juce::jlimit (area.getX() + OFFSET, area.getRight() - OFFSET, x);
    y = juce::jlimit (area.getY() + OFFSET, area.getBottom() - OFFSET, y);
    controlPoint.setXY (x, y);
}
