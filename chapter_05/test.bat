@echo off
cl /EHsc 5-18.c /Femsvc.exe
cl /EHsc 5-18.c /O2 /Femsvco2.exe
msvc.exe
msvco2.exe

clang 5-18.c -o llvm.exe
clang 5-18.c -O3 -o llvmo2.exe
llvm.exe
llvmo2.exe

gcc 5-18.c -o gnu.exe
gcc 5-18.c -O3 -o gnuo2.exe
gnu.exe
gnuo2.exe

del *.exe
del *.obj