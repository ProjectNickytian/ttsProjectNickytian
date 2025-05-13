@echo off
setlocal enabledelayedexpansion
for %%f in (*.wav) do (
  set "filename=%%~nf"
  set "newname=!filename:'=-!.wav"
  if not "!filename!"=="!newname:.wav=!" ren "%%f" "!newname!"
)
endlocal


pause