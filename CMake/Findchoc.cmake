#including CPM.cmake, a package manager:
#https://github.com/TheLartians/CPM.cmake
include(CPM)

#Fetching CHOC from git
#IF you want to instead point it to a local version, you can invoke CMake with
CPMAddPackage("gh:Tracktion/choc#b086c628c82f2e12c794ad2642b11b41ef6f50e1")