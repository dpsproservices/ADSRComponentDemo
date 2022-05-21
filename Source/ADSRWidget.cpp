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
        draggablePoint->setSize (ADSR_POINT_SIZE, ADSR_POINT_SIZE);

        if (i==0) // Attack
        {
            draggablePoint->setLabel ("A");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else if (i==2) // Decay
        {
            draggablePoint->setLabel ("D");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else if (i==4) // Sustain
        {
            draggablePoint->setLabel ("S");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else if (i==6) // Release
        {
            draggablePoint->setLabel ("R");
            draggablePoint->setInnerFillColour (pointColour);
            draggablePoint->setVisible (true);
        }
        else // curve control points in between points A, D, R
        {
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
        
        if (draggedComponent == draggablePoints.getUnchecked (0))
        {
            // Attack
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
            
            auto attack = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getX()),
                static_cast<float> (leftLimitX),
                static_cast<float> (rightLimitX),
                static_cast<float> (MIN_ADSR_DURATION),
                static_cast<float> (MAX_ADSR_DURATION)
            );
            
            attack = juce::jmin (MAX_ADSR_DURATION, attack);
            attack = juce::jmax (MIN_ADSR_DURATION, attack);
            attackDurationValue = attack;
        }
        else if (draggedComponent == draggablePoints.getUnchecked (2))
        {
            // Decay
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
            
            auto decay = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getX()),
                static_cast<float> (leftLimitX),
                static_cast<float> (rightLimitX),
                static_cast<float> (MIN_ADSR_DURATION),
                static_cast<float> (MAX_ADSR_DURATION)
            );

            decay = juce::jmin (MAX_ADSR_DURATION, decay);
            decay = juce::jmax (MIN_ADSR_DURATION, decay);
            decayDurationValue = decay;
        }
        else if (draggedComponent == draggablePoints.getUnchecked (4))
        {
            // Sustain
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
            
            auto sustain = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getY()),
                static_cast<float> (topLimitY),
                static_cast<float> (bottomLimitY),
                static_cast<float> (MAX_SUSTAIN_LEVEL),
                static_cast<float> (MIN_SUSTAIN_LEVEL)
            );
            
            sustain = juce::jmin (MAX_SUSTAIN_LEVEL, sustain);
            sustain = juce::jmax (MIN_SUSTAIN_LEVEL, sustain);
            sustainLevelValue = sustain;
        }
        else if (draggedComponent == draggablePoints.getUnchecked (6))
        {
            // Release
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
            
            auto release = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getX()),
                static_cast<float> (leftLimitX),
                static_cast<float> (rightLimitX),
                static_cast<float> (MIN_ADSR_DURATION),
                static_cast<float> (MAX_ADSR_DURATION)
            );
            
            release = juce::jmin (MAX_ADSR_DURATION, release);
            release = juce::jmax (MIN_ADSR_DURATION, release);
            releaseDurationValue = release;
        }
        else if (draggedComponent == draggablePoints.getUnchecked (1))
        {
            // Attack rate curve control point
            auto leftLimitX = leftEdgeX;
            auto rightLimitX = decayStartPoint.getX() - ADSR_POINT_SIZE;
            auto topLimitY = topEdgeY;
            auto bottomLimitY = bottomEdgeY - ADSR_POINT_SIZE;
            
            DBG("attackRateDrag leftLimitX " + juce::String(leftLimitX));
            DBG("attackRateDrag rightLimitX " + juce::String(rightLimitX));
            DBG("attackRateDrag topLimitY " + juce::String(topLimitY));
            DBG("attackRateDrag bottomLimitY " + juce::String(bottomLimitY));
                        
            if (draggedComponent->getBounds().getX() < leftLimitX)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withX (leftLimitX)
                );
            }
            
            if (draggedComponent->getBounds().getX() > rightLimitX)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withX (rightLimitX)
                );
            }
            
            if (draggedComponent->getBounds().getY() < topLimitY)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withY (topLimitY)
                );
            }
            
            if (draggedComponent->getBounds().getY() > bottomLimitY)
            {
                constrainer.applyBoundsToComponent (
                    *draggedComponent,
                    draggedComponent->getBounds().withY (bottomLimitY)
                );
            }
            
            auto rateX = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getX()),
                static_cast<float> (leftLimitX),
                static_cast<float> (rightLimitX),
                static_cast<float> (MIN_ADSR_RATE_XY),
                static_cast<float> (MAX_ADSR_RATE_XY)
            );
            
            rateX = juce::jmin (MAX_ADSR_RATE_XY, rateX);
            rateX = juce::jmax (MIN_ADSR_RATE_XY, rateX);
            attackDurationValue = rateX;

            auto rateY = juce::jmap (
                static_cast<float> (draggedComponent->getBounds().getY()),
                static_cast<float> (topLimitY),
                static_cast<float> (bottomLimitY),
                static_cast<float> (MIN_ADSR_RATE_XY),
                static_cast<float> (MAX_ADSR_RATE_XY)
            );
            
            rateX = juce::jmin (MIN_ADSR_RATE_XY, rateY);
            rateY = juce::jmax (MIN_ADSR_RATE_XY, rateY);
            
            attackRatePoint.setXY (rateX, rateY);
            
            DBG("attackRatePoint: " + attackRatePoint.toString() );
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
    auto sustainSegmentHeight = static_cast<float> (height * sustainLevel);
    auto releaseSegmentWidth = static_cast<float> (equalSegmentWidth * releaseDuration);
    
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
    auto attachControlPointX = juce::jmap (
        attackRatePoint.getX(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        static_cast<float> (leftEdgeX),
        decayStartPoint.getX()
    );

    auto attackControlPointY = juce::jmap (
        attackRatePoint.getY(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        static_cast<float> (topEdgeY),
        static_cast<float> (bottomEdgeY)
    );
    
    attackControlPoint.setXY (attachControlPointX, attackControlPointY);
    
    auto decayControlPointX = juce::jmap (
        decayRatePoint.getX(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        decayStartPoint.getX(),
        decayStartPoint.getX() + decaySegmentWidth
    );

    auto decayControlPointY = juce::jmap (
        decayRatePoint.getY(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        decayStartPoint.getY(),
        sustainStartPoint.getY()
    );

    decayControlPoint.setXY (decayControlPointX, decayControlPointY);
    
    auto releaseControlPointX = juce::jmap (
        releaseRatePoint.getX(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        releaseStartPoint.getX(),
        releaseStartPoint.getX() + releaseSegmentWidth
    );

    auto releaseControlPointY = juce::jmap (
        releaseRatePoint.getY(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        releaseStartPoint.getY(),
        static_cast<float> (bottomEdgeY)
    );

    releaseControlPoint.setXY (releaseControlPointX, releaseControlPointY);
    
    for ( int i = 0; i < 7; ++i )
    {
        auto* draggablePoint = draggablePoints.getUnchecked (i);
        if (i==0) // Attack
        {
            draggablePoint->setCentrePosition (decayStartPoint.getX(), decayStartPoint.getY());
        }
        else if (i==1) // Attack rate control point
        {
            draggablePoint->setCentrePosition (attackControlPoint.getX(), attackControlPoint.getY());
        }
        else if (i==2) // Decay
        {
            draggablePoint->setCentrePosition (sustainStartPoint.getX(), sustainStartPoint.getY());
        }
        else if (i==3) // Decay rate control point
        {
            draggablePoint->setCentrePosition (decayControlPoint.getX(), decayControlPoint.getY());
        }
        else if (i==4) // Sustain
        {
            draggablePoint->setCentrePosition (releaseStartPoint.getX(), releaseStartPoint.getY());
        }
        else if (i==5) // Release rate control point
        {
            draggablePoint->setCentrePosition (releaseControlPoint.getX(), releaseControlPoint.getY());
        }
        else if (i==6) // Release
        {
            draggablePoint->setCentrePosition (releaseEndPoint.getX(), releaseEndPoint.getY());
        }
    }
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
    repaint();
}
