@set ASESPRITE="C:\Program Files (x86)\Steam\steamapps\common\Aseprite\aseprite.exe"
@set IMAGE_DIR=.\Platformer\Assets\Images\Ase\
@set PIECES_DIR=.\Platformer\Assets\Images\Pieces\

@set HEROINE_WIDTH=432
@set HEROINE_HEIGHT=270
@set KO_WIDTH=216
@set KO_HEIGHT=378

@set ESCAPE_WIDTH=64
@set ESCAPE_HEIGHT=72

@set HAIR=(Pixie,Spiky,Ponytail,Bob,Straight)
@set COSTUMES=(Dress,Harem,Bunny,Catsuit,Casual,Workout,Maid,Lingerie,Swimsuit)
@set WEAPONS=(Base,Handcannon,Phaser,Magnum,Uzi)
@set LEVELS=(Alien,Mummy,Factory,Tropic)

REM Heroine Sheet

%ASESPRITE% -b --layer "Body" --layer "Arms" %IMAGE_DIR%Heroine.aseprite --sheet %PIECES_DIR%Base.png --sheet-width %HEROINE_WIDTH% --sheet-height %HEROINE_HEIGHT% --data dump.json

FOR %%L IN %HAIR% DO %ASESPRITE% -b --layer "Hair/%%L" %IMAGE_DIR%Heroine.aseprite --sheet %PIECES_DIR%%%L.png --sheet-width %HEROINE_WIDTH% --sheet-height %HEROINE_HEIGHT% --data dump.json
FOR %%L IN %COSTUMES% DO %ASESPRITE% -b --layer "Outfits/%%L" %IMAGE_DIR%Heroine.aseprite --sheet %PIECES_DIR%%%L.png --sheet-width %HEROINE_WIDTH% --sheet-height %HEROINE_HEIGHT% --data dump.json
FOR %%L IN %WEAPONS% DO %ASESPRITE% -b --layer "Weapons/%%L" %IMAGE_DIR%Heroine.aseprite --sheet %PIECES_DIR%%%LWeapon.png --sheet-width %HEROINE_WIDTH% --sheet-height %HEROINE_HEIGHT% --data dump.json
FOR %%L in %LEVELS% DO %ASESPRITE% -b --layer "Bonds/%%L" %IMAGE_DIR%Heroine.aseprite --sheet %PIECES_DIR%%%LBonds.png --sheet-width %HEROINE_WIDTH% --sheet-height %HEROINE_HEIGHT% --data dump.json

REM KO Sheet

FOR %%L IN %LEVELS% DO %ASESPRITE% -b --layer "%%LBondsKO" %IMAGE_DIR%HeroineKO.ase --sheet %PIECES_DIR%%%LBondsKO.png --sheet-width %KO_WIDTH% --sheet-height %KO_HEIGHT% --data dump.json
FOR %%L IN %LEVELS% DO %ASESPRITE% -b --layer "%%LBondsForeKO" %IMAGE_DIR%HeroineKO.ase --sheet %PIECES_DIR%%%LBondsForeKO.png --sheet-width %KO_WIDTH% --sheet-height %KO_HEIGHT% --data dump.json
%ASESPRITE% -b --layer "Body" --layer "Arms" %IMAGE_DIR%HeroineKO.ase --sheet %PIECES_DIR%BaseKO.png --sheet-width %KO_WIDTH% --sheet-height %KO_HEIGHT% --data dump.json
%ASESPRITE% -b --layer "Faux" %IMAGE_DIR%HeroineKO.ase --sheet %PIECES_DIR%FauxKO.png --sheet-width %KO_WIDTH% --sheet-height %KO_HEIGHT% --data dump.json

FOR %%L IN %HAIR% DO %ASESPRITE% -b --layer "Hair/%%L" %IMAGE_DIR%HeroineKO.ase --sheet %PIECES_DIR%%%LKO.png --sheet-width %KO_WIDTH% --sheet-height %KO_HEIGHT% --data dump.json
FOR %%L IN %COSTUMES% DO %ASESPRITE% -b --layer "Outfits/%%L" %IMAGE_DIR%HeroineKO.ase --sheet %PIECES_DIR%%%LKO.png --sheet-width %KO_WIDTH% --sheet-height %KO_HEIGHT% --data dump.json

FOR %%L IN %LEVELS% DO (%ASESPRITE% -b --layer "%%L/Body" %IMAGE_DIR%EscapeBar.ase --sheet %PIECES_DIR%%%LEscapeBarBase.png --sheet-width %ESCAPE_WIDTH% --sheet-height %ESCAPE_HEIGHT% --data dump.json & %ASESPRITE% -b --layer "%%L/Fill" %IMAGE_DIR%EscapeBar.ase --sheet %PIECES_DIR%%%LEscapeBarFill.png --sheet-width %ESCAPE_WIDTH% --sheet-height %ESCAPE_HEIGHT% --data dump.json & FOR %%H in %HAIR% DO %ASESPRITE% -b --layer "%%L/%%H" %IMAGE_DIR%EscapeBar.ase --sheet %PIECES_DIR%%%LEscapeBar%%H.png --sheet-width %ESCAPE_WIDTH% --sheet-height %ESCAPE_HEIGHT% --data dump.json)

DEL dump.json
