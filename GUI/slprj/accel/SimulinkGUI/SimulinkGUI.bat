@echo off
call "%VS90COMNTOOLS%..\..\VC\bin\vcvars64"

cd .
nmake -f SimulinkGUI.mk  GENERATE_REPORT=0
@if errorlevel 1 goto error_exit
exit /B 0

:error_exit
echo The make command returned an error of %errorlevel%
An_error_occurred_during_the_call_to_make
