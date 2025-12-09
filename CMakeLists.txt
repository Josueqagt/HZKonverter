cmake_minimum_required(VERSION 3.15)

project(HZKonverter VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(JUCE)

juce_add_plugin(HZInver
    COMPANY_NAME "AudioCream"
    BUNDLE_ID com.tenuserver.hzkonverter
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE TeNu
    PLUGIN_CODE HZIv
    FORMATS VST3 Standalone
    PRODUCT_NAME "HZKonverter"
)

juce_generate_juce_header(HZInver)

target_sources(HZInver PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Resampler.cpp
    Source/Resampler.h
)

target_link_libraries(HZInver PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

target_compile_definitions(HZInver
    PRIVATE
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_VST3_CAN_REPLACE_VST2=0
)
