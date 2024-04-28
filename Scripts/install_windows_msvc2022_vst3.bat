@echo off

set SCRIPT_DIRECTORY=%~dp0
cd "%SCRIPT_DIRECTORY%\.."

echo "Install Release version..."
cmake --install %SCRIPT_DIRECTORY%/../builds/vs2022 --prefix Release --component WebGain_VST3
cmake --install %SCRIPT_DIRECTORY%/../builds/vs2022 --prefix Release --component DenoGain_VST3

exit /b 0

:FAILURE
echo ErrorLevel:%ERRORLEVEL%
echo ***Build Failed***
exit 1

:SUCCESS
echo ***Build Success***
exit /b 0
