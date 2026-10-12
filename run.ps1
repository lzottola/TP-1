<# Script auxiliar para compilar el codigo #>

param(
    [switch]$run
)

& g++ -std=c++17 .\main.cpp  -o tp1.exe

if ($run)
{
    & .\tp1.exe
}
