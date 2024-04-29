#including CPM.cmake, a package manager:
#https://github.com/TheLartians/CPM.cmake
include(CPM)

#Fetching CHOC from git
#IF you want to instead point it to a local version, you can invoke CMake with
CPMAddPackage("gh:Tracktion/choc#2512542b2d65f3e92df7f2f1f7eeb712fa41a0de")