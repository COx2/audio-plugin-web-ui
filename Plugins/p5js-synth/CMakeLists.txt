project(p5.js-Synth VERSION 0.0.1)

set (BaseTargetName p5js-synth)

juce_add_plugin("${BaseTargetName}"
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "COx2"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Cox2
    PLUGIN_CODE P5js
    FORMATS AU AUv3 LV2 VST3 Standalone
    LV2URI "urn:cox2:p5js-synth"
    PRODUCT_NAME "p5js-synth")

clap_juce_extensions_plugin(TARGET "${BaseTargetName}"
    CLAP_ID "com.cox2.p5js-synth"
    CLAP_FEATURES instrument mono stereo)

juce_add_binary_data("${BaseTargetName}_WebViewBundle"
    HEADER_NAME "WebViewBundleData.h"
    NAMESPACE "WebView"
    SOURCES
        WebView/view.html)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties("${BaseTargetName}_WebViewBundle"
    PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE)

target_sources(${BaseTargetName}
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h)

target_compile_definitions(${BaseTargetName}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# Use PUBLIC to effect linked target like VST3, Standalone, CLAP, ...
target_compile_features(${BaseTargetName}
    PUBLIC
        cxx_std_17)

target_include_directories(${BaseTargetName}
    PUBLIC
        ${choc_SOURCE_DIR})

target_link_libraries(${BaseTargetName}
    PRIVATE
        "${BaseTargetName}_WebViewBundle"
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_audio_devices
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# On OSX, need to add the WebKit framework for WebView functionality
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(${BaseTargetName}
        PUBLIC
            "-framework WebKit -framework CoreServices")
endif ()

# For CHOC features.
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        find_package(PkgConfig REQUIRED)
        pkg_check_modules (gtk3 REQUIRED gtk+-3.0 IMPORTED_TARGET)
        pkg_check_modules (webkit2 REQUIRED webkit2gtk-4.0 IMPORTED_TARGET)
        target_link_libraries (${BaseTargetName} PUBLIC pthread PkgConfig::gtk3 PkgConfig::webkit2)
endif ()

message(STATUS "Binary directory: ${CMAKE_CURRENT_BINARY_DIR}")

# Copy my_executable to /install_folder after building
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${BaseTargetName}_artefacts/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>"
        DESTINATION "./${BaseTargetName}/")