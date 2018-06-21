IF EXIST "./Release/" (
    RMDIR /S /Q "./Release"
)

IF EXIST "C:\Program Files (x86)\Steam\steamapps\common\Aseprite\aseprite.exe" (
    CALL ImageRelease.bat
)

MKDIR "./Release"

COPY "Debug\Platformer.exe" "Release\Platformer.exe"

XCOPY /E /V "Platformer\Assets" "Release\Assets\"
XCOPY /E /V "Platformer\Saves" "Release\Saves\"

DEL Release\Assets\Images\Damsels\*.png

DEL Release\Saves\*.plSAV
DEL Release\Saves\LastSave.txt
DEL Release\Saves\Progress.plPRG
DEL Release\Saves\Preferences.ini

XCOPY /E /V "Clean" "Release\Saves\"

COPY Platformer\*.dll Release\*.dll
COPY Platformer\*.lib Release\*.lib
