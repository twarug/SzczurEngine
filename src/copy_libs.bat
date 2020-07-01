@echo off
cd ..
md "bin\x64\Debug"
md "bin\x64\Release"
cd src

::copy ffmpeg
echo Coping ffmpeg libraries
xcopy /Y "vendor\ffmpeg\bin\*.dll" "..\bin\x64\Debug\"
xcopy /Y "vendor\ffmpeg\bin\*.dll" "..\bin\x64\Release\"