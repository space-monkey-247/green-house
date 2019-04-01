@ECHO OFF

SETLOCAL
SET sourceDir=%CD%\src
echo sourceDir: "%sourceDir%" 

SET dest_dir=c:\Program Files\Arduino\libraries
rem SET dest_dir=c:\Program Files (x86)\Arduino\libraries
echo dest_dir: "%dest_dir%"
echo/

rem delete ubidots-mqtt-esp-master
IF EXIST "%dest_dir%\ubidots-mqtt-esp-master" (
	echo Deleting "%dest_dir%\ubidots-mqtt-esp-master" dir. 
	DEL "%dest_dir%\ubidots-mqtt-esp-master" /Q /F /S
)

rem delete ubidots-particle-mqtt-master
IF EXIST "%dest_dir%\ubidots-particle-mqtt-master" (
	echo Deleting "%dest_dir%\ubidots-particle-mqtt-master" dir. 
	DEL "%dest_dir%\ubidots-particle-mqtt-master" /Q /F /S
)

rem delete solar-panel-master
IF EXIST "%dest_dir%\solar-panel-master" (
	echo Deleting "%dest_dir%\solar-panel-master" dir. 
	DEL "%dest_dir%\solar-panel-master" /Q /F /S
)

IF EXIST "%dest_dir%" (
	xcopy "%sourceDir%" "%dest_dir%" /Y /F /E
)

echo/
echo Libs deployed succesfully
echo/


