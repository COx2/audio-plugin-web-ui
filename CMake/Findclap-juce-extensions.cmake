#including CPM.cmake, a package manager:
#https://github.com/TheLartians/CPM.cmake
include(CPM)

#Fetching clap-juce-extensions from git
#IF you want to instead point it to a local version, you can invoke CMake with
CPMAddPackage("gh:free-audio/clap-juce-extensions#e286d28dc5309b744d748c60723e8d94141e1b3f")