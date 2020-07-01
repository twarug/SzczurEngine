@echo off
cd ..
md "test"
cd src

echo Coping assets
xcopy /Y /E /D "Engine\SzczurEngine\out\*" "..\test\"
