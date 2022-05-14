#pragma once

#include <JuceHeader.h>
#include "ADSRWidget.h"

class MainComponent : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    
    juce::Colour backgroundColour;    // dark slate graphite #2B2B2B rgb(43, 43, 43)

    ADSRWidget adsrWidget;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
