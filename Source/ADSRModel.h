//
//  ADSRModel.h
//  ADSRComponentDemo
//
//  Created by Joseph Skarulis on 5/29/22.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Constants.h"
#include "ValuePoint.h"

class ADSRModel
{
public:

    ADSRModel () :
        // Default duration limits
        minAttackDuration (MIN_DURATION), maxAttackDuration (MAX_DURATION),
        minDecayDuration (MIN_DURATION), maxDecayDuration (MAX_DURATION),
        minSustainLevel (MIN_SUSTAIN_LEVEL), maxSustainLevel (MAX_SUSTAIN_LEVEL),
        minReleaseDuration (MIN_DURATION), maxReleaseDuration (MAX_DURATION),
    
        // Default rate limits
        minAttackRate (MIN_RATE), maxAttackRate (MAX_RATE),
        minDecayRate (MIN_RATE), maxDecayRate (MAX_RATE),
        minReleaseRate (MIN_RATE), maxReleaseRate (MAX_RATE),
    
        // Default durations to maximum, sustain to 50%
        attackDurationValue (DEFAULT_DURATION),
        decayDurationValue (DEFAULT_DURATION),
        sustainLevelValue (DEFAULT_SUSTAIN_LEVEL),
        releaseDurationValue (DEFAULT_DURATION),
    
        // set the A, D, R rate points to middle of their segments
        attackRateValuePoint (DEFAULT_RATE, DEFAULT_RATE),
        decayRateValuePoint (DEFAULT_RATE, DEFAULT_RATE),
        releaseRateValuePoint (DEFAULT_RATE, DEFAULT_RATE)
    {
        
    }
    
    //==============================================================================
    
    void setAttackDurationLimits (const float& minDuration, const float& maxDuration)
    {
        minAttackDuration = minDuration;
        maxAttackDuration = maxDuration;
    }
    
    void setDecayDurationLimits (const float& minDuration, const float& maxDuration)
    {
        minDecayDuration = minDuration;
        maxDecayDuration = maxDuration;
    }
    
    void setSustainLevelLimits (const float& minLevel, const float& maxLevel)
    {
        minSustainLevel = minLevel;
        maxSustainLevel = maxLevel;
    }
    
    void setReleaseDurationLimits (const float& minDuration, const float& maxDuration)
    {
        minReleaseDuration = minDuration;
        maxReleaseDuration = maxDuration;
    }
    
    //==============================================================================
    
    void setAttackRateLimits (const float& minRate, const float& maxRate)
    {
        minAttackRate = minRate;
        maxAttackRate = maxRate;
    }
    
    void setDecayRateLimits (const float& minRate, const float& maxRate)
    {
        minDecayRate = minRate;
        maxDecayRate = maxRate;
    }
    
    void setReleaseRateLimits (const float& minRate, const float& maxRate)
    {
        minReleaseRate = minRate;
        maxReleaseRate = maxRate;
    }
    
    //==============================================================================

    void setAttackDuration (const float& attackDuration)
    {
        
        attackDurationValue = juce::jlimit (minAttackDuration, maxAttackDuration, attackDuration);
    }
    
    void setDecayDuration (const float& decayDuration)
    {
        decayDurationValue = juce::jlimit (minDecayDuration, maxDecayDuration, decayDuration);
    }
    
    void setSustainLevel (const float& sustainLevel)
    {
        sustainLevelValue = juce::jlimit (minSustainLevel, maxSustainLevel, sustainLevel);
    }
    
    void setReleaseDuration (const float& releaseDuration)
    {
        releaseDurationValue = juce::jlimit (minReleaseDuration, maxReleaseDuration, releaseDuration);
    }
    
    //==============================================================================
    
    float getAttackDuration() { return static_cast<float> (attackDurationValue.getValue()); }
    
    float getDecayDuration() { return static_cast<float> (decayDurationValue.getValue()); }
    
    float getSustainLevel() { return static_cast<float> (sustainLevelValue.getValue()); }
    
    float getReleaseDuration() { return static_cast<float> (releaseDurationValue.getValue()); }
    
    //==============================================================================
    
    void setAttackDurationValue (const juce::Value& value)
    {
        attackDurationValue.referTo (value);
    }
    
    void setDecayDurationValue (const juce::Value& value)
    {
        decayDurationValue.referTo (value);
    }
    
    void setSustainLevelValue (const juce::Value& value)
    {
        sustainLevelValue.referTo (value);
    }
    
    void setReleaseDurationValue (const juce::Value& value)
    {
        releaseDurationValue.referTo (value);
    }
    
    //==============================================================================
    
    void setAttackRate (const float& x, const float& y)
    {
        attackRateValuePoint.setXY (
            juce::jlimit (minAttackRate, maxAttackRate, x),
            juce::jlimit (minAttackRate, maxAttackRate, y)
        );
    }
    
    void setDecayRate (const float& x, const float& y)
    {
        decayRateValuePoint.setXY (
           juce::jlimit (minDecayRate, maxDecayRate, x),
           juce::jlimit (minDecayRate, maxDecayRate, y)
       );
    }
    
    void setReleaseRate (const float& x, const float& y)
    {
        releaseRateValuePoint.setXY (
            juce::jlimit (minReleaseRate, maxReleaseRate, x),
            juce::jlimit (minReleaseRate, maxReleaseRate, y)
        );
    }
    
    //==============================================================================
    
    void setAttackRate (const juce::Point<float>& centerPoint)
    {
        attackRateValuePoint.setXY (centerPoint.getX(), centerPoint.getY());
    }
    
    void setDecayRate (const juce::Point<float>& centerPoint)
    {
        decayRateValuePoint.setXY (centerPoint.getX(), centerPoint.getY());
    }
    
    void setReleaseRate (const juce::Point<float>& centerPoint)
    {
        releaseRateValuePoint.setXY (centerPoint.getX(), centerPoint.getY());
    }
    
    //==============================================================================
    
    const ValuePoint<float>& getAttackRateValuePoint() { return attackRateValuePoint; }
    
    const ValuePoint<float>& getDecayRateValuePoint() { return decayRateValuePoint; }
    
    const ValuePoint<float>& getReleaseRateValuePoint() { return releaseRateValuePoint; }
    
    //==============================================================================
    
    void setAttackRateValue (const juce::Value& xValueToReferTo, const juce::Value& yValueToReferTo)
    {
        attackRateValuePoint.referToXY (xValueToReferTo, yValueToReferTo);
    }
    
    void setDecayRateValue (const juce::Value& xValueToReferTo, const juce::Value& yValueToReferTo)
    {
        decayRateValuePoint.referToXY (xValueToReferTo, yValueToReferTo);
    }
    
    void setReleaseRateValue (const juce::Value& xValueToReferTo, const juce::Value& yValueToReferTo)
    {
        releaseRateValuePoint.referToXY (xValueToReferTo, yValueToReferTo);
    }
    
    //==============================================================================
    
    /*
     
     Digital Envelope Generators chapter 6.5 page 286
     
     Curve equations for Attack, Decay, Release rates
     
     Convex rate curve diagram A:
     y = x^(0.4 - 0.4x)
     
     Concave rate curve diagram G:
     y = 1 - x^(0.4 - 0.4x)
     
    */
    
    //==============================================================================
    
private:

    float minAttackDuration, maxAttackDuration;
    float minDecayDuration, maxDecayDuration;
    float minSustainLevel, maxSustainLevel;
    float minReleaseDuration, maxReleaseDuration;
    
    float minAttackRate, maxAttackRate;
    float minDecayRate, maxDecayRate;
    float minReleaseRate, maxReleaseRate;
    
    /*
     1 dimensional ADSR parameter values with ranged limits
     Updating the duration values will cause the graph path to resize dynamically
    */
    juce::Value attackDurationValue;
    juce::Value decayDurationValue;
    juce::Value sustainLevelValue;
    juce::Value releaseDurationValue;
    
    /*
     2 dimensional Attack, Decay, Release curve rates
     Updating the rate X or Y values will cause the rate curves
     in the graph path to change slope, concave, convex etc
     
     Initially the XY values will be set to the middle of the segment
     such that the rate curves will be linear by default
    */
    ValuePoint<float> attackRateValuePoint;
    ValuePoint<float> decayRateValuePoint;
    ValuePoint<float> releaseRateValuePoint;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRModel)
};
