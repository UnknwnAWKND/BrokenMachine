cmake_minimum_required(VERSION 3.22)

project(BrokenMachine VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    JUCE
    URL https://github.com/juce-framework/JUCE/archive/refs/tags/9.0.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(BrokenMachine
    COMPANY_NAME "Unknown Awakening"
    BUNDLE_ID "com.unknownawakening.brokenmachine"
    PLUGIN_MANUFACTURER_CODE Unkn
    PLUGIN_CODE BrMa
    FORMATS VST3
    PRODUCT_NAME "Broken Machine"
    PLUGIN_NAME "Broken Machine"
    DESCRIPTION "Dynamic 424-style preamp, distorted reverb, and unstable guitar processor"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Fx
)

juce_generate_juce_header(BrokenMachine)

target_sources(BrokenMachine PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Parameters/Parameters.cpp
    Source/Parameters/Parameters.h
    Source/DSP/EnvelopeFollower.cpp
    Source/DSP/EnvelopeFollower.h
    Source/DSP/Compressor.cpp
    Source/DSP/Compressor.h
    Source/DSP/Saturator.cpp
    Source/DSP/Saturator.h
    Source/DSP/WowFlutter.cpp
    Source/DSP/WowFlutter.h
    Source/DSP/MicroDouble.cpp
    Source/DSP/MicroDouble.h
    Source/DSP/ToneEngine.cpp
    Source/DSP/ToneEngine.h
    Source/DSP/SpeakerModel.cpp
    Source/DSP/SpeakerModel.h
    Source/DSP/SpatialProcessor.cpp
    Source/DSP/SpatialProcessor.h
    Source/DSP/SafetyLimiter.cpp
    Source/DSP/SafetyLimiter.h
    Source/UI/CustomLookAndFeel.cpp
    Source/UI/CustomLookAndFeel.h
    Source/UI/Meter.cpp
    Source/UI/Meter.h
)

target_compile_definitions(BrokenMachine PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(BrokenMachine
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

if(MSVC)
    target_compile_options(BrokenMachine PRIVATE /permissive- /Zc:__cplusplus)
endif()
