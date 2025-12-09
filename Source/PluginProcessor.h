#pragma once
#include "JuceHeader.h"
#include "Resampler.h"

class HZInverAudioProcessor : public juce::AudioProcessor
{
public:
    HZInverAudioProcessor();
    ~HZInverAudioProcessor() override;

    // ==== Funciones requeridas JUCE ====
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override { return true; }
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "HZInver"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override {}
    void setStateInformation(const void* data, int sizeInBytes) override {}

    // ==============================================================
    //               LÓGICA DE ARCHIVO / RESAMPLING
    // ==============================================================

    /** Cargar archivo desde GUI */
    bool loadFile(const juce::File& file);

    /** Ejecutar conversión 44.1 ↔ 48 */
    bool convertFile(bool overwrite);

    /** Getters expuestos al Editor */
    double getDetectedSampleRate() const { return detectedSampleRate; }
    juce::File getLoadedFile() const { return loadedFile; }
    juce::File getConvertedFile() const { return convertedFile; }
    juce::String getLastMessage() const { return lastMessage; }
    bool hasConvertedFile() const { return convertedFile.exists(); }
    bool hasLoadedFile() const { return loadedFile.exists(); }

private:
    juce::File loadedFile;
    juce::File convertedFile;

    double detectedSampleRate = 0.0;
    juce::String lastMessage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HZInverAudioProcessor)
};
