//
//  ADSRWidget.cpp
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/13/22.
//

#include "ADSRWidget.h"
#include "CustomLookAndFeel.h"

ADSRWidget::ADSRWidget() :
    bounds(),
    attackDurationValue (MAX_ADSR_DURATION),
    decayDurationValue (MAX_ADSR_DURATION),
    sustainLevelValue (DEFAULT_SUSTAIN_LEVEL),
    releaseDurationValue (MAX_ADSR_DURATION),
    pointDiameter (30),
    pointFontSize (16),
    backgroundColour (43, 43, 43),
    pathColour (88, 250, 255),
    gradientStartColour (62, 71, 71),
    gradientMidColour (68, 156, 158),
    gradientEndColour (83, 231, 235),
    pointColour (0, 58, 160),
    controlPointColour (0, 58, 160),
    gradient(),
    attackRatePoint (0.5f,0.5f),
    attackControlPoint(),
    decayStartPoint(),
    decayRatePoint (0.5f,0.5f),
    decayControlPoint(),
    sustainStartPoint(),
    releaseStartPoint(),
    releaseRatePoint (0.5f,0.5f),
    releaseControlPoint(),
    releaseEndPoint(),
    attackRectangle(),
    decayRectangle(),
    sustainRectangle(),
    releaseRectangle(),
    path(),
    framePath(),
    attackDurationSlider(),
    decayDurationSlider(),
    sustainLevelSlider(),
    releaseDurationSlider()
{
    attackDurationSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    attackDurationSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    attackDurationSlider.setRange (MIN_ADSR_DURATION, MAX_ADSR_DURATION);
    attackDurationSlider.getValueObject().referTo (attackDurationValue);
    attackDurationSlider.setLookAndFeel (&customLookAndFeel);
    attackDurationSlider.onValueChange = [this] ()
    {
//        auto attackDuration = getAttackDuration();
//        auto decayDuration = getDecayDuration();
//        auto sustainLevel = getSustainLevel();
//        auto releaseDuration = getReleaseDuration();
//
//        DBG("attackDuration: " + juce::String(attackDuration));
//        DBG("decayDuration: " + juce::String(decayDuration));
//        DBG("sustainLevel: " + juce::String(sustainLevel));
//        DBG("releaseDuration: " + juce::String(releaseDuration));

        //attackDurationValue = attackDurationSlider.getValue();
        repositionPoints();
        resizeSegments();
        repaint();
    };
    
    decayDurationSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    decayDurationSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    decayDurationSlider.setRange (MIN_ADSR_DURATION, MAX_ADSR_DURATION);
    decayDurationSlider.getValueObject().referTo (decayDurationValue);
    decayDurationSlider.setLookAndFeel (&customLookAndFeel);
    decayDurationSlider.onValueChange = [this] ()
    {
//        auto attackDuration = getAttackDuration();
//        auto decayDuration = getDecayDuration();
//        auto sustainLevel = getSustainLevel();
//        auto releaseDuration = getReleaseDuration();
//
//        DBG("attackDuration: " + juce::String(attackDuration));
//        DBG("decayDuration: " + juce::String(decayDuration));
//        DBG("sustainLevel: " + juce::String(sustainLevel));
//        DBG("releaseDuration: " + juce::String(releaseDuration));
        
        //decayDurationValue = decayDurationSlider.getValue();
        repositionPoints();
        resizeSegments();
        repaint();
    };
    
    sustainLevelSlider.setSliderStyle (juce::Slider::SliderStyle::LinearBarVertical);
    sustainLevelSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    sustainLevelSlider.setRange (MIN_SUSTAIN_LEVEL, MAX_SUSTAIN_LEVEL);
    sustainLevelSlider.getValueObject().referTo (sustainLevelValue);
    sustainLevelSlider.setLookAndFeel (&customLookAndFeel);
    sustainLevelSlider.onValueChange = [this] ()
    {
//        auto attackDuration = getAttackDuration();
//        auto decayDuration = getDecayDuration();
//        auto sustainLevel = getSustainLevel();
//        auto releaseDuration = getReleaseDuration();
//
//        DBG("attackDuration: " + juce::String(attackDuration));
//        DBG("decayDuration: " + juce::String(decayDuration));
//        DBG("sustainLevel: " + juce::String(sustainLevel));
//        DBG("releaseDuration: " + juce::String(releaseDuration));
        
        //sustainLevelValue = sustainLevelSlider.getValue();
        repositionPoints();
        resizeSegments();
        repaint();
    };
    
    releaseDurationSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    releaseDurationSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    releaseDurationSlider.setRange (MIN_ADSR_DURATION, MAX_ADSR_DURATION);
    releaseDurationSlider.getValueObject().referTo (releaseDurationValue);
    releaseDurationSlider.setLookAndFeel (&customLookAndFeel);
    releaseDurationSlider.onValueChange = [this] ()
    {
//        auto attackDuration = getAttackDuration();
//        auto decayDuration = getDecayDuration();
//        auto sustainLevel = getSustainLevel();
//        auto releaseDuration = getReleaseDuration();
//
//        DBG("attackDuration: " + juce::String(attackDuration));
//        DBG("decayDuration: " + juce::String(decayDuration));
//        DBG("sustainLevel: " + juce::String(sustainLevel));
//        DBG("releaseDuration: " + juce::String(releaseDuration));
        
        //releaseDurationValue = releaseDurationSlider.getValue();
        repositionPoints();
        resizeSegments();
        repaint();
    };

    addAndMakeVisible (attackDurationSlider);
    addAndMakeVisible (decayDurationSlider);
    addAndMakeVisible (sustainLevelSlider);
    addAndMakeVisible (releaseDurationSlider);
}
    
void ADSRWidget::paint (juce::Graphics &g)
{
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds());
    
    drawGraph(g);
    //drawPoints(g);
    
    //g.setColour (juce::Colours::red);
    //g.drawRect (attackRectangle);
    
    //g.setColour (juce::Colours::blue);
    //g.drawRect (decayRectangle);
    
    //g.setColour (juce::Colours::yellow);
    //g.drawRect (sustainRectangle);
    
    //g.setColour (juce::Colours::green);
    //g.drawRect (releaseRectangle);
}

void ADSRWidget::resized()
{
    bounds = getLocalBounds().reduced(PADDING);
    topEdgeY = bounds.getY();
    bottomEdgeY = bounds.getBottom();
    leftEdgeX = bounds.getX();
    rightEdgeX = bounds.getRight();
    width = bounds.getWidth();
    height = bounds.getHeight();
    equalSegmentWidth = static_cast<float> (width / 4.f);
    
    DBG("topEdgeY: " + juce::String(topEdgeY));
    DBG("bottomEdgeY: " + juce::String(bottomEdgeY));
    DBG("leftEdgeX: " + juce::String(leftEdgeX));
    DBG("rightEdgeX: " + juce::String(rightEdgeX));
    DBG("width: " + juce::String(width));
    DBG("height: " + juce::String(height));
    DBG("equalSegmentWidth: " + juce::String(equalSegmentWidth));
    
    gradient.clearColours();

    // resize the vertical line gradient to fill underneath the graph path
    gradient.point1 = juce::Point<float> (leftEdgeX, bottomEdgeY);
    gradient.point2 = juce::Point<float> (leftEdgeX, topEdgeY);
    
    gradient.addColour (0.f, gradientStartColour);
    gradient.addColour (0.15f, gradientMidColour);
    gradient.addColour (1.f, gradientEndColour);
    
    repositionPoints();
    resizeSegments();
}

void ADSRWidget::mouseEnter (const juce::MouseEvent& event)
{
    
}

void ADSRWidget::mouseMove (const juce::MouseEvent& event)
{
    
}

void ADSRWidget::mouseExit (const juce::MouseEvent& event)
{
    
}

void ADSRWidget::mouseDown (const juce::MouseEvent& event)
{
    
}

void ADSRWidget::mouseDrag (const juce::MouseEvent& event)
{
    
}

void ADSRWidget::mouseUp (const juce::MouseEvent& event)
{
    
}

//==============================================================================

float ADSRWidget::getAttackDuration() { return static_cast<float> (attackDurationValue.getValue()); }

float ADSRWidget::getDecayDuration() { return static_cast<float> (decayDurationValue.getValue()); }

float ADSRWidget::getSustainLevel() { return static_cast<float> (sustainLevelValue.getValue()); }

float ADSRWidget::getReleaseDuration() { return static_cast<float> (releaseDurationValue.getValue()); }

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
    
    // segment point A
    decayStartPoint.setXY (leftEdgeX + attackSegmentWidth, topEdgeY);
    
    // segment point D
    auto decayX = decayStartPoint.getX() + decaySegmentWidth;
    auto decayY = (topEdgeY + height) - static_cast<float> (height * sustainLevel); //- (pointDiameter / 2.f);
    sustainStartPoint.setXY (decayX, decayY);
    
    // segment point S
    releaseStartPoint.setXY (sustainStartPoint.getX() + equalSegmentWidth, sustainStartPoint.getY());
    
    // segment point R
    releaseEndPoint.setXY (releaseStartPoint.getX() + releaseSegmentWidth, bottomEdgeY);
    
    // reposition the bezier curve control points
    auto attachControlPointX = juce::jmap (attackRatePoint.getX(), 0.f, 1.f, static_cast<float> (leftEdgeX), decayStartPoint.getX() );
    auto attackControlPointY = juce::jmap (attackRatePoint.getY(), 0.f, 1.f, static_cast<float> (topEdgeY), static_cast<float> (bottomEdgeY));
    attackControlPoint.setXY (attachControlPointX, attackControlPointY);
    
    auto decayControlPointX = juce::jmap (decayRatePoint.getX(), 0.f, 1.f, decayStartPoint.getX(), decayStartPoint.getX() + decaySegmentWidth);
    auto decayControlPointY = juce::jmap (decayRatePoint.getY(), 0.f, 1.f, decayStartPoint.getY(), sustainStartPoint.getY());
    decayControlPoint.setXY (decayControlPointX, decayControlPointY);
    
    auto releaseControlPointX = juce::jmap (releaseRatePoint.getX(), 0.f, 1.f, releaseStartPoint.getX(), releaseStartPoint.getX() + releaseSegmentWidth);
    auto releaseControlPointY = juce::jmap (releaseRatePoint.getY(), 0.f, 1.f, releaseStartPoint.getY(), static_cast<float> (bottomEdgeY));
    releaseControlPoint.setXY (releaseControlPointX, releaseControlPointY);
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
    
    sustainRectangle.setBounds (sustainStartPoint.getX(), topEdgeY, equalSegmentWidth, releaseEndPoint.getY() - topEdgeY);
    
    releaseRectangle.setX (releaseStartPoint.getX());
    releaseRectangle.setY (releaseStartPoint.getY());
    releaseRectangle.setWidth (releaseEndPoint.getX() - releaseStartPoint.getX());
    releaseRectangle.setHeight (releaseEndPoint.getY() - releaseStartPoint.getY());
    
    attackDurationSlider.setBounds (attackRectangle);
    decayDurationSlider.setBounds (decayRectangle);
    sustainLevelSlider.setBounds (sustainRectangle);
    releaseDurationSlider.setBounds (releaseRectangle);
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
    g.strokePath (path, juce::PathStrokeType(4.f) );
    
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
    
    DBG("attackX: " + juce::String(attackX));
    DBG("attackY: " + juce::String(attackY));
    DBG("decayX: " + juce::String(decayX));
    DBG("decayY: " + juce::String(decayY));
    DBG("sustainX: " + juce::String(sustainX));
    DBG("sustainY: " + juce::String(sustainY));
    DBG("releaseX: " + juce::String(releaseX));
    DBG("releaseY: " + juce::String(releaseY));
    
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
