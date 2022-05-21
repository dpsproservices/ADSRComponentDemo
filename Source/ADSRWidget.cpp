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
    controlPointColour (0, 58, 160),
    selectedColour (127, 127, 127),
    gradient(),
    attackRatePoint (0.5f, 0.5f),
    attackControlPoint(),
    decayStartPoint(),
    decayRatePoint (0.5f, 0.5f),
    decayControlPoint(),
    sustainStartPoint(),
    releaseStartPoint(),
    releaseRatePoint (0.5f, 0.5f),
    releaseControlPoint(),
    releaseEndPoint(),
    attackRectangle(),
    decayRectangle(),
    sustainRectangle(),
    releaseRectangle(),
    attackDraggableBounds(),
    decayDraggableBounds(),
    sustainDraggableBounds(),
    releaseDraggableBounds(),
    path(),
    framePath()
{
    draggedComponent = nullptr;

    for ( int i = 0; i < 4; ++i )
    {
        auto* draggablePoint = draggablePoints.add (new DraggablePoint());
        addAndMakeVisible (draggablePoint);
        
        draggablePoint->setFontColour (juce::Colours::white);
        draggablePoint->setFontSize (ADSR_FONT_SIZE);
        draggablePoint->setInnerFillColour (pointColour);
        draggablePoint->setOuterFillColour (selectedColour);
        draggablePoint->setSize (ADSR_POINT_SIZE, ADSR_POINT_SIZE);

        if (i==0) // Attack
        {
            draggablePoint->setLabel ("A");
        }
        else if (i==1) // Decay
        {
            draggablePoint->setLabel ("D");
        }
        else if (i==2) // Sustain
        {
            draggablePoint->setLabel ("S");
        }
        else if (i==3) // Release
        {
            draggablePoint->setLabel ("R");
        }

        draggablePoint->addMouseListener (this, false);
    }
}
    
void ADSRWidget::paint (juce::Graphics &g)
{
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds());
    
    drawGraph (g);
    
    g.setColour (juce::Colours::red);
    g.drawRect (attackRectangle);
    
    g.setColour (juce::Colours::blue);
    g.drawRect (decayRectangle);
    
    g.setColour (juce::Colours::yellow);
    g.drawRect (sustainRectangle);
    
    g.setColour (juce::Colours::green);
    g.drawRect (releaseRectangle);
    
    g.setColour (juce::Colours::red);
    g.drawRect (attackDraggableBounds);
    
    g.setColour (juce::Colours::blue);
    g.drawRect (decayDraggableBounds);
    
    g.setColour (juce::Colours::yellow);
    g.drawRect (sustainDraggableBounds);
    
    g.setColour (juce::Colours::green);
    g.drawRect (releaseDraggableBounds);
}

void ADSRWidget::resized()
{
    recalculateBounds();
    repositionPoints();
    resizeSegments();

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
        
        if (draggedComponent == draggablePoints.getUnchecked (0))
        {
            // Attack
            DBG("attackDrag");
            
            auto leftLimitX = leftEdgeX - OFFSET;
            auto rightLimitX = decayStartPoint.getX() - OFFSET;
            auto attackY = decayStartPoint.getY() - OFFSET;
            
            constrainer.applyBoundsToComponent (
                *draggedComponent,
                draggedComponent->getBounds().withY (attackY)
            );
            
            if (draggedComponent->getBounds().getX() < leftLimitX)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withX (leftLimitX).withY (attackY)
                );
            }
            
            if (draggedComponent->getBounds().getX() > rightLimitX)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withX (rightLimitX).withY (attackY)
                );
            }
            
            attackDurationValue = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getX()),
                static_cast<float> (leftLimitX),
                static_cast<float> (rightLimitX),
                0.1f,
                1.f
            );
        }
        else if (draggedComponent == draggablePoints.getUnchecked (1))
        {
            // Decay
            DBG("decayDrag");
            auto leftLimitX = decayStartPoint.getX() - OFFSET;
            auto rightLimitX = sustainStartPoint.getX() - OFFSET;
            auto decayY = sustainStartPoint.getY() - OFFSET;

            constrainer.applyBoundsToComponent (
                *draggedComponent,
                draggedComponent->getBounds().withY (decayY)
            );
            
            if (draggedComponent->getBounds().getX() < leftLimitX)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withX (leftLimitX).withY (decayY)
                );
            }
            
            if (draggedComponent->getBounds().getX() > rightLimitX)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withX (rightLimitX).withY (decayY)
                );
            }
            
            decayDurationValue = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getX()),
                static_cast<float> (leftLimitX),
                static_cast<float> (rightLimitX),
                0.1f,
                1.f
            );
        }
        else if (draggedComponent == draggablePoints.getUnchecked (2))
        {
            // Sustain
            DBG("sustainDrag");
            auto topLimitY = topEdgeY - OFFSET;
            auto bottomLimitY = bottomEdgeY - OFFSET;
            auto sustainX = releaseStartPoint.getX() - OFFSET;
            
            constrainer.applyBoundsToComponent (
                *draggedComponent,
                draggedComponent->getBounds().withX (sustainX)
            );
            
            if (draggedComponent->getBounds().getY() < topLimitY)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withY (topLimitY).withX (sustainX)
                );
            }
            
            if (draggedComponent->getBounds().getY() > bottomLimitY)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withY (bottomLimitY).withX (sustainX)
                );
            }
            
            sustainLevelValue = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getY()),
                static_cast<float> (topLimitY),
                static_cast<float> (bottomLimitY),
                static_cast<float> (MAX_SUSTAIN_LEVEL),
                static_cast<float> (MIN_SUSTAIN_LEVEL)
            );
        }
        else if (draggedComponent == draggablePoints.getUnchecked (3))
        {
            // Release
            DBG("releaseDrag");
            auto leftLimitX = releaseStartPoint.getX() - OFFSET;
            auto rightLimitX = releaseEndPoint.getX() - OFFSET;
            auto releaseY = releaseEndPoint.getY() - OFFSET;
            
            constrainer.applyBoundsToComponent (
                *draggedComponent,
                draggedComponent->getBounds().withY (releaseY)
            );
            
            if (draggedComponent->getBounds().getX() < leftLimitX)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withX (leftLimitX).withY (releaseY)
                );
            }
            
            if (draggedComponent->getBounds().getX() > rightLimitX)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withX (rightLimitX).withY (releaseY)
                );
            }
            
            releaseDurationValue = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getX()),
                static_cast<float> (leftLimitX),
                static_cast<float> (rightLimitX),
                0.1f,
                1.f
            );
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
    
    DBG("topEdgeY: " + juce::String (topEdgeY));
    DBG("bottomEdgeY: " + juce::String (bottomEdgeY));
    DBG("leftEdgeX: " + juce::String (leftEdgeX));
    DBG("rightEdgeX: " + juce::String (rightEdgeX));
    DBG("width: " + juce::String (width));
    DBG("height: " + juce::String (height));
    DBG("equalSegmentWidth: " + juce::String (equalSegmentWidth));
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
    auto sustainSegmentHeight = static_cast<float> (height * sustainLevel);
    
    // segment point A
    decayStartPoint.setXY (leftEdgeX + attackSegmentWidth, topEdgeY);
    
    // segment point D
    auto decayX = decayStartPoint.getX() + decaySegmentWidth;
    auto decayY = (topEdgeY + height) - sustainSegmentHeight;

    sustainStartPoint.setXY (decayX, decayY);
    
    // segment point S
    releaseStartPoint.setXY (sustainStartPoint.getX() + equalSegmentWidth, sustainStartPoint.getY());
    
    // segment point R
    releaseEndPoint.setXY (releaseStartPoint.getX() + releaseSegmentWidth, bottomEdgeY);
    
    // reposition the bezier curve control points
    auto attachControlPointX = juce::jmap (attackRatePoint.getX(), 0.f, 1.f, static_cast<float> (leftEdgeX), decayStartPoint.getX());
    auto attackControlPointY = juce::jmap (attackRatePoint.getY(), 0.f, 1.f, static_cast<float> (topEdgeY), static_cast<float> (bottomEdgeY));
    attackControlPoint.setXY (attachControlPointX, attackControlPointY);
    
    auto decayControlPointX = juce::jmap (decayRatePoint.getX(), 0.f, 1.f, decayStartPoint.getX(), decayStartPoint.getX() + decaySegmentWidth);
    auto decayControlPointY = juce::jmap (decayRatePoint.getY(), 0.f, 1.f, decayStartPoint.getY(), sustainStartPoint.getY());
    decayControlPoint.setXY (decayControlPointX, decayControlPointY);
    
    auto releaseControlPointX = juce::jmap (releaseRatePoint.getX(), 0.f, 1.f, releaseStartPoint.getX(), releaseStartPoint.getX() + releaseSegmentWidth);
    auto releaseControlPointY = juce::jmap (releaseRatePoint.getY(), 0.f, 1.f, releaseStartPoint.getY(), static_cast<float> (bottomEdgeY));
    releaseControlPoint.setXY (releaseControlPointX, releaseControlPointY);
    
    for ( int i = 0; i < 4; ++i )
    {
        auto* draggablePoint = draggablePoints.getUnchecked (i);
        if (i==0) // Attack
        {
            draggablePoint->setCentrePosition (decayStartPoint.getX(), decayStartPoint.getY());
        }
        else if (i==1) // Decay
        {
            draggablePoint->setCentrePosition (sustainStartPoint.getX(), sustainStartPoint.getY());
        }
        else if (i==2) // Sustain
        {
            draggablePoint->setCentrePosition (releaseStartPoint.getX(), releaseStartPoint.getY());
        }
        else if (i==3) // Release
        {
            draggablePoint->setCentrePosition (releaseEndPoint.getX(), releaseEndPoint.getY());
        }
    }
}

void ADSRWidget::resizeSegments()
{
    attackRectangle.setX (leftEdgeX);
    attackRectangle.setY (topEdgeY);
    attackRectangle.setWidth (decayStartPoint.getX() - leftEdgeX);
    attackRectangle.setHeight (bottomEdgeY - decayStartPoint.getY());
    
    decayRectangle.setX (decayStartPoint.getX());
    decayRectangle.setY (topEdgeY);
    decayRectangle.setWidth (sustainStartPoint.getX() - decayStartPoint.getX());
    decayRectangle.setHeight (sustainStartPoint.getY() - topEdgeY);
    
    sustainRectangle.setX (sustainStartPoint.getX());
    sustainRectangle.setY (topEdgeY);
    sustainRectangle.setWidth (equalSegmentWidth);
    sustainRectangle.setHeight (releaseEndPoint.getY() - topEdgeY);
    
    releaseRectangle.setX (releaseStartPoint.getX());
    releaseRectangle.setY (releaseStartPoint.getY());
    releaseRectangle.setWidth (releaseEndPoint.getX() - releaseStartPoint.getX());
    releaseRectangle.setHeight (releaseEndPoint.getY() - releaseStartPoint.getY());

    attackDraggableBounds.setBounds (
        leftEdgeX - OFFSET,
        topEdgeY - OFFSET,
        OFFSET + equalSegmentWidth + OFFSET,
        ADSR_POINT_SIZE
    );
    
    decayDraggableBounds.setBounds (
        leftEdgeX + equalSegmentWidth - OFFSET,
        sustainStartPoint.getY() - OFFSET,
        OFFSET + equalSegmentWidth + OFFSET,
        ADSR_POINT_SIZE
    );
    
    sustainDraggableBounds.setBounds (
        releaseStartPoint.getX() - OFFSET,
        topEdgeY - OFFSET,
        ADSR_POINT_SIZE,
        OFFSET + height + OFFSET
    );
    
    releaseDraggableBounds.setBounds (
        releaseStartPoint.getX() - OFFSET,
        releaseEndPoint.getY() - OFFSET,
        OFFSET + equalSegmentWidth + OFFSET,
        ADSR_POINT_SIZE
    );
}

void ADSRWidget::drawGraph (juce::Graphics& g)
{
    path.clear();
    
    // start path at bottom left corner
    path.startNewSubPath (leftEdgeX, bottomEdgeY);

    // from bottom left point bezier curve through Attack control point to Decay start point
    path.quadraticTo (attackControlPoint, decayStartPoint);
    
    // from Decay start point bezier curve through Decay control point to Sustain start point
    path.quadraticTo (decayControlPoint, sustainStartPoint);
    
    // from Sustain start point line to Release start point
    path.lineTo (releaseStartPoint);
    
    // from Release start point bezier curve through Release control point to Release end point
    path.quadraticTo (releaseControlPoint, releaseEndPoint);
    
    // back from Release end point line to bottom left corner to close it
    path.lineTo (leftEdgeX, bottomEdgeY);
    
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
    framePath.startNewSubPath (leftEdgeX - 4, bottomEdgeY);
    
    // from bottom left line to Attack start
    framePath.lineTo (rightEdgeX + 4, bottomEdgeY);
    
    // close path at bottom left corner
    framePath.closeSubPath();
    
    g.setColour (backgroundColour);
    g.setOpacity (1.f);
    g.strokePath (framePath, juce::PathStrokeType(4.f) );
}

void ADSRWidget::update()
{
    recalculateBounds();
    repositionPoints();
    resizeSegments();
    repaint();
}
