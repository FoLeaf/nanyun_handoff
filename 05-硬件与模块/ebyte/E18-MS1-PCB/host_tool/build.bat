@echo off
echo === E18-MS1-PCB Host Tool Build Script ===
echo.

:: Try MSVC first
where cl.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo [MSVC] Compiling...
    cl /O2 /W3 /Fe:E18-HostTool.exe main.c /link user32.lib gdi32.lib comctl32.lib comdlg32.lib
    if %errorlevel% equ 0 (
        echo [MSVC] Build OK: E18-HostTool.exe
        goto :done
    )
)

:: Try MinGW
where gcc.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo [MinGW] Compiling...
    gcc -O2 -Wall -municode -o E18-HostTool.exe main.c -lcomctl32 -lcomdlg32 -lgdi32
    if %errorlevel% equ 0 (
        echo [MinGW] Build OK: E18-HostTool.exe
        goto :done
    )
)

echo [ERROR] No compiler found. Install MSVC or MinGW-w64.
pause
exit /b 1

:done
echo.
echo Run: E18-HostTool.exe
pause
