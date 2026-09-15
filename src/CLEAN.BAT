@echo off
rem **************************************************************************
rem  Clean Script: Bridge Builder
rem
rem  Description:
rem
rem    Removes all build artifacts (object files, executables, and logs).
rem
rem  Usage:
rem
rem    clean
rem
rem **************************************************************************

echo Cleaning build artifacts ...

if exist MAIN.OBJ        del MAIN.OBJ
if exist APP.OBJ         del APP.OBJ
if exist GEOMETRY.OBJ    del GEOMETRY.OBJ
if exist RENDER.OBJ      del RENDER.OBJ
if exist DISPLAY.OBJ     del DISPLAY.OBJ
if exist UTIL.OBJ        del UTIL.OBJ
if exist FILEIO.OBJ      del FILEIO.OBJ
if exist INIT.OBJ        del INIT.OBJ
if exist BRIDGE.EXE      del BRIDGE.EXE
if exist BUILD.LOG       del BUILD.LOG

echo Done.
