#include "PluginEditor.h"
#include "PluginProcessor.h"

HZInverAudioProcessorEditor::HZInverAudioProcessorEditor(HZInverAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(960, 660);

    // ===== Paleta de colores general =====
    auto& lf = getLookAndFeel();
    lf.setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff101015));
    lf.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff262b33));
    lf.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xfff39c12));
    lf.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    lf.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    lf.setColour(juce::ToggleButton::textColourId, juce::Colours::lightgrey);

    // ===== Título (texto, por si acaso / accesibilidad) =====
    addAndMakeVisible(titleLabel);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff1c40f));

    // ===== Info de archivo y sample rate =====
    addAndMakeVisible(fileLabel);
    fileLabel.setText("Archivo: (ninguno)", juce::dontSendNotification);
    fileLabel.setJustificationType(juce::Justification::centredLeft);
    fileLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);

    addAndMakeVisible(rateLabel);
    rateLabel.setText("Sample Rate: -", juce::dontSendNotification);
    rateLabel.setJustificationType(juce::Justification::centredLeft);
    rateLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);

    // ===== Mensajes de estado =====
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Usa 'Cargar archivo...' o arrastra un archivo en el recuadro central.",
                        juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centredRight);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::grey);

    // OJO: mantenemos dropHintLabel oculto (no addAndMakeVisible) para evitar duplicar textos.

    // ===== Botones =====
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Cargar archivo...");
    loadButton.addListener(this);

    addAndMakeVisible(convertButton);
    convertButton.setButtonText("Convertir de 44.1 a 48 kHz");
    convertButton.addListener(this);

    addAndMakeVisible(downloadButton);
    downloadButton.setButtonText("Descargar...");
    downloadButton.addListener(this);

    addAndMakeVisible(overwriteToggle);
    overwriteToggle.setButtonText("Sobrescribir archivo original");
    overwriteToggle.setToggleState(false, juce::dontSendNotification);

    // ===== Logos =====
    // Logo grande "Audio Cream" para el área de drag & drop
    {
        juce::File root("C:/HZInver");
        juce::File watermarkFile = root.getChildFile("HZInverLogo.png");

        if (watermarkFile.existsAsFile())
        {
            logoImage = juce::ImageFileFormat::loadFrom(watermarkFile);
        }
        else
        {
            DBG("No se encontro HZInverLogo.png en C:/HZInver");
        }

        // Logo del encabezado: C:\HZInver\LOGO\HZKONVER.png
        juce::File headerDir  = root.getChildFile("LOGO");
        juce::File headerFile = headerDir.getChildFile("HZKONVER.png");

        if (headerFile.existsAsFile())
        {
            headerLogo = juce::ImageFileFormat::loadFrom(headerFile);
        }
        else
        {
            DBG("No se encontro HZKONVER.png en C:/HZInver/LOGO");
        }
    }

    updateLabelsFromProcessor();
}

HZInverAudioProcessorEditor::~HZInverAudioProcessorEditor()
{
    loadButton.removeListener(this);
    convertButton.removeListener(this);
    downloadButton.removeListener(this);
}

// =====================================================================
//                             PAINT (DISEÑO)
// =====================================================================
void HZInverAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto fullBounds = getLocalBounds();

    // ===== Fondo general tipo "FL Studio" con 3 tonos =====
    juce::Colour top(0xff141821);
    juce::Colour mid(0xff1b202b);
    juce::Colour bottom(0xff090b10);

    juce::ColourGradient bgGrad(top,
                                0.0f, 0.0f,
                                bottom,
                                0.0f, (float)fullBounds.getHeight(),
                                false);
    bgGrad.addColour(0.45, mid);

    g.setGradientFill(bgGrad);
    g.fillAll();

    // ===== Banda superior para el logo =====
    juce::Rectangle<int> headerArea = fullBounds.removeFromTop(80);

    juce::Colour headTop(0xff202532);
    juce::Colour headBottom(0xff151822);
    juce::ColourGradient headGrad(headTop,
                                  0.0f, (float)headerArea.getY(),
                                  headBottom,
                                  0.0f, (float)headerArea.getBottom(),
                                  false);
    g.setGradientFill(headGrad);
    g.fillRect(headerArea);

    // Línea de separación inferior de la banda
    g.setColour(juce::Colour(0x66000000));
    g.drawLine(0.0f,
               (float)headerArea.getBottom(),
               (float)getWidth(),
               (float)headerArea.getBottom(),
               1.0f);

    g.setColour(juce::Colour(0x22ffffff));
    g.drawLine(0.0f,
               (float)headerArea.getBottom() - 1.0f,
               (float)getWidth(),
               (float)headerArea.getBottom() - 1.0f,
               1.0f);

    // ===== Logo HZKONVER centrado en el header =====
    if (headerLogo.isValid())
    {
        int maxW = headerArea.getWidth() * 3 / 5;   // 60% del ancho
        int maxH = headerArea.getHeight() - 14;     // pequeño margen vertical

        int posX = headerArea.getX() + (headerArea.getWidth()  - maxW) / 2;
        int posY = headerArea.getY() + (headerArea.getHeight() - maxH) / 2;

        g.setOpacity(0.98f);
        g.drawImageWithin(headerLogo,
                          posX,
                          posY,
                          maxW,
                          maxH,
                          juce::RectanglePlacement::centred);
        g.setOpacity(1.0f);
    }

    // =================================================================
    //      Área de contenido (labels + recuadro de arrastre + botones)
    // =================================================================
    juce::Rectangle<int> content = getLocalBounds().reduced(20);
    content.removeFromTop(80 + 10); // quitamos header + pequeño espacio

    // Dejar espacio para "Archivo:" y "Sample Rate:" debajo del logo
    juce::Rectangle<int> labelsZone = content.removeFromTop(60);
    juce::ignoreUnused(labelsZone); // solo define la altura, los Labels se posicionan en resized()

    // Definir el área de drag & drop en el centro
    juce::Rectangle<int> dropArea = content;
    dropArea.removeFromBottom(90);  // espacio para los botones
    dropArea.reduce(0, 10);         // un poco de margen vertical interno

    // ===== Fondo del recuadro de arrastre =====
    g.setColour(juce::Colours::black.withAlpha(0.40f));
    g.fillRoundedRectangle(dropArea.toFloat(), 14.0f);

    // ===== Logo "Audio Cream" como watermark dentro del recuadro =====
    if (logoImage.isValid())
    {
        float alpha = 0.16f;
        int logoW = (int)(dropArea.getWidth() * 0.55f);
        int logoH = (int)(dropArea.getHeight() * 0.75f);

        int posX = dropArea.getX() + (dropArea.getWidth()  - logoW) / 2;
        int posY = dropArea.getY() + (dropArea.getHeight() - logoH) / 2;

        g.setOpacity(alpha);
        g.drawImageWithin(logoImage,
                          posX,
                          posY,
                          logoW,
                          logoH,
                          juce::RectanglePlacement::centred);
        g.setOpacity(1.0f);
    }

    // Borde exterior + pseudo "inner glow"
    g.setColour(juce::Colour(0x55ffffff));
    g.drawRoundedRectangle(dropArea.toFloat(), 14.0f, 2.0f);
    g.setColour(juce::Colour(0x33000000));
    g.drawRoundedRectangle(dropArea.reduced(3).toFloat(), 12.0f, 1.0f);

    // ===== Texto de ayuda centrado dentro del recuadro =====
    g.setColour(juce::Colours::white.withAlpha(0.90f));
    g.setFont(16.0f);
    g.drawFittedText("Arrastra aqui tu archivo de audio\n(WAV, MP3, FLAC, etc.)",
                     dropArea.reduced(24),
                     juce::Justification::centred,
                     3);
}

// =====================================================================
//                             RESIZED (LAYOUT)
// =====================================================================
void HZInverAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    // ---- Header (ya pintado en paint, aquí solo colocamos el título opcional) ----
    auto headerArea = area.removeFromTop(80);
    titleLabel.setBounds(headerArea.removeFromLeft(headerArea.getWidth() / 3));

    // ---- Zona de info de archivo debajo del logo ----
    auto infoArea = area.removeFromTop(60);
    fileLabel.setBounds(infoArea.removeFromTop(24));
    rateLabel.setBounds(infoArea.removeFromTop(24));

    // ---- Zona central (drag & drop). Solo usamos esto para el dropHintLabel oculto ----
    auto centerArea = area;
    centerArea.removeFromBottom(90); // reservamos la parte inferior para botones
    centerArea.reduce(10, 10);
    dropHintLabel.setBounds(centerArea.reduced(40)); // aunque no sea visible, queda centrado

    // ---- Zona inferior: botones y status ----
    auto bottom = area.removeFromBottom(90);
    auto leftBottom = bottom.removeFromLeft(bottom.getWidth() * 2 / 3);
    auto rightBottom = bottom;

    loadButton.setBounds(leftBottom.removeFromTop(30).removeFromLeft(230).reduced(4));
    convertButton.setBounds(leftBottom.removeFromTop(34).removeFromLeft(280).reduced(4));
    overwriteToggle.setBounds(leftBottom.removeFromTop(26));

    downloadButton.setBounds(rightBottom.removeFromTop(30).removeFromRight(190).reduced(4));
    statusLabel.setBounds(rightBottom);
}

// =====================================================================
//                        DRAG & DROP DE ARCHIVOS
// =====================================================================
bool HZInverAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    if (files.isEmpty())
        return false;

    juce::File f(files[0]);
    auto ext = f.getFileExtension().toLowerCase();

    return (ext == ".wav" || ext == ".aiff" || ext == ".aif" || ext == ".flac" || ext == ".mp3");
}

void HZInverAudioProcessorEditor::filesDropped(const juce::StringArray& files, int, int)
{
    if (files.isEmpty())
        return;

    juce::File f(files[0]);
    tryLoadFile(f);
}

// =====================================================================
//                               BOTONES
// =====================================================================
void HZInverAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Selecciona un archivo de audio...",
            juce::File(),
            "*.wav;*.aiff;*.aif;*.flac;*.mp3");

        auto flags = juce::FileBrowserComponent::openMode
                   | juce::FileBrowserComponent::canSelectFiles;

        auto* chooserPtr = fileChooser.get();

        chooserPtr->launchAsync(flags,
            [this](const juce::FileChooser& fc)
            {
                auto f = fc.getResult();
                if (f.existsAsFile())
                    tryLoadFile(f);
            });
    }
    else if (button == &convertButton)
    {
        tryConvert();
    }
    else if (button == &downloadButton)
    {
        tryDownload();
    }
}

// =====================================================================
//                               HELPERS
// =====================================================================
void HZInverAudioProcessorEditor::updateLabelsFromProcessor()
{
    if (audioProcessor.hasLoadedFile())
    {
        fileLabel.setText("Archivo: " + audioProcessor.getLoadedFile().getFileName(),
                          juce::dontSendNotification);

        auto sr = audioProcessor.getDetectedSampleRate();
        juce::String target = (sr == 44100.0 ? "48,000 Hz" : (sr == 48000.0 ? "44,100 Hz" : "-"));

        rateLabel.setText("Sample Rate detectado: " + juce::String(sr, 1) +
                              " Hz  →  destino: " + target,
                          juce::dontSendNotification);
    }
    else
    {
        fileLabel.setText("Archivo: (ninguno)", juce::dontSendNotification);
        rateLabel.setText("Sample Rate: -", juce::dontSendNotification);
    }

    statusLabel.setText(audioProcessor.getLastMessage(), juce::dontSendNotification);
}

void HZInverAudioProcessorEditor::tryLoadFile(const juce::File& file)
{
    if (audioProcessor.loadFile(file))
        statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xff2ecc71));
    else
        statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe74c3c));

    updateLabelsFromProcessor();
    repaint();
}

void HZInverAudioProcessorEditor::tryConvert()
{
    bool overwrite = overwriteToggle.getToggleState();

    statusLabel.setText("Convirtiendo archivo, por favor espera...",
                        juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff1c40f));
    repaint(); // fuerza refresco antes del trabajo pesado

    if (audioProcessor.convertFile(overwrite))
    {
        statusLabel.setText("Conversion finalizada.", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xff2ecc71));
    }
    else
    {
        statusLabel.setText(audioProcessor.getLastMessage(), juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe74c3c));
    }

    updateLabelsFromProcessor();
    repaint();
}

void HZInverAudioProcessorEditor::tryDownload()
{
    if (!audioProcessor.hasConvertedFile())
    {
        statusLabel.setText("No se ha convertido ningun archivo todavia", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe67e22));
        return;
    }

    auto src = audioProcessor.getConvertedFile();

    fileChooser = std::make_unique<juce::FileChooser>(
        "Guardar archivo convertido como...",
        src,
        "*" + src.getFileExtension());

    auto flags = juce::FileBrowserComponent::saveMode
               | juce::FileBrowserComponent::canSelectFiles;

    auto* chooserPtr = fileChooser.get();

    chooserPtr->launchAsync(flags,
        [this, src](const juce::FileChooser& fc)
        {
            auto dest = fc.getResult();
            if (! dest.getFullPathName().isNotEmpty())
                return;

            if (src.copyFileTo(dest))
            {
                statusLabel.setText("Archivo guardado en: " + dest.getFullPathName(),
                                    juce::dontSendNotification);
                statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xff2ecc71));
            }
            else
            {
                statusLabel.setText("No se pudo copiar el archivo convertido.",
                                    juce::dontSendNotification);
                statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe74c3c));
            }

            repaint();
        });
}