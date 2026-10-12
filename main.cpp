#include <iostream>
#include <fstream>
#include <string>

#define CTD_NOMBRES 5
#define CTD_LINEAS 8
#define CTD_AEROPUERTOS 15

int main ()
{
    std::string* nombres {};
    std::string* lineas {};
    std::string* iata {};
    std::string* aeropuertos {};

    /* Carga de datos aux (nombres, codigos, etc.) */
    std::ifstream archivo ("datos/nombres.txt");;
    if (!archivo)
    {
        std::cout << "Hubo un error abriendo el archivo nombres.txt\n";
        return -1;
    }
}