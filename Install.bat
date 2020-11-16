@echo off
echo ____________________________________________________________________________
echo !!!!.... Installing XLance Componant ....!!!!
echo !
echo !

SET fwpath= "%windir%\Microsoft.NET\Framework\v2.0.50727"
SET fwpath1= "%windir%\Microsoft.NET\Framework\v4.0.30319"

%fwpath%\RegAsm.exe "%~dp0%\XLanceDLL.dll"
%fwpath1%\RegAsm.exe "%~dp0%\XLanceDLL.dll"

echo !
echo !!!!.... Installation Finished ....!!!!
echo ____________________________________________________________________________
pause