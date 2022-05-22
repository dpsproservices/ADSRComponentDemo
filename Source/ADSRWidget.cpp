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
        else // curve control points in between points A, D, R
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
        
        if (draggedComponent == draggablePoints.getUnchecked (0))
        {
            // Attack
            auto leftLimitX = leftEdgeX - OFFSET;
            auto rightLimitX = decayStartPoint.getX() - OFFSET;
            auto attackY = decayStartPoint.getY() - OFFSET;
            auto attackX = draggedComponent->getBounds().getX();
            
            constrainHorizontal (leftLimitX, rightLimitX, attackY);
            attackDurationValue = getDuration (leftLimitX, rightLimitX, attackX);
        }
        else if (draggedComponent == draggablePoints.getUnchecked (2))
        {
            // Decay
            auto leftLimitX = decayStartPoint.getX() - OFFSET;
            auto rightLimitX = sustainStartPoint.getX() - OFFSET;
            auto decayY = sustainStartPoint.getY() - OFFSET;
            auto decayX = draggedComponent->getBounds().getX();

            constrainHorizontal (leftLimitX, rightLimitX, decayY);
            decayDurationValue = getDuration (leftLimitX, rightLimitX, decayX);
        }
        else if (draggedComponent == draggablePoints.getUnchecked (4))
        {
            // Sustain
            auto topLimitY = topEdgeY - OFFSET;
            auto bottomLimitY = bottomEdgeY - OFFSET;
            auto sustainX = releaseStartPoint.getX() - OFFSET;
            auto sustainY = draggedComponent->getBounds().getY();
            
            constrainVertical (topLimitY, bottomLimitY, sustainX);
            sustainLevelValue = getLevel (topLimitY, bottomLimitY, sustainY);
        }
        else if (draggedComponent == draggablePoints.getUnchecked (6))
        {
            // Release
            auto leftLimitX = releaseStartPoint.getX() - OFFSET;
            auto rightLimitX = releaseEndPoint.getX() - OFFSET;
            auto releaseY = releaseEndPoint.getY() - OFFSET;
            auto x = draggedComponent->getBounds().getX();
            
            constrainHorizontal (leftLimitX, rightLimitX, releaseY);
            releaseDurationValue = getDuration (leftLimitX, rightLimitX, x);
        }
        else if (draggedComponent == draggablePoints.getUnchecked (1))
        {
            // Attack rate curve control point
            auto leftLimitX = leftEdgeX;
            auto rightLimitX = decayStartPoint.getX();
            auto topLimitY = topEdgeY;
            auto bottomLimitY = bottomEdgeY;
            auto x = draggedComponent->getBounds().getX();
            auto y = draggedComponent->getBounds().getY();

            constrainArea (leftLimitX, rightLimitX, topLimitY, bottomLimitY);
            
            auto rateX = juce::jmap (
                static_cast<float> (x),
                static_cast<float> (leftLimitX),
                static_cast<float> (rightLimitX),
                static_cast<float> (MIN_ADSR_RATE_XY),
                static_cast<float> (MAX_ADSR_RATE_XY)
            );

            auto rateY = juce::jmap (
                static_cast<float> (y),
                static_cast<float> (topLimitY),
                static_cast<float> (bottomLimitY),
                static_cast<float> (MIN_ADSR_RATE_XY),
                static_cast<float> (MAX_ADSR_RATE_XY)
            );
            
            rateX = juce::jlimit (MIN_ADSR_RATE_XY, MAX_ADSR_RATE_XY, rateX);
            rateY = juce::jlimit (MIN_ADSR_RATE_XY, MAX_ADSR_RATE_XY, rateY);
            
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
    //auto sustainSegmentWidth = static_cast<float> (equalSegmentWidth);
    auto releaseSegmentWidth = static_cast<float> (equalSegmentWidth * releaseDuration);
    
    auto attackSegmentHeight = static_cast<float> (height);
    auto sustainSegmentHeight = static_cast<float> (height * sustainLevel);

    // segment point A
    decayStartPoint.setXY (leftEdgeX + attackSegmentWidth, topEdgeY);
    
    // segment point D
    auto decayX = decayStartPoint.getX() + decaySegmentWidth;
    auto decayY = (topEdgeY + height) - sustainSegmentHeight;
    
    auto decaySegmentHeight = static_cast<float> (decayY - topEdgeY);

    sustainStartPoint.setXY (decayX, decayY);
    
    // segment point S
    releaseStartPoint.setXY (sustainStartPoint.getX() + equalSegmentWidth, sustainStartPoint.getY());
    
    // segment point R
    releaseEndPoint.setXY (releaseStartPoint.getX() + releaseSegmentWidth, bottomEdgeY);
    
    auto releaseSegmentHeight = static_cast<float> (releaseEndPoint.getY() - releaseStartPoint.getY());
    
    // reposition the bezier curve control points
    auto attachControlPointX = juce::jmap (
        attackRatePoint.getX(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        static_cast<float> (leftEdgeX) + (0.1f * attackSegmentWidth),
        decayStartPoint.getX() - (0.1f * attackSegmentWidth)
    );

    auto attackControlPointY = juce::jmap (
        attackRatePoint.getY(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        static_cast<float> (topEdgeY) + (0.1f * attackSegmentHeight),
        static_cast<float> (bottomEdgeY) - (0.1f * attackSegmentHeight)
    );
    
    attachControlPointX = juce::jlimit (static_cast<float> (leftEdgeX) + (0.1f * attackSegmentWidth), decayStartPoint.getX() - (0.1f * attackSegmentWidth), attachControlPointX);

    attackControlPointY = juce::jlimit (static_cast<float> (topEdgeY) + (0.1f * attackSegmentHeight), static_cast<float> (bottomEdgeY) - (0.1f * attackSegmentHeight), attackControlPointY);
    
    attackControlPoint.setXY (attachControlPointX, attackControlPointY);
    
    auto decayControlPointX = juce::jmap (
        decayRatePoint.getX(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        decayStartPoint.getX() + (0.1f * decaySegmentWidth),
        sustainStartPoint.getX() - (0.1f * decaySegmentWidth)
    );

    auto decayControlPointY = juce::jmap (
        decayRatePoint.getY(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        decayStartPoint.getY() + (0.1f * decaySegmentHeight),
        sustainStartPoint.getY() - + (0.1f * decaySegmentHeight)
    );
    
    decayControlPointX = juce::jlimit (decayStartPoint.getX() + (0.1f * decaySegmentWidth), sustainStartPoint.getX() - (0.1f * decaySegmentWidth), decayControlPointX);

    decayControlPointY = juce::jlimit (decayStartPoint.getY() + (0.1f * decaySegmentHeight), sustainStartPoint.getY() - + (0.1f * decaySegmentHeight), decayControlPointY);

    decayControlPoint.setXY (decayControlPointX, decayControlPointY);
    
    auto releaseControlPointX = juce::jmap (
        releaseRatePoint.getX(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        releaseStartPoint.getX() + (0.1f * releaseSegmentWidth),
        releaseEndPoint.getX() - (0.1f * releaseSegmentWidth)
    );

    auto releaseControlPointY = juce::jmap (
        releaseRatePoint.getY(),
        MIN_ADSR_RATE_XY,
        MAX_ADSR_RATE_XY,
        releaseStartPoint.getY() + (0.1f * releaseSegmentHeight),
        static_cast<float> (bottomEdgeY) - (0.1f * releaseSegmentHeight)
    );
    
    releaseControlPointX = juce::jlimit (releaseStartPoint.getX() + (0.1f * releaseSegmentWidth), releaseEndPoint.getX() - (0.1f * releaseSegmentWidth), releaseControlPointX);

    releaseControlPointY = juce::jlimit (releaseStartPoint.getY() + (0.1f * releaseSegmentHeight), static_cast<float> (bottomEdgeY) - (0.1f * releaseSegmentHeight), releaseControlPointY);

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
    
    // from Sustain start point horizontal line to Release start point
    path.lineTo (releaseStartPoint);
    
    // from Release start point bezier curve through Release control point to Release end point
    path.quadraticTo (releaseControlPoint, releaseEndPoint);
    
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

void ADSRWidget::constrainArea (const int& leftX, const int& rightX, const int& topY, const int& bottomY)
{
    DBG("leftX " + juce::String (leftX));
    DBG("rightX " + juce::String (rightX));
    DBG("topY " + juce::String (topY));
    DBG("bottomY " + juce::String (bottomY));
    
    if (draggedComponent != nullptr)
    {
        auto bounds = draggedComponent->getBounds();
        auto leftEdgeX = bounds.getX();
        auto rightEdgeX = bounds.getRight();
        auto topEdgeY = bounds.getY();
        auto bottomEdgeY = bounds.getBottom();
//        auto width = bounds.getWidth();
//        auto height = bounds.getHeight();
        
        auto limitX = juce::jlimit (leftX, rightX - 30, leftEdgeX);
        auto limitY = juce::jlimit (topY, bottomY - 30, topEdgeY);
        
//        constrainer.applyBoundsToComponent (
//           *draggedComponent,
//           bounds.withX (limitX)
//        );
        
//        if (leftEdgeX < leftX)
//        {
//            constrainer.applyBoundsToComponent (
//                *draggedComponent,
//                bounds.withX (leftX)
//            );
//        }
//
//        if (rightEdgeX > rightX)
//        {
//            constrainer.applyBoundsToComponent (
//                *draggedComponent,
//                bounds.withRightX (rightX)
//            );
//        }
//
//        if (topEdgeY < topY)
//        {
//            constrainer.applyBoundsToComponent (
//                *draggedComponent,
//                bounds.withY (topY)
//            );
//        }
//
//        if (bottomEdgeY > bottomY)
//        {
//            constrainer.applyBoundsToComponent (
//                *draggedComponent,
//                bounds.withBottomY (bottomY)
//            );
//        }
    }
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
