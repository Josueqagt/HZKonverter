#pragma once
#include "JuceHeader.h"

class HZResampler
{
public:
    static double detectSampleRate(const juce::File& file);

    static juce::File convertSampleRate(
        const juce::File& input,
        double newRate,
        bool overwrite,
        juce::String& outMessage
    );
};
