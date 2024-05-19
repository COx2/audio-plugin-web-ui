#including CPM.cmake, a package manager:
#https://github.com/TheLartians/CPM.cmake
include(CPM)

#Fetching CHOC from git
#IF you want to instead point it to a local version, you can invoke CMake with
CPMAddPackage("gh:Tracktion/choc#6d591ce4ff202cf116edb71cf76832e5e1b3eeb1")