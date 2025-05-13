@echo off
REM Compile poslogam.c to poslogam.exe
echo Compiling poslogam.c...
gcc poslogam.c -o poslogam.exe
if errorlevel 1 (
    echo Error compiling poslogam.c
    pause
    exit /b 1
)

REM Compile trnskrp.c to trnskrp.exe
echo Compiling trnskrp.c...
gcc trnskrp.c -o trnskrp.exe
if errorlevel 1 (
    echo Error compiling trnskrp.c
    pause
    exit /b 1
)

REM Compile mainffmpeg.c to mainffmpeg.exe
echo Compiling mainffmpeg.c...
gcc mainffmpeg.c -o mainffmpeg.exe
if errorlevel 1 (
    echo Error compiling mainffmpeg.c
    pause
    exit /b 1
)

REM Run poslogam.exe
echo Running poslogam...
poslogam.exe

REM Run trnskrp.exe
echo Running trnskrp...
trnskrp.exe

REM Instructions to the user
echo Edit output2.txt - test voice pitch do5
echo When ready, type done

:waitloop
set /p userinput=Enter command:
if /i "%userinput%"=="done" goto runmain
echo Waiting for "done" command...
goto waitloop

:runmain
REM Run mainffmpeg.exe
echo Running mainffmpeg...
mainffmpeg.exe

echo All operations completed.
pause
