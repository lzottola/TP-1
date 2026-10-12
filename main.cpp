#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#define CTD_PAISES 5
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
    std::string paises[CTD_PAISES];
    std::string lineas[CTD_LINEAS];
    std::string aeropuertos[CTD_AEROPUERTOS];
    std::string iata[CTD_AEROPUERTOS]; /* Capaz es innecesario porque el codigo ya esta dentro del arribo */
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

    return str;
}

/*
    Carga de arribos.
    Devuelve la cantidad de arribos cargados (hasta que decida cambiarlo
    por una linked list o un vector)
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

    return ctd_arribos;
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

    for (int i = 0; i < CTD_PAISES; i++)
    {
        std::getline(arc_nombres, input);
        contexto->paises[i] = input;
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

/*
    Obtiene la cantidad de pasajeros semanales dado
    un arreglo de arribos.
*/
void total_pasajeros_semanales (
    Arribo* arribos, 
    size_t c_arribos, 
    int pasajeros[CTD_PAISES][CTD_LINEAS]
) {
    for (int i = 0; i < c_arribos; i++)
    {
        Arribo* arribo = &arribos[i];
        pasajeros[arribo->pais][arribo->linea] += arribo->pasajeros;
    }
}

/*
    Obtiene la cantidad de pasajeros que arribaron a cada
    aeropuerto

    * Nota: preferiria utilizar un hashmap en vez de tener un lookup
    O(n^2), pero no se si es que esta permitido. Dejo una mejor solución para otro dia
*/
void total_pasajeros_aeropuerto_semanales (
    Arribo* arribos,
    size_t c_arribos,
    std::string codigos[CTD_AEROPUERTOS],
    int pasajeros[CTD_AEROPUERTOS]
) {
    for (int i = 0; i < c_arribos; i++)
    {
        Arribo* arribo = &arribos[i];

        for (int j = 0; j < CTD_AEROPUERTOS; j++)
            if (codigos[j] == (arribo->iata))
                pasajeros[j] += arribo->pasajeros;
    }
}

void total_vuelos_aeropuerto (
    Arribo* arribos, 
    size_t c_arribos, 
    int total_vuelos[CTD_AEROPUERTOS]
) {
    for (int i = 0; i < c_arribos; i++)
    {
        Arribo* arribo = &arribos[i];
        total_vuelos[arribo->linea]++;
    }
}

int main ()
{
    size_t ctd_arribos {};
    Arribo arribos[2048] {};
    Contexto contexto {};

    ctd_arribos = cargar_arribos(arribos);

    if (!ctd_arribos)
    {
        std::cout << "Hubo un error al cargar los arribos\n";
        return -1;
    };
    if (!cargar_contexto(&contexto))
    {
        std::cout << "Hubo un error al cargar el contexto\n";
        return -1;
    };

    /*
        PTO 3

        Preferi separar el total de pasajeros por pais/linea y la cantidad
        de vuelos para un aeropuerto en dos funciones distintas debido
        a que siento que es mas modular comparado a hacer todo
        en una misma funcion monolitica.
    */
    {
        int pasajeros[CTD_PAISES][CTD_LINEAS] {};
        total_pasajeros_semanales(arribos, ctd_arribos, pasajeros);

        for (int pais = 0; pais < CTD_PAISES; pais++)
            for (int linea = 0; linea < CTD_LINEAS; linea++)
                std::cout << contexto.paises[pais] << "  " << contexto.lineas[linea] << "  " << pasajeros[pais][linea] << "\n";
    }

    /*
        PTO 4

        Por ahora, utilizo un hashmap debido a que es muchisimo mas conveniente
        mapear cada codigo a su aeropuerto en vez de hacer cualquier otra locura
    */
    {
        int pasajeros[CTD_AEROPUERTOS] {};
        total_pasajeros_aeropuerto_semanales(arribos, ctd_arribos, contexto.iata, pasajeros);

        int min_pasajeros = 100000000;
        int min_aeropuerto = 0;
        for (int aeropuerto = 0; aeropuerto < CTD_AEROPUERTOS; aeropuerto++)
        {
            int pasajeros_aeropuerto = pasajeros[aeropuerto];

            if (pasajeros_aeropuerto > 0 && pasajeros_aeropuerto < min_pasajeros)
            {
                min_pasajeros = pasajeros_aeropuerto;
                min_aeropuerto = aeropuerto;
            }
        }
        
        std::cout << "La aerolinea con menos pasajeros fue " << contexto.aeropuertos[min_aeropuerto] << "\n";
    }
}
