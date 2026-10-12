#include <iostream>
#include <fstream>
#include <string>

#define CTD_NOMBRES 5
#define CTD_LINEAS 8
#define CTD_AEROPUERTOS 15

struct Arribo
{
    int pais;
    int linea;
    int pasajeros;
    int dia;
    std::string iata;
};

/*
    Funcion auxiliar para leer un string desde un stream.
*/
std::string leer_string(std::ifstream* stream, size_t n)
{
    std::string str(n, '-');
    stream->read(&str[0], n);
}

int main ()
{
    /* Carga de arribos */
    Arribo arribos[2048];

    std::ifstream arc_arribos ("datos/arribos.bin", std::ios_base::binary);;
    if (!arc_arribos)
    {
        std::cout << "Hubo un error abriendo el archivo de nombres.\n";
        return -1;
    }

    int ctd_arribos = 0;
    while (!arc_arribos.eof())
    {
        Arribo arribo;

        arc_arribos.read((char*)&arribo.pais, sizeof(int));
        arc_arribos.read((char*)&arribo.linea, sizeof(int));
        arribo.iata = leer_string(&arc_arribos, 3);
        arc_arribos.read((char*)&arribo.pasajeros, sizeof(int));
        arc_arribos.read((char*)&arribo.dia, sizeof(int));

        arribos[ctd_arribos] = arribo;
        ctd_arribos++;
    }

    /* Carga de datos aux (nombres, codigos, etc.) */

    /* Mapeo int -> nombre */
    std::string nombres[CTD_NOMBRES] {};
    std::string lineas[CTD_LINEAS] {};
    std::string aeropuertos[CTD_AEROPUERTOS] {};
    std::string iata[CTD_AEROPUERTOS] {};

    std::ifstream arc_nombres ("datos/nombres.txt");;
    if (!arc_nombres)
    {
        std::cout << "Hubo un error abriendo el archivo de nombres.\n";
        return -1;
    }

    /* 
        Utilizamos getline porque si utilizamos el operador >>, este ignora
        los espacios en blanco en vez de unicamente los newline.
    */

    std::string input;
    for (int i = 0; i < CTD_NOMBRES; i++)
    {
        std::getline(arc_nombres, input);
        nombres[i] = input;
    }

    for (int i = 0; i < CTD_LINEAS; i++)
    {
        std::getline(arc_nombres, input);
        lineas[i] = input;
    }

    for (int i = 0; i < CTD_AEROPUERTOS; i++)
    {
        std::getline(arc_nombres, input);

        iata[i] = input.substr(0, 3); // Los codigos IATA son de 3 caracteres.
        aeropuertos[i] = input.substr(4, input.length() - 4);
    }
}