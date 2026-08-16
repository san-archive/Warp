@echo off
rem  warp 빌드 스크립트
rem
rem    build.bat          릴리스 빌드
rem    build.bat debug    디버그 빌드 (심볼 포함)
rem    build.bat clean    빌드 결과 삭제
rem
rem  x86 전용. 32비트 포인터로 캐스팅하는 레거시 매크로가 있어 64비트에서 동작하지 않는다.
setlocal

set "ROOT=%~dp0"
if not exist "%ROOT%src" if exist "%ROOT%work\src" set "ROOT=%ROOT%work\"

if /i "%~1"=="clean" goto :clean

if defined VCINSTALLDIR goto :have_toolchain

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto :no_toolchain

set "VSPATH="
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -property installationPath`) do set "VSPATH=%%i"
if not defined VSPATH goto :no_toolchain
if not exist "%VSPATH%\VC\Auxiliary\Build\vcvars32.bat" goto :no_toolchain

call "%VSPATH%\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
if errorlevel 1 goto :no_toolchain
if not defined VCINSTALLDIR goto :no_toolchain

:have_toolchain

set "CFLAGS=/nologo /W3 /MT /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /utf-8"
set "LIBS=user32.lib gdi32.lib winmm.lib"

if /i "%~1"=="debug" (
    set "CFLAGS=%CFLAGS% /Od /Zi /D_DEBUG"
    echo [build] configuration: debug
) else (
    set "CFLAGS=%CFLAGS% /O2 /DNDEBUG"
    echo [build] configuration: release
)

pushd "%ROOT%"
if not exist obj mkdir obj
if not exist bin mkdir bin

cl %CFLAGS% /Foobj\ /Fdobj\warp.pdb /Febin\warp.exe src\*.c src\*.cpp /link /SUBSYSTEM:WINDOWS %LIBS%
set "RC=%ERRORLEVEL%"
popd

if not "%RC%"=="0" (
    echo [build] FAILED ^(exit %RC%^)
    exit /b %RC%
)

echo [build] ok -^> bin\warp.exe
exit /b 0

:clean
pushd "%ROOT%"
if exist obj rmdir /s /q obj
if exist bin\warp.exe del /q bin\warp.exe
popd
echo [build] cleaned
exit /b 0

:no_toolchain
echo [build] no Visual Studio x86 toolchain found.
echo [build] install "Visual Studio Build Tools" with the C++ workload,
echo [build] or run this from a "x86 Native Tools Command Prompt".
exit /b 1
