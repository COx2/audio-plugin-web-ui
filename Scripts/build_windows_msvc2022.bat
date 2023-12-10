@echo off

set SCRIPT_DIRECTORY=%~dp0
cd "%SCRIPT_DIRECTORY%\.."

set "VSDEVCMD_2022_ENTERPRISE=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
set "VSDEVCMD_2022_PROFESSIONAL=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
set "VSDEVCMD_2022_COMMUNITY=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set "GENERATOR_VS2022=Visual Studio 17 2022"

if exist "%VSDEVCMD_2022_ENTERPRISE%" (
    echo Use VS2022 Enterprise
    call "%VSDEVCMD_2022_ENTERPRISE%"
) else if exist "%VSDEVCMD_2022_PROFESSIONAL%" (
    echo Use VS2022 Professional
    call "%VSDEVCMD_2022_PROFESSIONAL%"
) else if exist "%VSDEVCMD_2022_COMMUNITY%" (
    echo Use VS2022 Community
    call "%VSDEVCMD_2022_COMMUNITY%"
) else (
    echo Error: VS2022 is not installed. 1>&2
    exit /b 1
)
if %ERRORLEVEL% neq 0 goto FAILURE

echo "[audio-plugin-web-ui] Configure from preset..."
cmake --preset vs2022

echo "[audio-plugin-web-ui] Build Debug version..."
cmake --build --preset vs2022-debug

echo "[audio-plugin-web-ui] Build Release version..."
cmake --build --preset vs2022-release

exit /b 0

:FAILURE
echo ErrorLevel:%ERRORLEVEL%
echo ***Build Failed***
exit 1

:SUCCESS
echo ***Build Success***
exit /b 0
