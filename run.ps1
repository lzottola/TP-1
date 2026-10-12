<# Script auxiliar para compilar el codigo #>

param(
    [switch]$run
)

& g++ -std=c++17 .\main.cpp  -o tp1.exe
& .\tp1.exe
