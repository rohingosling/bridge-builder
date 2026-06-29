@echo off
rem **************************************************************************
rem  Build Script: Bridge Builder
rem  Compiler:     Borland Turbo C++ 3.1
rem  Environment:  DOS / DOSBox
rem
rem  Description:
rem
rem    Compiles the Bridge Builder source files (C++) and links them with
rem    GRAPHICS.LIB to produce BRIDGE.EXE.
rem
rem    Uses the large memory model (-ml) for far pointer support. BGI
rem    graphics are used for the bridge visualization interface.
rem
rem    All compiler and linker output is logged to BUILD.LOG.
rem
rem  Usage:
rem
rem    build          Build BRIDGE.EXE
rem
rem **************************************************************************

echo ============================================ >build.log
echo  Bridge Builder - Build Log >>build.log
echo  Compiler: Borland Turbo C++ 3.1 >>build.log
echo ============================================ >>build.log
echo. >>build.log

rem --------------------------------------------------------------------------
rem  Compile
rem --------------------------------------------------------------------------

echo Compiling MAIN.CPP ...
echo ---- MAIN.CPP ---- >> build.log
bcc -c -ml MAIN.CPP >> build.log
if errorlevel 1 goto fail

echo Compiling APP.CPP ...
echo ---- APP.CPP ---- >> build.log
bcc -c -ml APP.CPP >> build.log
if errorlevel 1 goto fail

echo Compiling GEOMETRY.CPP ...
echo ---- GEOMETRY.CPP ---- >> build.log
bcc -c -ml GEOMETRY.CPP >> build.log
if errorlevel 1 goto fail

echo Compiling RENDER.CPP ...
echo ---- RENDER.CPP ---- >> build.log
bcc -c -ml RENDER.CPP >> build.log
if errorlevel 1 goto fail

echo Compiling DISPLAY.CPP ...
echo ---- DISPLAY.CPP ---- >> build.log
bcc -c -ml DISPLAY.CPP >> build.log
if errorlevel 1 goto fail

echo Compiling UTIL.CPP ...
echo ---- UTIL.CPP ---- >> build.log
bcc -c -ml UTIL.CPP >> build.log
if errorlevel 1 goto fail

echo Compiling FILEIO.CPP ...
echo ---- FILEIO.CPP ---- >> build.log
bcc -c -ml FILEIO.CPP >> build.log
if errorlevel 1 goto fail

echo Compiling INIT.CPP ...
echo ---- INIT.CPP ---- >> build.log
bcc -c -ml INIT.CPP >> build.log
if errorlevel 1 goto fail

rem --------------------------------------------------------------------------
rem  Link
rem --------------------------------------------------------------------------

echo Linking BRIDGE.EXE ...
echo ---- LINK BRIDGE.EXE ---- >> build.log
bcc -ml -eBRIDGE.EXE MAIN.OBJ APP.OBJ GEOMETRY.OBJ RENDER.OBJ DISPLAY.OBJ UTIL.OBJ FILEIO.OBJ INIT.OBJ GRAPHICS.LIB >> build.log
if errorlevel 1 goto fail

echo. >> build.log
echo ============================================ >> build.log
echo  Build successful: BRIDGE.EXE >> build.log
echo ============================================ >> build.log

echo.
echo Build successful: BRIDGE.EXE
goto end

rem --------------------------------------------------------------------------
rem  Error
rem --------------------------------------------------------------------------

:fail
echo. >> build.log
echo *** BUILD FAILED *** >> build.log

echo.
echo Build failed! See BUILD.LOG for details.

:end
