#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#define CTD_PAISES 5
#define CTD_LINEAS 8
#define CTD_AEROPUERTOS 15

/*
    Struct auxiliar para poder utilizarlo en funciones
    fuera de main sin tener que pasar muchos argumentos.
*/
struct Contexto
{
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
    Algoritmo de ordenamiento de inserción de forma ascendente
    Admite tambien un parametro de indices, para poder
    controlar donde cambio cada uno
*/
void arr_insertion_sort (int* arr, size_t n, int* indices)
{
    for (size_t i = 0; i < n; i++) {
        int k = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > k)
        {
            arr[j+1] = arr[j];
            indices[j+1] = indices[j];
            j--;
        }
            
        arr[j+1] = k;
        indices[j+1] = i;
    }
}

void arr_rev (int* arr, size_t n)
{
    for (size_t i = 0; i < n/2; i++)
    {
        int a = arr[i];
        size_t c = 7 - 1 - i;

        arr[i] = arr[c];
        arr[c] = a;
    }
}

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

    for (size_t i = 0; i < CTD_PAISES; i++)
    {
        std::getline(arc_nombres, input);
        contexto->paises[i] = input;
    }
    for (size_t i = 0; i < CTD_LINEAS; i++)
    {
        std::getline(arc_nombres, input);
        contexto->lineas[i] = input;
    }
    for (size_t i = 0; i < CTD_AEROPUERTOS; i++)
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
    for (size_t i = 0; i < c_arribos; i++)
    {
        Arribo* arribo = &arribos[i];
        pasajeros[arribo->pais][arribo->linea] += arribo->pasajeros;
    }
}

void informe_pasajeros_diario (
    Arribo* arribos, 
    size_t c_arribos,
    int pais,
    int linea,
    int pasajeros[7]
) {
    for (size_t i = 0; i < c_arribos; i++)
    {
        Arribo arribo = arribos[i];
        if (arribo.pais != pais || arribo.linea != linea)
            continue;
            
        pasajeros[arribo.dia] += arribo.pasajeros;
    }
}

/*
    Obtiene la cantidad de pasajeros que arribaron a cada
    aeropuerto ademas de la cantidad de vuelos

    * Nota: preferiria utilizar un hashmap en vez de tener un lookup
    O(n^2), pero no se si es que esta permitido. Dejo una mejor solución para otro dia
*/
void total_pasajeros_aeropuerto_semanales (
    Arribo* arribos,
    size_t c_arribos,
    std::string codigos[CTD_AEROPUERTOS],
    int pasajeros[CTD_AEROPUERTOS],
    int vuelos[CTD_AEROPUERTOS]
) {
    for (int i = 0; i < c_arribos; i++)
    {
        Arribo* arribo = &arribos[i];

        for (int j = 0; j < CTD_AEROPUERTOS; j++)
            if (codigos[j] == (arribo->iata))
            {
                pasajeros[j] += arribo->pasajeros;
                vuelos[j]++;
            }
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
    */
    {
        int pasajeros[CTD_PAISES][CTD_LINEAS] {};
        total_pasajeros_semanales(arribos, ctd_arribos, pasajeros); 

        int padding_pais = 10;
        int padding_lineas = 25;
        int padding_pasajeros = 25;

        std::cout << "===== INFORME DE PASAJEROS =====\n";
        std::cout << std::left << std::setfill(' ')
                  << std::setw(padding_pais) << "Pais"
                  << std::setw(padding_lineas) << "Linea"
                  << std::setw(padding_pasajeros) << "Pasajeros" << "\n";

        for (int pais = 0; pais < CTD_PAISES; pais++)
            for (int linea = 0; linea < CTD_LINEAS; linea++)
                std::cout << std::left << std::setfill(' ')
                          << std::setw(padding_pais) << contexto.paises[pais] 
                          << std::setw(padding_lineas) << contexto.lineas[linea]
                          << std::setw(padding_pasajeros) << pasajeros[pais][linea] << "\n";
    }

    /*
        Nota: las siguientes funciones tienen busqueda de maximo/minimo, que deben
        ser movidas a funciones propias.
    */

    /*
        PTO 4
    */
    {
        int total_pasajeros[CTD_AEROPUERTOS] {};
        int total_vuelos[CTD_AEROPUERTOS] {};
        total_pasajeros_aeropuerto_semanales(arribos, ctd_arribos, contexto.iata, total_pasajeros, total_vuelos);

        int min_pasajeros = 100000000;
        int min_aeropuerto = 0;

        for (int aeropuerto = 0; aeropuerto < CTD_AEROPUERTOS; aeropuerto++)
        {
            int pasajeros = total_pasajeros[aeropuerto];

            if (pasajeros > 0 && pasajeros < min_pasajeros)
            {
                min_pasajeros = pasajeros;
                min_aeropuerto = aeropuerto;
            }
        }
        
        std::cout << "\n";
        std::cout << "La aerolinea con menos pasajeros fue " << contexto.aeropuertos[min_aeropuerto] << "\n";
        std::cout << "Tuvo " << min_pasajeros << " pasajeros en " << total_vuelos[min_aeropuerto] << " vuelos\n";
    }

    /*
        PTO 5
    */
   {
        /* 
            Se podria reutilizar la misma estructura cargada del PTO 3, pero
            decidi no hacerlo por si es que despues muevo cada uno a una
            función unica.
        */
        int pasajeros[CTD_PAISES][CTD_LINEAS] {};
        total_pasajeros_semanales(arribos, ctd_arribos, pasajeros);

        int max_pasajeros = 0;

        int max_pais = 0;
        int max_linea = 0;

        for (int pais = 0; pais < CTD_PAISES; pais++)
            for (int linea = 0; linea < CTD_LINEAS; linea++)
            {
                int combinacion_pasajeros = pasajeros[pais][linea];

                if (combinacion_pasajeros > max_pasajeros)
                {
                    max_pasajeros = combinacion_pasajeros;
                    max_pais = pais;
                    max_linea = linea;
                }
            }
        
        std::cout << "\n";
        std::cout << "La combinación con mayor pasajeros (" << max_pasajeros << ") fue " << contexto.paises[max_pais] << " " << contexto.lineas[max_linea] << "\n";

        int pasajeros_diarios[7] {};
        int indices[7] {};

        informe_pasajeros_diario(arribos, ctd_arribos, max_pais, max_linea, pasajeros_diarios);

        /* Ordenamiento y reversa */
        arr_insertion_sort(pasajeros_diarios, 7, indices);
        arr_rev(indices, 7);
        arr_rev(pasajeros_diarios, 7);

        std::string dias[7] { "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo" };
        for (int i = 0; i < 7; i++)
            std::cout << std::left << std::setfill(' ')
                      << std::setw(10) << dias[indices[i]]
                      << pasajeros_diarios[i] << "\n";
   }
}
