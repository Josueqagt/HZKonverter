#include "Resampler.h"
#include <cmath>

// ==========================================================
//  Utilidades de log (C:\HZInver\hzlog.txt)
// ==========================================================
namespace
{
    juce::File getLogFile()
    {
        return juce::File("C:/HZInver/hzlog.txt");
    }

    void logLine(const juce::String& s)
    {
        auto f = getLogFile();
        f.appendText(s + "\n");
    }
}

// ==========================================================
//  Detectar Sample Rate
// ==========================================================
double HZResampler::detectSampleRate(const juce::File& file)
{
    juce::AudioFormatManager fm;
    fm.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(fm.createReaderFor(file));
    if (reader == nullptr)
        return 0.0;

    return reader->sampleRate;
}

// ==========================================================
//  Convertir Sample Rate (lectura completa + Lagrange)
// ==========================================================
juce::File HZResampler::convertSampleRate(const juce::File& input,
                                          double newRate,
                                          bool overwrite,
                                          juce::String& outMessage)
{
    outMessage.clear();

    if (! input.existsAsFile())
    {
        outMessage = "Error: el archivo de entrada no existe.";
        return juce::File();
    }

    juce::AudioFormatManager fm;
    fm.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(fm.createReaderFor(input));
    if (reader == nullptr)
    {
        outMessage = "Error: no se pudo leer el archivo.";
        return juce::File();
    }

    const int numChannels   = (int) reader->numChannels;
    const double inRate     = reader->sampleRate;
    const juce::int64 inLen = reader->lengthInSamples;

    if (numChannels <= 0 || inLen <= 0)
    {
        outMessage = "Error: archivo de audio vacio o invalido.";
        return juce::File();
    }

    if (std::abs(inRate - newRate) < 1.0)
    {
        outMessage = "El archivo ya esta en ese Sample Rate.";
        return input;
    }

    // ========= LOG DE ENTRADA =========
    logLine("==== Iniciando conversion ====");
    logLine("Archivo: " + input.getFullPathName());
    logLine("Canales: " + juce::String(numChannels));
    logLine("SampleRate origen: " + juce::String(inRate));
    logLine("SampleRate destino: " + juce::String(newRate));
    logLine("Samples totales: " + juce::String(inLen));

    // ======================================================
    // 1) Leer TODO el archivo en memoria
    // ======================================================
    if (inLen > std::numeric_limits<int>::max())
    {
        outMessage = "Error: archivo demasiado largo para este metodo.";
        return juce::File();
    }

    const int inSamplesInt = (int) inLen;

    juce::AudioBuffer<float> readBuffer(numChannels, inSamplesInt);
    if (! reader->read(&readBuffer, 0, inSamplesInt, 0, true, true))
    {
        outMessage = "Error: fallo al leer el audio.";
        return juce::File();
    }

    // ======================================================
    // 2) Calcular muestras de salida y crear buffer salida
    //    N_out = ceil(newRate * N_in / inRate)
    // ======================================================
    const double ratioSR    = newRate / inRate;
    const juce::int64 outLen64 = (juce::int64) std::ceil(ratioSR * (double) inLen);

    if (outLen64 <= 0 || outLen64 > std::numeric_limits<int>::max())
    {
        outMessage = "Error: longitud de salida invalida.";
        return juce::File();
    }

    const int outSamplesInt = (int) outLen64;
    juce::AudioBuffer<float> outBuffer(numChannels, outSamplesInt);
    outBuffer.clear();

    // ======================================================
    // 3) Resampling por canal con LagrangeInterpolator
    //    IMPORTANTE: ratio = inRate / newRate
    //    y el ultimo parametro es el NUMERO DE MUESTRAS DE SALIDA
    // ======================================================
    const float* inPtr  = nullptr;
    float*       outPtr = nullptr;

    for (int ch = 0; ch < numChannels; ++ch)
    {
        juce::LagrangeInterpolator interp;
        interp.reset();

        inPtr  = readBuffer.getReadPointer(ch);
        outPtr = outBuffer.getWritePointer(ch);

        const double speedRatio = inRate / newRate;

        const int consumed = interp.process(speedRatio,
                                            inPtr,
                                            outPtr,
                                            outSamplesInt);

        // Log basico por canal
        logLine("Canal " + juce::String(ch) +
                " - outSamples=" + juce::String(outSamplesInt) +
                " - inConsumidos=" + juce::String(consumed));
    }

    // ======================================================
    // 4) Preparar archivo de salida junto al original
    // ======================================================
    juce::File output = input;

    if (! overwrite)
    {
        juce::String suffix = (newRate > inRate ? "_48hz" : "_44hz");
        auto parent = input.getParentDirectory();
        auto newName = input.getFileNameWithoutExtension() + suffix + ".wav";
        output = parent.getChildFile(newName);
    }

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::FileOutputStream> outStream(output.createOutputStream());

    if (outStream == nullptr)
    {
        outMessage = "Error: no se pudo crear el archivo de salida.";
        return juce::File();
    }

    auto* rawStream = outStream.get();
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wav.createWriterFor(rawStream,
                            newRate,
                            (unsigned int) numChannels,
                            24,              // bits de salida
                            {},              // metadata vacia
                            0));             // calidad por defecto

    // El writer ahora es dueño del stream:
    outStream.release();

    if (writer == nullptr)
    {
        outMessage = "Error: no se pudo crear el writer WAV.";
        delete rawStream; // por si createWriterFor fallo
        return juce::File();
    }

    const bool ok = writer->writeFromAudioSampleBuffer(outBuffer, 0, outSamplesInt);

    if (! ok)
    {
        outMessage = "Error al escribir el audio de salida.";
        return juce::File();
    }

    logLine("Total frames escritos: " + juce::String(outSamplesInt));
    logLine("==== Conversion finalizada OK ====\n");

    outMessage = "Archivo guardado en: " + output.getFullPathName();
    return output;
}
