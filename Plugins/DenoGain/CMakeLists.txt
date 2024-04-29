project(DenoGain VERSION 0.0.1)

set (BaseTargetName DenoGain)

juce_add_plugin("${BaseTargetName}"
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "COx2"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Cox2
    PLUGIN_CODE Deng
    FORMATS Standalone VST3 AU #AUv3 #LV2
    LV2URI "urn:cox2:denogain"
    PRODUCT_NAME "DenoGain")

clap_juce_extensions_plugin(TARGET "${BaseTargetName}"
    CLAP_ID "com.cox2.denogain"
    CLAP_FEATURES audio-effect filter mono stereo)

juce_add_binary_data("${BaseTargetName}_WebViewBundle"
    HEADER_NAME "WebViewBundleData.h"
    NAMESPACE "WebView"
    SOURCES
        WebView/WebViewBundle.zip
    )

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

message(STATUS "[${BaseTargetName}] Binary directory: ${CMAKE_CURRENT_BINARY_DIR}")

# Add cmake install task for this project.
if(TARGET ${BaseTargetName}_VST3)
    get_target_property(VST3_PRODUCT_FILE ${BaseTargetName}_VST3 JUCE_PLUGIN_ARTEFACT_FILE)
    message("VST3_PRODUCT_FILE: ${VST3_PRODUCT_FILE}")

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "/Library/Audio/Plug-Ins/VST3"
                COMPONENT ${BaseTargetName}_VST3)

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "C:/Program Files/Common Files/VST3"
                COMPONENT ${BaseTargetName}_VST3)

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "/usr/lib/vst3"
                COMPONENT ${BaseTargetName}_VST3)

    endif()
endif()

if(TARGET ${BaseTargetName}_CLAP)
    get_target_property(CLAP_PRODUCT_FILE ${BaseTargetName}_CLAP JUCE_PLUGIN_ARTEFACT_FILE)
    message("CLAP_PRODUCT_FILE: ${CLAP_PRODUCT_FILE}")

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

        install(FILES "${CLAP_PRODUCT_FILE}"
                DESTINATION "/Library/Audio/Plug-Ins/CLAP"
                COMPONENT ${BaseTargetName}_CLAP)

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

        install(FILES "${CLAP_PRODUCT_FILE}"
                DESTINATION "C:/Program Files/Common Files/CLAP"
                COMPONENT ${BaseTargetName}_CLAP)

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "/usr/lib/clap"
                COMPONENT ${BaseTargetName}_CLAP)
    endif()
endif()
