#include "PluginProcessor.h"
#include "PluginEditor.h"

HZInverAudioProcessor::HZInverAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

HZInverAudioProcessor::~HZInverAudioProcessor() {}

// ============================================================
//                      AUDIO NO SE USA (solo passthrough)
// ============================================================
void HZInverAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void HZInverAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    // No procesamos audio en tiempo real, es solo utilidad offline.
    juce::ignoreUnused(buffer);
}

// ============================================================
//                       GUI
// ============================================================
juce::AudioProcessorEditor* HZInverAudioProcessor::createEditor()
{
    return new HZInverAudioProcessorEditor(*this);
}

// ============================================================
//                      CARGAR ARCHIVO
// ============================================================
bool HZInverAudioProcessor::loadFile(const juce::File& file)
{
    if (!file.existsAsFile())
    {
        lastMessage = "El archivo no existe.";
        return false;
    }

    loadedFile = file;
    detectedSampleRate = HZResampler::detectSampleRate(file);

    if (detectedSampleRate <= 0)
    {
        lastMessage = "No se pudo detectar el Sample Rate.";
        loadedFile = juce::File();
        return false;
    }

    convertedFile = juce::File(); // limpiar cualquier conversión previa
    lastMessage = "Archivo cargado correctamente.";

    return true;
}

// ============================================================
//                         CONVERTIR
// ============================================================
bool HZInverAudioProcessor::convertFile(bool overwrite)
{
    if (!loadedFile.existsAsFile())
    {
        lastMessage = "No hay archivo cargado.";
        return false;
    }

    // Decide destino automáticamente 44.1 <-> 48 kHz
    double newRate;
    if (std::abs(detectedSampleRate - 44100.0) < 1.0)
        newRate = 48000.0;
    else if (std::abs(detectedSampleRate - 48000.0) < 1.0)
        newRate = 44100.0;
    else
        newRate = (detectedSampleRate < 48000.0 ? 48000.0 : 44100.0);

    juce::String msg;
    juce::File outFile = HZResampler::convertSampleRate(
        loadedFile,
        newRate,
        overwrite,
        msg
    );

    lastMessage = msg;

    if (!outFile.existsAsFile())
        return false;

    convertedFile = outFile;
    return true;
}

// ============================================================
//         FACTORÍA OBLIGATORIA PARA EL PLUGIN JUCE
// ============================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HZInverAudioProcessor();
}
