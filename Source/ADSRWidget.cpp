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
                attackDurationArea.getX(),
                attackDurationArea.getRight(),
                MIN_DURATION,
                MAX_DURATION
            );

            model.setAttackDuration (duration);
        }
        // Decay
        else if (draggedComponent == draggablePoints.getUnchecked (2))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), decayDurationArea, decayDurationPoint);
            
            auto duration = juce::jmap (
                decayDurationPoint.getX(),
                decayDurationArea.getX(),
                decayDurationArea.getRight(),
                MIN_DURATION,
                MAX_DURATION
            );

            model.setDecayDuration (duration);
        }
        // Sustain
        else if (draggedComponent == draggablePoints.getUnchecked (4))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), sustainLevelArea, sustainLevelPoint);
            
            auto sustain = juce::jmap (
                sustainLevelPoint.getY(),
                sustainLevelArea.getY(),
                sustainLevelArea.getBottom(),
                MAX_SUSTAIN_LEVEL,
                MIN_SUSTAIN_LEVEL
            );
            
            model.setSustainLevel (sustain);
        }
        // Release
        else if (draggedComponent == draggablePoints.getUnchecked (6))
        {
            constrainPointToArea (draggedComponent->getBounds().getCentre().toFloat(), releaseDurationArea, releaseDurationPoint);
            
            auto duration = juce::jmap (
                releaseDurationPoint.getX(),
                releaseDurationArea.getX(),
                releaseDurationArea.getRight(),
                MIN_DURATION,
                MAX_DURATION
            );

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
                attackRateArea.getBottom() - OFFSET,
                attackRateArea.getY() + OFFSET,
                MIN_RATE,
                MAX_RATE
            );

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
                decayRateArea.getBottom() - OFFSET,
                decayRateArea.getY() + OFFSET,
                MIN_RATE,
                MAX_RATE
            );

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
                releaseRateArea.getBottom() - OFFSET,
                releaseRateArea.getY() + OFFSET,
                MIN_RATE,
                MAX_RATE
            );

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

void ADSRWidget::repositionAttack()
{
    auto attackDuration = getAttackDuration();
    auto attackRate = getAttackRate();
    
    // reposition the actual segment areas proportion to the model
    auto attackSegmentWidth = static_cast<float> (equalSegmentWidth * attackDuration);
    attackSegmentWidth = juce::jmax (static_cast<float> (POINT_SIZE), attackSegmentWidth);

    auto attackSegmentHeight = static_cast<float> (height);
    
    // segment point A
    attackDurationPoint.setXY (leftEdgeX + attackSegmentWidth, topEdgeY);
    
    attackRateArea.setBounds (leftEdgeX + OFFSET, topEdgeY + OFFSET, attackSegmentWidth - POINT_SIZE, attackSegmentHeight - POINT_SIZE);
    
    attackDurationArea.setBounds (leftEdgeX + OFFSET, topEdgeY - OFFSET, equalSegmentWidth, POINT_SIZE);
    
    constrainPointToModel (attackRate, MIN_RATE, MAX_RATE, attackRateArea, attackRatePoint);
}

void ADSRWidget::repositionDecay()
{
    auto sustainLevel = getSustainLevel();

    if ( sustainLevel == MAX_SUSTAIN_LEVEL )
    {
        // segment point D
        auto decayX = attackDurationPoint.getX();
        auto decayY = topEdgeY;
        
        decayDurationPoint.setXY (decayX, decayY);

        decayRateArea.setBounds (decayX, decayY, 1, 1);
        
        // Horizontal draggable areas are limited to height of the control point
        decayDurationArea.setBounds (decayX, decayY, 1, 1);
        
        decayRatePoint.setXY (MIN_RATE, MIN_RATE);
    }
    else if ( sustainLevel >= MIN_SUSTAIN_LEVEL && sustainLevel < MAX_SUSTAIN_LEVEL)
    {
        auto sustainSegmentHeight = static_cast<float> (height * sustainLevel);
        auto decayDuration = getDecayDuration();
        auto decayRate = getDecayRate();
        
        auto decaySegmentWidth = static_cast<float> (equalSegmentWidth * decayDuration);
        decaySegmentWidth = juce::jmax (static_cast<float> (POINT_SIZE), decaySegmentWidth);
        
        // segment point D
        auto decayX = attackDurationPoint.getX() + decaySegmentWidth;
        auto decayY = (topEdgeY + height) - sustainSegmentHeight;
        auto decaySegmentHeight = static_cast<float> (decayY - topEdgeY);
        
        decayDurationPoint.setXY (decayX, decayY);
        
        // 2D area for the Decay Rate curve
        decayRateArea.setBounds (attackDurationPoint.getX() + OFFSET, attackDurationPoint.getY() + OFFSET, decaySegmentWidth - POINT_SIZE, decaySegmentHeight - POINT_SIZE);
        
        // Horizontal draggable areas are limited to height of the control point
        decayDurationArea.setBounds (attackDurationPoint.getX() + OFFSET, decayDurationPoint.getY() - OFFSET, equalSegmentWidth, POINT_SIZE);
        
        constrainPointToModel (decayRate, MIN_RATE, MAX_RATE, decayRateArea, decayRatePoint);
    }
}

void ADSRWidget::repositionSustain()
{
    auto sustainLevel = getSustainLevel();

    if (sustainLevel == MAX_SUSTAIN_LEVEL)
    {
        // segment point S inline with segment point A
        // point D not shown
        sustainLevelPoint.setXY (attackDurationPoint.getX() + equalSegmentWidth, attackDurationPoint.getY());
    }
    else if ( sustainLevel >= MIN_SUSTAIN_LEVEL && sustainLevel < MAX_SUSTAIN_LEVEL)
    {
        // segment point S inline with segment point D
        sustainLevelPoint.setXY (decayDurationPoint.getX() + equalSegmentWidth, decayDurationPoint.getY());
    }

    // Vertical draggable area limit width to control point diameter
    sustainLevelArea.setBounds (sustainLevelPoint.getX() - OFFSET, topEdgeY - OFFSET, POINT_SIZE,  OFFSET + height + OFFSET);
}

void ADSRWidget::repositionRelease()
{
    auto sustainLevel = getSustainLevel();
    
    // do not show the Release segment if the Sustain level is at its minimum
    if ( sustainLevel == MIN_SUSTAIN_LEVEL )
    {
        releaseDurationPoint.setXY (sustainLevelPoint.getX(), bottomEdgeY);
        
        releaseDurationArea.setBounds (releaseDurationPoint.getX(), releaseDurationPoint.getY(), 1, 1);
        
        releaseRateArea.setBounds (sustainLevelPoint.getX(), sustainLevelPoint.getY(), 1, 1);
        
        releaseRatePoint.setXY (MIN_RATE, MAX_RATE);
    }
    else if ( sustainLevel > MIN_SUSTAIN_LEVEL && sustainLevel <= MAX_SUSTAIN_LEVEL )
    {
        auto releaseDuration = getReleaseDuration();
        auto releaseRate = getReleaseRate();
        
        auto releaseSegmentWidth = static_cast<float> (equalSegmentWidth * releaseDuration);
        releaseSegmentWidth = juce::jmax (static_cast<float> (POINT_SIZE), releaseSegmentWidth);
        
        // segment point R
        releaseDurationPoint.setXY (sustainLevelPoint.getX() + releaseSegmentWidth, bottomEdgeY);
        
        auto releaseSegmentHeight = static_cast<float> (releaseDurationPoint.getY() - sustainLevelPoint.getY());
        
        // Horizontal draggable areas are limited to height of the control point
        releaseDurationArea.setBounds (sustainLevelPoint.getX() + OFFSET, releaseDurationPoint.getY() - OFFSET, equalSegmentWidth, POINT_SIZE);
        
        releaseRateArea.setBounds (sustainLevelPoint.getX() + OFFSET, sustainLevelPoint.getY() + OFFSET, releaseSegmentWidth - POINT_SIZE, releaseSegmentHeight - POINT_SIZE);
        
        constrainPointToModel (releaseRate, MIN_RATE, MAX_RATE, releaseRateArea, releaseRatePoint);
    }
}

void ADSRWidget::repositionPoints()
{
    // get the model reference values set during mouse drag events
    // reposition the rate curve points based on the resized segments areas

    repositionAttack();
    repositionDecay();
    repositionSustain();
    repositionRelease();

    // reposition the centers of the draggable points based on the control points
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
    
    auto sustainLevel = getSustainLevel();
    
    if ( sustainLevel >= MIN_SUSTAIN_LEVEL && sustainLevel <= MAX_SUSTAIN_LEVEL )
    {
        // show all 4 segments Attack, Decay, Sustain, Release
        
        // from bottom left point bezier curve through Attack control point to Decay start point
        graphPath.quadraticTo (attackRatePoint, attackDurationPoint);
        
        // from Decay start point bezier curve through Decay control point to Sustain start point
        graphPath.quadraticTo (decayRatePoint, decayDurationPoint);
        
        // from Sustain start point horizontal line to Release start point
        graphPath.lineTo (sustainLevelPoint);
        
        // from Release start point bezier curve through Release control point to Release end point
        graphPath.quadraticTo (releaseRatePoint, releaseDurationPoint);
    }
    else if ( sustainLevel == MIN_SUSTAIN_LEVEL )
    {
        // dont show the Sustain or Release segments only Attack and Decay
        
        // from bottom left point bezier curve through Attack control point to Decay start point
        graphPath.quadraticTo (attackRatePoint, attackDurationPoint);
        
        // from Decay start point bezier curve through Decay control point to Sustain start point
        graphPath.quadraticTo (decayRatePoint, decayDurationPoint);
    }
    else if ( sustainLevel == MAX_SUSTAIN_LEVEL )
    {
        // dont show the Decay segment only Attack, Sustain, Release
        
        // from bottom left point bezier curve through Attack control point to Decay start point
        graphPath.quadraticTo (attackRatePoint, attackDurationPoint);
        
        // from Sustain start point horizontal line to Release start point
        graphPath.lineTo (sustainLevelPoint);
        
        // from Release start point bezier curve through Release control point to Release end point
        graphPath.quadraticTo (releaseRatePoint, releaseDurationPoint);
    }

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
    
    g.setColour (juce::Colours::green);

    if ( sustainLevel >= MIN_SUSTAIN_LEVEL && sustainLevel <= MAX_SUSTAIN_LEVEL )
    {
        // show all 4 segments Attack, Decay, Sustain, Release
        
        g.drawRect (attackRateArea);
        g.drawRect (attackDurationArea);
        g.drawRect (decayDurationArea);
        g.drawRect (decayRateArea);
        g.drawRect (sustainLevelArea);
        g.drawRect (releaseDurationArea);
        g.drawRect (releaseRateArea);
    }
    else if ( sustainLevel == MIN_SUSTAIN_LEVEL )
    {
        // dont show the Sustain or Release segments only Attack and Decay
        
        g.drawRect (attackRateArea);
        g.drawRect (attackDurationArea);
        g.drawRect (decayDurationArea);
        g.drawRect (decayRateArea);
    }
    else if ( sustainLevel == MAX_SUSTAIN_LEVEL )
    {
        // dont show the Decay segment only Attack, Sustain, Release
        
        g.drawRect (attackRateArea);
        g.drawRect (attackDurationArea);
        g.drawRect (sustainLevelArea);
        g.drawRect (releaseDurationArea);
        g.drawRect (releaseRateArea);
    }
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
    auto x = juce::jlimit (area.getX(), area.getRight(), draggedPoint.getX());
    auto y = juce::jlimit (area.getY(), area.getBottom(), draggedPoint.getY());
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
    auto minX = area.getX();
    auto maxX = area.getRight();
    auto minY = area.getY();
    auto maxY = area.getBottom();

    auto x = juce::jmap (
         modelPoint.getX(),
         modelLimitMin,
         modelLimitMax,
         minX,
         maxX
    );

    auto y = juce::jmap (
         modelPoint.getY(),
         modelLimitMin,
         modelLimitMax,
         maxY,
         minY
    );
    
    x = juce::jlimit (minX, maxX, x);
    y = juce::jlimit (minY, maxY, y);
    controlPoint.setXY (x, y);
}
