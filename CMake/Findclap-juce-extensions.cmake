#including CPM.cmake, a package manager:
#https://github.com/TheLartians/CPM.cmake
include(CPM)

#Fetching clap-juce-extensions from git
#IF you want to instead point it to a local version, you can invoke CMake with
CPMAddPackage("gh:free-audio/clap-juce-extensions#54d83e121545490fdbb2d3dec3ba0c1da6005dde")