#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"


class HZInverAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      public juce::FileDragAndDropTarget,
      public juce::Button::Listener
{
public:
    explicit HZInverAudioProcessorEditor(HZInverAudioProcessor&);
    ~HZInverAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void buttonClicked(juce::Button* button) override;

private:
    HZInverAudioProcessor& audioProcessor;

    
    juce::Label titleLabel;
    juce::Label fileLabel;
    juce::Label rateLabel;
    juce::Label statusLabel;
    juce::Label dropHintLabel;

    juce::TextButton loadButton { "Cargar archivo..." };
    juce::TextButton convertButton { "Convertir de 44.1 a 48 kHz" };
    juce::TextButton downloadButton { "Descargar..." };

    juce::ToggleButton overwriteToggle { "Sobrescribir archivo original" };
    juce::Image logoImage;
    juce::Image headerLogo;   // nuevo: logo "HZKONVER" para el encabezado

    // 👇 NUEVO: un único FileChooser reutilizable
    std::unique_ptr<juce::FileChooser> fileChooser;

    void updateLabelsFromProcessor();
    void tryLoadFile(const juce::File& file);
    void tryConvert();
    void tryDownload();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HZInverAudioProcessorEditor)
};
