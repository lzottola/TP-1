/*
    TP°1

    Profesor: Eduardo Zuñiga
    Curso y comisión: AyED K1041

    Realizado por:
        * Luca Patricio Zottola - lzottola@frba.utn.edu.ar
*/

#include <iostream>
#include <fstream>
#include <string>

#define CTD_NOMBRES 5
#define CTD_LINEAS 8
#define CTD_AEROPUERTOS 15

/*
    Struct auxiliar para poder utilizarlo en funciones
    fuera de main sin tener que pasar muchos argumentos.
*/
struct Contexto
{
    /* 
        C++ no permite establecer arrays, entonces
        los convertimos en punteros que son lo mismo
        de cualquier forma
    */
    std::string* nombres;
    std::string* lineas;
    std::string* aeropuertos;
    std::string* iata;
};

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
std::string leer_string (std::ifstream* stream, size_t n)
{
    std::string str(n, '-');
    stream->read(&str[0], n);
}

/*
    Carga de arribos
*/
int cargar_arribos (Arribo* arribos)
{
    std::ifstream arc_arribos ("datos/arribos.bin", std::ios_base::binary);;
    if (!arc_arribos)
        return 0;

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

    return 1;
}

/*
    Realiza la carga de contexto (nombres, pais, etc.)
*/
int cargar_contexto (Contexto* contexto)
{
    std::ifstream arc_nombres ("datos/nombres.txt");;
    if (!arc_nombres)
        return 0;

    /* 
        Utilizamos getline porque si utilizamos el operador >>, este ignora
        los espacios en blanco en vez de unicamente los newline.
    */
    std::string input;

    for (int i = 0; i < CTD_NOMBRES; i++)
    {
        std::getline(arc_nombres, input);
        contexto->nombres[i] = input;
    }
    for (int i = 0; i < CTD_LINEAS; i++)
    {
        std::getline(arc_nombres, input);
        contexto->lineas[i] = input;
    }
    for (int i = 0; i < CTD_AEROPUERTOS; i++)
    {
        std::getline(arc_nombres, input);

        contexto->iata[i] = input.substr(0, 3); // Los codigos IATA son de 3 caracteres.
        contexto->aeropuertos[i] = input.substr(4, input.length() - 4);
    }

    return 1;
}

int main ()
{
    Arribo arribos[2048] {};
    Contexto contexto {};

    if (!cargar_arribos(arribos))
    {
        std::cout << "Hubo un error al cargar los arribos\n";
        return -1;
    };
    if (!cargar_contexto(&contexto))
    {
        std::cout << "Hubo un error al cargar el contexto\n";
        return -1;
    };
}
