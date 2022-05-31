//
//  ValuePoint.hpp
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/24/22.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

using CallbackFunc = const std::function<void()>;

struct ValueListener : juce::Value::Listener
{
    ValueListener (juce::Value& valueToUse, const CallbackFunc& callbackToUse)
        : value (valueToUse)
        , callback (callbackToUse)
    {
        value.addListener (this);
    }

    ~ValueListener() override { value.removeListener (this); }

    void valueChanged (juce::Value& value) override { callback(); }

    juce::Value& value;
    CallbackFunc callback;
};

/* ValuePoint
 * Sublcass juce::Point so as to model a 2D point
 * with X and Y as juce::Value which can be referred to
 */
template <typename ValueType>
class ValuePoint : juce::Point<ValueType>
{
public:

    /** Creates a Value point based at the origin */
    constexpr ValuePoint() = default;

    /** Creates a copy of another Value point. */
    constexpr ValuePoint (const ValuePoint&) = default;
    
    constexpr ValuePoint (ValueType initialX, ValueType initialY) noexcept :
        juce::Point<ValueType> (initialX, initialY),
        xValue (initialX),
        yValue (initialY) {}
    
    //==============================================================================
    
    inline void setX (ValueType newX) noexcept
    {
        juce::Point<ValueType>::setX (newX);
        xValue = newX;
    }

    inline void setY (ValueType newY) noexcept
    {
        juce::Point<ValueType>::setY (newY);
        yValue = newY;
    }

    void setXY (ValueType newX, ValueType newY) noexcept
    {
        juce::Point<ValueType>::setXY (newX, newY);
        xValue = newX;
        yValue = newY;
    }
    
    constexpr inline ValueType getX() const noexcept { return juce::Point<ValueType>::getX(); }

    constexpr inline ValueType getY() const noexcept { return juce::Point<ValueType>::getY(); }

    //==============================================================================

    void referToX (const juce::Value& xValueToReferTo)
    {
        xValue.referTo (xValueToReferTo);
    }

    void referToY (const juce::Value& yValueToReferTo)
    {
        yValue.referTo (yValueToReferTo);
    }
    
    void referToXY (const juce::Value& xValueToReferTo, const juce::Value& yValueToReferTo)
    {
        xValue.referTo (xValueToReferTo);
        yValue.referTo (yValueToReferTo);
    }
    
private:
    
    juce::Value xValue;
    juce::Value yValue;
    
    //==============================================================================
    
    ValueListener xValueListener {xValue, [this] { juce::Point<ValueType>::setX (xValue.getValue()); }};
    ValueListener yValueListener {yValue, [this] { juce::Point<ValueType>::setY (yValue.getValue()); }};
};
