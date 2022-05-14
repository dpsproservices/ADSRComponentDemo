//
//  ADSRWidget.cpp
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/13/22.
//

#include "ADSRWidget.h"

ADSRWidget::ADSRWidget() :
    attackDurationValue (0.1f),
    decayDurationValue (0.1f),
    sustainDurationValue (1.0f),
    releaseDurationValue (0.1f),
    attackCurveValue (0.f),
    decayCurveValue (0.f),
    releaseCurveValue (0.f),
    pointDiameter (24),
    pointFontSize (16),
    backgroundColour (43, 43, 43),
    pathColour (88, 250, 255),
    gradientStartColour (62, 71, 71),
    gradientMidColour (68, 156, 158), // teal
    gradientEndColour (83, 231, 235),
    pointColour (0, 58, 160),
    controlPointColour (0, 58, 160),
    gradient(),
    attackStartPoint(),
    attackControlPoint(),
    decayStartPoint(),
    decayControlPoint(),
    sustainStartPoint(),
    releaseStartPoint(),
    releaseControlPoint(),
    releaseEndPoint(),
    attackRectangle(),
    decayRectangle(),
    sustainRectangle(),
    releaseRectangle(),
    path(),
    framePath()
{
    
}
    
void ADSRWidget::paint (juce::Graphics &g)
{
    g.setColour (juce::Colours::green);
    g.drawRect (getLocalBounds());
    
    drawGraph(g);
    drawPoints(g);
}

void ADSRWidget::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    auto topEdgeY = bounds.getY();
    auto bottomEdgeY = bounds.getHeight();
    auto leftEdgeX = bounds.getX();
    auto rightEdgeX = bounds.getRight();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();
    
    auto segmentWidth = static_cast<float> (width / 4.f);
    
    gradient.clearColours();

    // resize the vertical line gradient to fill underneath the graph path
    gradient.point1 = juce::Point<float> (leftEdgeX, bottomEdgeY);
    gradient.point2 = juce::Point<float> (leftEdgeX, topEdgeY);
    
    gradient.addColour (0.f, gradientStartColour);
    gradient.addColour (0.15f, gradientMidColour);
    gradient.addColour (1.f, gradientEndColour);
    
    attackRectangle.setX (leftEdgeX);
    attackRectangle.setY (topEdgeY);
    attackRectangle.setWidth (segmentWidth);
    attackRectangle.setHeight (height);
    
    // reposition the Attack, Decay, Sustain, Release start, control, end points
    attackStartPoint.setXY (leftEdgeX, bottomEdgeY);
    
    auto attackControlPointX = static_cast<float> ((leftEdgeX + segmentWidth) / 2.f);
    auto attackControlPointY = static_cast<float> (height / 2.f);
    attackControlPoint.setXY (attackControlPointX , attackControlPointY);
    
    decayRectangle.setX (leftEdgeX + segmentWidth);
    decayRectangle.setY (topEdgeY);
    decayRectangle.setWidth (segmentWidth);
    decayRectangle.setHeight (static_cast<float> (height / 2.f));
    
    decayStartPoint.setXY (leftEdgeX + segmentWidth, topEdgeY);
    auto decayControlPointX = static_cast<float> (leftEdgeX + segmentWidth + (segmentWidth / 2.f));
    auto decayControlPointY = static_cast<float> (height / 4.f);
    decayControlPoint.setXY (decayControlPointX, decayControlPointY);

    sustainStartPoint.setXY (leftEdgeX + segmentWidth + segmentWidth, topEdgeY + static_cast<float> (height / 2.f));
    
    auto releaseX = leftEdgeX + segmentWidth + segmentWidth + segmentWidth;
    auto releaseY = topEdgeY + static_cast<float> (height / 2.f);
    releaseRectangle.setX (releaseX);
    releaseRectangle.setY (releaseY);
    releaseRectangle.setWidth (segmentWidth);
    releaseRectangle.setHeight (static_cast<float> (height / 2.f));

    releaseStartPoint.setXY (releaseX, releaseY);
    releaseControlPoint.setXY (releaseX + static_cast<float> (segmentWidth / 2.f), releaseY + static_cast<float> (height / 4.f));
    releaseEndPoint.setXY (rightEdgeX, bottomEdgeY);
}

//==============================================================================

void ADSRWidget::drawGraph (juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(20).toFloat();
    auto leftEdgeX = bounds.getX();
//    auto topEdgeY = bounds.getY();
    auto rightEdgeX = bounds.getRight();
    auto bottomEdgeY = bounds.getHeight();
    
    path.clear();
    
    // start path at bottom left corner
    path.startNewSubPath (leftEdgeX, bottomEdgeY);
    
    // from bottom left line to Attack start
    path.lineTo (attackStartPoint);

    // from Attack start point bezier curve through Attack control point to Decay start point
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
    g.strokePath (path, juce::PathStrokeType(4.f) );
    
    framePath.clear();

    // start frame path at bottom left corner minus stroke width
    framePath.startNewSubPath (leftEdgeX - 4, bottomEdgeY);
    
    // from bottom left line to Attack start
    framePath.lineTo (rightEdgeX, bottomEdgeY);
    
    // close path at bottom left corner
    framePath.closeSubPath();
    
    g.setColour (backgroundColour);
    g.setOpacity (1.f);
    g.strokePath (framePath, juce::PathStrokeType(4.f) );
    
}

void ADSRWidget::drawPoints (juce::Graphics& g)
{
    // draw Attack point at Decay start
    auto attackX = decayStartPoint.getX() - static_cast<float> (pointDiameter / 2.f);
    auto attackY = decayStartPoint.getY() - static_cast<float> (pointDiameter / 2.f);
    
    // draw Decay point at Sustain start
    auto decayX = sustainStartPoint.getX() - static_cast<float> (pointDiameter / 2.f);
    auto decayY = sustainStartPoint.getY() - static_cast<float> (pointDiameter / 2.f);
    
    // draw Sustain point at Release start
    auto sustainX = releaseStartPoint.getX() - static_cast<float> (pointDiameter / 2.f);
    auto sustainY = releaseStartPoint.getY() - static_cast<float> (pointDiameter / 2.f);
    
    // draw Release point at Release end
    auto releaseX = releaseEndPoint.getX() - static_cast<float> (pointDiameter / 2.f);
    auto releaseY = releaseEndPoint.getY() - static_cast<float> (pointDiameter / 2.f);
    
    g.setOpacity (0.75f);
    g.setColour (pointColour);
    
    g.fillEllipse (attackX, attackY, pointDiameter, pointDiameter);
    g.fillEllipse (decayX, decayY, pointDiameter, pointDiameter);
    g.fillEllipse (sustainX, sustainY, pointDiameter, pointDiameter);
    g.fillEllipse (releaseX, releaseY, pointDiameter, pointDiameter);
    
    g.setOpacity (1.f);
    g.setColour (juce::Colours::white);
    g.setFont (pointFontSize);
    
    // draw the labels
    g.drawText ("A", attackX, attackY, pointDiameter, pointDiameter, juce::Justification::centred);
    g.drawText ("D", decayX, decayY, pointDiameter, pointDiameter, juce::Justification::centred);
    g.drawText ("S", sustainX, sustainY, pointDiameter, pointDiameter, juce::Justification::centred);
    g.drawText ("R", releaseX, releaseY, pointDiameter, pointDiameter, juce::Justification::centred);
}

void ADSRWidget::drawControlPoint (juce::Graphics& g, const juce::Point<float>& controlPoint)
{
    g.setOpacity (0.75f);
    g.setColour (controlPointColour);
    
    // draw Attack point at Decay start
    auto controlPointX = controlPoint.getX();
    auto controlPointY = controlPoint.getY();
    
    g.fillEllipse (controlPointX, controlPointY, pointDiameter, pointDiameter);
}
