@echo off

REM Main menu
:menu
echo.
echo Please choose an option:
echo 1. Close
echo 2. Restart the script
echo 3. Wipe build folder and restart
echo 4. Zip build folder and save as FE Build.zip
set /p choice=Enter your choice (1/2/3/4): 

if "%choice%"=="1" goto close
if "%choice%"=="2" goto restart
if "%choice%"=="3" goto wipeandrestart
if "%choice%"=="4" goto zipbuildfolder
echo Invalid choice. Please try again.
goto menu

:close
echo Goodbye!
exit

:restart
echo Restarting the script...
goto copyfiles

:wipeandrestart
echo Wiping all contents of the build folder...
if exist "build" (
    rmdir /S /Q "build"
    echo Build folder contents wiped successfully.
) else (
    echo Build folder does not exist, no need to wipe.
)
goto copyfiles

:zipbuildfolder
echo Preparing to zip build folder...

REM Check if build folder exists; if not, build it
if not exist "build" (
    echo Build folder does not exist. Creating and populating it now...
    goto copyfiles
)

REM Delete existing FE Build.zip if it exists
if exist "FE Build.zip" (
    echo FE Build.zip already exists. Deleting the old zip file...
    del /Q "FE Build.zip"
    echo Old FE Build.zip deleted.
)

REM Use PowerShell to create the ZIP file
PowerShell -Command "Compress-Archive -Path 'build\*' -DestinationPath 'FE Build.zip'"
echo Build folder has been successfully zipped into FE Build.zip!
goto menu

:end
REM Function to copy files and folders
:copyfiles
echo Starting to copy files...

REM Create the build folder if it doesn't exist
if not exist "build" (
    mkdir "build"
)

REM Read the directories from Build.cfg and copy them
if not exist "Build.cfg" (
    echo Error: Build.cfg not found. Please create the file with the required paths.
    goto menu
)

echo Reading directories from Build.cfg...

for /f "usebackq delims=" %%d in ("Build.cfg") do (
    echo Copying "%%d" to the build folder...
    if exist "%%d" (
        if exist "%%d\" (
            xcopy /E /I /Y "%%d" "build\%%~nd"
        ) else (
            copy /Y "%%d" "build\"
        )
        echo "%%d" copied successfully.
    ) else (
        echo Warning: "%%d" does not exist and will be skipped.
    )
)

echo All specified files and folders have been processed!
goto menu