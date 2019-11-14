@echo off
cmake -B"BUILD/VS2019/%3" -H"." -G"Visual Studio 16" -A x64
pause
