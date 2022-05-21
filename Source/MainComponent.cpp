#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() :
    backgroundColour (43, 43, 43),
    adsrWidget()
{
    setSize (680, 440);
    addAndMakeVisible (adsrWidget);
    
//    setSize (600, 400);
//    targetArea.setBounds(450, 250, 150, 140);
//    addAndMakeVisible (targetArea);
//    targetSource.setBounds(0,0,50,50);
//    addAndMakeVisible (targetSource);
    
//    setOpaque (false);

}

MainComponent::~MainComponent() { }

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    g.fillAll (backgroundColour);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(PADDING);
    auto topEdgeY = bounds.getY();
//    auto bottomEdgeY = bounds.getHeight();
    auto leftEdgeX = bounds.getX();
//    auto rightEdgeX = bounds.getRight();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();
    
    adsrWidget.setBounds (leftEdgeX, topEdgeY, width, height);
}
