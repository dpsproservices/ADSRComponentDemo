//
//  Constants.h
//  ADSRComponentDemo - App
//
//  Created by Joseph Skarulis on 5/29/22.
//

#pragma once

//============================ ADSR envelope model defaults =====================================

// Scaled Attack, Decay, Release durations normalized between 0.1 to 1.0 linear dB scale
// Minimum duration cannot be zero to avoid squared transitions in the ADSR envelope
// These normalized values should be mapped to actual time durations in milliseonds
// based om the host application sample rate (samples per second)
#define MIN_DURATION 0.1f               
#define MAX_DURATION 1.0f               // do not modify
#define DEFAULT_DURATION MAX_DURATION   // do not modify

// Scaled Sustain level normalized between 0.0 to 1.0
// Maximum sustain level 1 means no decay segment
// Zero sustain level means no sustain or release segments, only Attack and Decay
#define MIN_SUSTAIN_LEVEL 0.0f          // do not modify
#define MAX_SUSTAIN_LEVEL 1.0f          // do not modify
#define DEFAULT_SUSTAIN_LEVEL 0.5f      // half way some Decay, some Sustain, some Release

// To avoid overlapping ADSR draggable points with the rate curve control points
// Reduce the bounds of the rectangle areas constraining each of the A, D, R rate curve points
#define RATE_BOUNDS_PERCENT_REDUCE 1.f    // percentage to reduce bounds constraining rate curve points
#define RATE_OFFSET (RATE_BOUNDS_PERCENT_REDUCE / 100.f)    // do not modify
#define MIN_RATE (RATE_OFFSET)                              // do not modify
#define MAX_RATE (1.f - MIN_RATE)                           // do not modify
#define DEFAULT_RATE ((MAX_RATE - MIN_RATE) / 2.f)          // do not modify

//============================ ADSR widget UI defaults =====================================

#define PADDING 40                  // Padding between the inner ADSR graph and outer widget bounds
#define POINT_SIZE 40               // width of the draggable ADSR point components
#define OFFSET 20                   // half the width of the draggable ADSR point component
#define RATE_POINT_SIZE 26          // width of the draggable rate curve control point component
#define FONT_SIZE  16               // font size of the draggable point labels 'A', 'D', 'S', 'R'
