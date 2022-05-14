#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() :
    backgroundColour (43, 43, 43),
    adsrWidget()
{
    setSize (600, 400);
    
    addAndMakeVisible (adsrWidget);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    g.fillAll (backgroundColour);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    auto topEdgeY = bounds.getY();
//    auto bottomEdgeY = bounds.getHeight();
    auto leftEdgeX = bounds.getX();
//    auto rightEdgeX = bounds.getRight();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();
    
    adsrWidget.setBounds (leftEdgeX, topEdgeY, width, height);
}
