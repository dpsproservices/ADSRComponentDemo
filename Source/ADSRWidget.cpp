//
//  ADSRWidget.cpp
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/13/22.
//

#include "ADSRWidget.h"

ADSRWidget::ADSRWidget() :
    bounds(),
    attackDurationValue (MAX_ADSR_DURATION),
    decayDurationValue (MAX_ADSR_DURATION),
    sustainLevelValue (DEFAULT_SUSTAIN_LEVEL),
    releaseDurationValue (MAX_ADSR_DURATION),
    backgroundColour (43, 43, 43),
    pathColour (88, 250, 255),
    gradientStartColour (62, 71, 71),
    gradientMidColour (68, 156, 158),
    gradientEndColour (83, 231, 235),
    pointColour (0, 58, 160),
    controlPointColour (130, 0, 61),
    selectedColour (127, 127, 127),
    gradient(),
    attackModelPoint (0.5f, 0.5f),
    decayModelPoint (0.5f, 0.5f),
    releaseModelPoint (0.5f, 0.5f),
    attackControlPoint(),
    decayControlPoint(),
    releaseControlPoint(),
    attackPoint(),
    decayPoint(),
    sustainPoint(),
    releasePoint(),
    attackArea(),
    decayArea(),
    releaseArea(),
    path(),
    framePath()
{
    draggedComponent = nullptr;

    for ( int i = 0; i < 7; ++i )
    {
        auto* draggablePoint = draggablePoints.add (new DraggablePoint());
        addAndMakeVisible (draggablePoint);
        
        draggablePoint->setFontColour (juce::Colours::white);
        draggablePoint->setFontSize (ADSR_FONT_SIZE);
        draggablePoint->setOuterFillColour (selectedColour);

        if (i==0) // Attack
        {
            draggablePoint->setSize (ADSR_POINT_SIZE, ADSR_POINT_SIZE);
            draggablePoint->setLabel ("A");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else if (i==2) // Decay
        {
            draggablePoint->setSize (ADSR_POINT_SIZE, ADSR_POINT_SIZE);
            draggablePoint->setLabel ("D");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else if (i==4) // Sustain
        {
            draggablePoint->setSize (ADSR_POINT_SIZE, ADSR_POINT_SIZE);
            draggablePoint->setLabel ("S");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else if (i==6) // Release
        {
            draggablePoint->setSize (ADSR_POINT_SIZE, ADSR_POINT_SIZE);
            draggablePoint->setLabel ("R");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else // rate curve control points in between points A, D, R
        {
            draggablePoint->setSize (CONTROL_POINT_SIZE, CONTROL_POINT_SIZE);
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
    
    constrainer.setMinimumOnscreenAmounts (0xffffff, 0xffffff, 0xffffff, 0xffffff);
    constrainer.setSizeLimits (bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
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
            auto leftLimitX = leftEdgeX - OFFSET;
            auto rightLimitX = attackPoint.getX() - OFFSET;
            auto attackY = attackPoint.getY() - OFFSET;
            auto attackX = draggedComponent->getBounds().getX();
            
            constrainHorizontal (leftLimitX, rightLimitX, attackY);
            attackDurationValue = getDuration (leftLimitX, rightLimitX, attackX);
        }
        // Decay
        else if (draggedComponent == draggablePoints.getUnchecked (2))
        {
            auto leftLimitX = attackPoint.getX() - OFFSET;
            auto rightLimitX = decayPoint.getX() - OFFSET;
            auto decayY = decayPoint.getY() - OFFSET;
            auto decayX = draggedComponent->getBounds().getX();

            constrainHorizontal (leftLimitX, rightLimitX, decayY);
            decayDurationValue = getDuration (leftLimitX, rightLimitX, decayX);
        }
        // Sustain
        else if (draggedComponent == draggablePoints.getUnchecked (4))
        {
            auto topLimitY = topEdgeY - OFFSET;
            auto bottomLimitY = bottomEdgeY - OFFSET;
            auto sustainX = sustainPoint.getX() - OFFSET;
            auto sustainY = draggedComponent->getBounds().getY();
            
            constrainVertical (topLimitY, bottomLimitY, sustainX);
            sustainLevelValue = getLevel (topLimitY, bottomLimitY, sustainY);
        }
        // Release
        else if (draggedComponent == draggablePoints.getUnchecked (6))
        {
            auto leftLimitX = sustainPoint.getX() - OFFSET;
            auto rightLimitX = releasePoint.getX() - OFFSET;
            auto releaseY = releasePoint.getY() - OFFSET;
            auto x = draggedComponent->getBounds().getX();
            
            constrainHorizontal (leftLimitX, rightLimitX, releaseY);
            releaseDurationValue = getDuration (leftLimitX, rightLimitX, x);
        }
        // Attack rate curve control point
        else if (draggedComponent == draggablePoints.getUnchecked (1))
        {
            constrainModel (draggedComponent->getBounds().getCentre().toFloat(), attackArea, attackModelPoint);
        }
        // Decay rate curve control point
        else if (draggedComponent == draggablePoints.getUnchecked (3))
        {
            constrainModel (draggedComponent->getBounds().getCentre().toFloat(), decayArea, decayModelPoint);
        }
        // Release rate curve control point
        else if (draggedComponent == draggablePoints.getUnchecked (5))
        {
            constrainModel (draggedComponent->getBounds().getCentre().toFloat(), releaseArea, releaseModelPoint);
        }

        update();
    }
}
  
//==============================================================================

float ADSRWidget::getAttackDuration() { return static_cast<float> (attackDurationValue.getValue()); }

float ADSRWidget::getDecayDuration() { return static_cast<float> (decayDurationValue.getValue()); }

float ADSRWidget::getSustainLevel() { return static_cast<float> (sustainLevelValue.getValue()); }

float ADSRWidget::getReleaseDuration() { return static_cast<float> (releaseDurationValue.getValue()); }

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
    auto attackDuration = getAttackDuration();
    auto decayDuration = getDecayDuration();
    auto sustainLevel = getSustainLevel();
    auto releaseDuration = getReleaseDuration();

    // reposition the segment points
    auto attackSegmentWidth = static_cast<float> (equalSegmentWidth * attackDuration);
    auto decaySegmentWidth = static_cast<float> (equalSegmentWidth * decayDuration);

    auto releaseSegmentWidth = static_cast<float> (equalSegmentWidth * releaseDuration);
    
    auto attackSegmentHeight = static_cast<float> (height);
    auto sustainSegmentHeight = static_cast<float> (height * sustainLevel);

    // segment point A
    attackPoint.setXY (leftEdgeX + attackSegmentWidth, topEdgeY);
    
    // segment point D
    auto decayX = attackPoint.getX() + decaySegmentWidth;
    auto decayY = (topEdgeY + height) - sustainSegmentHeight;
    
    auto decaySegmentHeight = static_cast<float> (decayY - topEdgeY);

    decayPoint.setXY (decayX, decayY);
    
    // segment point S
    sustainPoint.setXY (decayPoint.getX() + equalSegmentWidth, decayPoint.getY());
    
    // segment point R
    releasePoint.setXY (sustainPoint.getX() + releaseSegmentWidth, bottomEdgeY);
    
    auto releaseSegmentHeight = static_cast<float> (releasePoint.getY() - sustainPoint.getY());
    
    // reposition the bezier curve control points
    attackArea.setBounds (leftEdgeX, topEdgeY, attackSegmentWidth, attackSegmentHeight);
    
    constrainArea (attackModelPoint, attackArea, attackControlPoint);
    
    decayArea.setBounds (attackPoint.getX(), attackPoint.getY(), decaySegmentWidth, decaySegmentHeight);
    
    constrainArea (decayModelPoint, decayArea, decayControlPoint);

    releaseArea.setBounds (sustainPoint.getX(), sustainPoint.getY(), releaseSegmentWidth, releaseSegmentHeight);
    
    constrainArea (releaseModelPoint, releaseArea, releaseControlPoint);

    for ( int i = 0; i < 7; ++i )
    {
        auto* draggablePoint = draggablePoints.getUnchecked (i);
        if (i==0) // Attack
        {
            draggablePoint->setCentrePosition (attackPoint.getX(), attackPoint.getY());
        }
        else if (i==1) // Attack rate control point
        {
            draggablePoint->setCentrePosition (attackControlPoint.getX(), attackControlPoint.getY());
        }
        else if (i==2) // Decay
        {
            draggablePoint->setCentrePosition (decayPoint.getX(), decayPoint.getY());
        }
        else if (i==3) // Decay rate control point
        {
            draggablePoint->setCentrePosition (decayControlPoint.getX(), decayControlPoint.getY());
        }
        else if (i==4) // Sustain
        {
            draggablePoint->setCentrePosition (sustainPoint.getX(), sustainPoint.getY());
        }
        else if (i==5) // Release rate control point
        {
            draggablePoint->setCentrePosition (releaseControlPoint.getX(), releaseControlPoint.getY());
        }
        else if (i==6) // Release
        {
            draggablePoint->setCentrePosition (releasePoint.getX(), releasePoint.getY());
        }
    }
}

void ADSRWidget::drawGraph (juce::Graphics& g)
{
    path.clear();
    
    juce::Point<float> startPoint (leftEdgeX, bottomEdgeY);

    // start path at bottom left corner
    path.startNewSubPath (leftEdgeX, bottomEdgeY);

    // from bottom left point bezier curve through Attack control point to Decay start point
    path.quadraticTo (attackControlPoint, attackPoint);
    
    // from Decay start point bezier curve through Decay control point to Sustain start point
    path.quadraticTo (decayControlPoint, decayPoint);
    
    // from Sustain start point horizontal line to Release start point
    path.lineTo (sustainPoint);
    
    // from Release start point bezier curve through Release control point to Release end point
    path.quadraticTo (releaseControlPoint, releasePoint);
    
    // close path at bottom left corner
    path.closeSubPath();
    
    // Fill the Path
    g.setGradientFill (gradient);
    g.setOpacity (0.25f);
    g.fillPath (path);

    g.setColour (pathColour);
    g.setOpacity (0.75f);
    g.strokePath (path, juce::PathStrokeType (4.f));
    
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

// constrain DraggablePoint to horizontal bounds
void ADSRWidget::constrainHorizontal (const int& leftX, const int& rightX, const float& y)
{
    if (draggedComponent != nullptr)
    {
        constrainer.applyBoundsToComponent (
            *draggedComponent,
            draggedComponent->getBounds().withY (y)
        );
        
        if (draggedComponent->getBounds().getX() < leftX)
        {
            constrainer.applyBoundsToComponent (
                *draggedComponent,
                draggedComponent->getBounds().withX (leftX).withY (y)
            );
        }
        
        if (draggedComponent->getBounds().getX() > rightX)
        {
            constrainer.applyBoundsToComponent (
                *draggedComponent,
                draggedComponent->getBounds().withX (rightX).withY (y)
            );
        }
    }
}

void ADSRWidget::constrainVertical (const int& topY, const int& bottomY, const float& x)
{
    if (draggedComponent != nullptr)
    {
        constrainer.applyBoundsToComponent (
            *draggedComponent,
            draggedComponent->getBounds().withX (x)
        );
        
        if (draggedComponent->getBounds().getY() < topY)
        {
            constrainer.applyBoundsToComponent (
                *draggedComponent,
                draggedComponent->getBounds().withY (topY).withX (x)
            );
        }
        
        if (draggedComponent->getBounds().getY() > bottomY)
        {
            constrainer.applyBoundsToComponent (
                *draggedComponent,
                draggedComponent->getBounds().withY (bottomY).withX (x)
            );
        }
    }
}

void ADSRWidget::constrainArea (
    const juce::Point<float>& modelPoint,
    const juce::Rectangle<float>& area,
    juce::Point<float>& controlPoint
)
{
    auto leftX = area.getX();
    auto rightX = area.getRight();
    auto topY = area.getY();
    auto bottomY = area.getBottom();
    auto segmentWidth = area.getWidth();
    auto segmentHeight = area.getHeight();
    auto leftLimitX = leftX + (ADSR_RATE_XY_OFFSET * segmentWidth);
    auto rightLimitX = rightX - (ADSR_RATE_XY_OFFSET * segmentWidth);
    auto topLimitY = topY + (ADSR_RATE_XY_OFFSET * segmentHeight);
    auto bottomLimitY = bottomY - (ADSR_RATE_XY_OFFSET * segmentHeight);

    auto x = juce::jmap (
        modelPoint.getX(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        leftLimitX,
        rightLimitX
    );

    auto y = juce::jmap (
        modelPoint.getY(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        topLimitY,
        bottomLimitY
    );
    
    x = juce::jlimit (leftLimitX, rightLimitX, x);
    y = juce::jlimit (topLimitY, bottomLimitY, y);
    
    controlPoint.setXY (x, y);
}
                            
void ADSRWidget::constrainModel (
    const juce::Point<float>& draggedPoint,
    const juce::Rectangle<float>& area,
    juce::Point<float>& modelPoint
)
{
    auto x = juce::jmap (
        draggedPoint.getX(),
        area.getX(),
        area.getRight(),
        static_cast<float> (MIN_ADSR_RATE_XY),
        static_cast<float> (MAX_ADSR_RATE_XY)
    );

    auto y = juce::jmap (
        draggedPoint.getY(),
        area.getY(),
        area.getBottom(),
        static_cast<float> (MIN_ADSR_RATE_XY),
        static_cast<float> (MAX_ADSR_RATE_XY)
    );

    x = juce::jlimit (MIN_ADSR_RATE_XY, MAX_ADSR_RATE_XY, x);
    y = juce::jlimit (MIN_ADSR_RATE_XY, MAX_ADSR_RATE_XY, y);
    
    modelPoint.setXY (x, y);
}

float ADSRWidget::getDuration (const int& leftX, const int& rightX, const float& x)
{
    float duration = juce::jmap (
        static_cast<float> (x),
        static_cast<float> (leftX),
        static_cast<float> (rightX),
        static_cast<float> (MIN_ADSR_DURATION),
        static_cast<float> (MAX_ADSR_DURATION)
    );
    
    return juce::jlimit (MIN_ADSR_DURATION, MAX_ADSR_DURATION, duration);
}

float ADSRWidget::getLevel (const int& topY, const int& bottomY, const float& y)
{
    float level = juce::jmap (
        static_cast<float> (draggedComponent->getBounds().getY()),
        static_cast<float> (topY),
        static_cast<float> (bottomY),
        static_cast<float> (MAX_SUSTAIN_LEVEL),
        static_cast<float> (MIN_SUSTAIN_LEVEL)
    );

    return juce::jlimit (MIN_SUSTAIN_LEVEL, MAX_SUSTAIN_LEVEL, level);
}
