#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <iomanip> //Para setw())
const int TAM_MAX_NOMBRE_ARCHIVO = 20;
const int TAM_LINEA = 100;
const int MAX_CELDA = 20;
const int MAX_NOMBRE=20;
const int MAX_APELLIDO=20;
using namespace std;
struct EstructuraReporte
{
    char *nombre_archivo;
    char *nombre;
    char *apellido;
    char *ciudad;
};
struct EstructuraInfoReporte
{
    double numero_semanas;
    double unidades_producidas;
    double unidades_vendidas;
    double utilidad_operacional;
    double utilidad_neta;
    EstructuraReporte *total_reportes;
};
struct CeldaSinCalcular
{
    char *nombre;
    char *dato;
};
struct CeldaCalculada
{
    char *nombre;
    double dato;
};
struct EstructuraHoja
{
    int filas;
    int columnas;
    CeldaSinCalcular **celdas;
    CeldaCalculada **celdas_calculadas;
};
struct EstructuraArchivo
{
    char *nombre_archivo;
    int numero_hojas;
    EstructuraHoja *hojas_sin_calcular;
    EstructuraInfoReporte *reportes;
};
void imprimir(char* texto);
char* getNombreArchivoHCalculo();
int contarLineas(char* nombre_archivo);
bool archivoPuedeUsarse(char* nombre_archivo);
bool textoVacio ( char* linea );
int leerNumeroDeHojasDeCalculo (char* nombre_archivo);
void imprimirLineas(char* nombre);
EstructuraHoja leerFilasColumnas(char * nombre_archivo, int numero_hoja);
int main()
{
    try //Este bloque lo que hace es intentar unas sentencias, en caso de ocurrir una excepción, pasa al bloque catch
    {
        EstructuraArchivo *archivo = new EstructuraArchivo; //Creo el archivo, que posteriormente puedo eliminar
        archivo->nombre_archivo = getNombreArchivoHCalculo(); //Aqui se pregunta por el nombre de el archivo (sin el .txt)
        ifstream archivo_hojas_calculo; //Abro el archivo de las hojas de cálculo
        archivo_hojas_calculo.open(archivo->nombre_archivo, ios::in); //Aquí queda abierto el archivo
        if(archivoPuedeUsarse(archivo->nombre_archivo)) // Pregunto si puede ser usado
        {
            archivo->numero_hojas = leerNumeroDeHojasDeCalculo(archivo->nombre_archivo); //Guardo el numero de hojas de calculo
            archivo->hojas_sin_calcular = new EstructuraHoja[archivo->numero_hojas]; //Creo un arreglo en la estructura que me ayudara a guardar la informacion de cada hoja de calculo
            /*//Se supone que esta linea es para hacer un debug y ver cuantas líneas realmente tiene el archivo y cuantas hojas de calculo
            //Tambien para imprimir el contenido del archivo
            cout<<"Este archivo "<<archivo->nombre_archivo<<" tiene "<<archivo->numero_hojas<<" hojas de calculo";
            cout<<" y tiene "<<contarLineas(archivo->nombre_archivo)<<" lineas."<<endl;
            cout<<endl<<setw(5)<<" "<<"CONTENIDO"<<endl;
            imprimirLineas(archivo->nombre_archivo);
            cout<<endl;*/
            // Este for lee el numero de filas y columnas del numero de hojas especificado
            for ( int i = 1; i <=archivo->numero_hojas; ++i)
                archivo->hojas_sin_calcular[i-1] = leerFilasColumnas(archivo->nombre_archivo, i);
            /*//Este for muestra la información capturada en el anterior
            for ( int i= 0; i <archivo->numero_hojas; ++i)
            {
                cout<<"Hoja de calculo numero "<<i+1<<endl;
                cout<<"Filas: "<<archivo->hojas_sin_calcular[i].filas<<"\tcolumnas: "<<archivo->hojas_sin_calcular[i].columnas<<endl;
            }*/
            // Significa que la última no está definida
            if (!archivo->hojas_sin_calcular[archivo->numero_hojas-1].filas)
                //Decimos que hubo un error
                throw 1;
            // Compruebo que no haya una hoja de más
            try
            {
                EstructuraHoja *hoja_de_descarte = new EstructuraHoja;
                *hoja_de_descarte=leerFilasColumnas(archivo->nombre_archivo, archivo->numero_hojas+1);
                //Este if es que significa que existe una hoja más
                if ((hoja_de_descarte->filas)&&(hoja_de_descarte->columnas))
                    throw 1;
            }
            catch(...)
            {
                throw 1;
            }
        }
        else //Por algun motivo, no puede usarse el archivo inicial, entonces se debe terminar la ejecución, solo hago que ocurra un error para que pase al bloque catch
            throw 1;
        return 0;
    }
    catch(...) //Pasó algo raro
    {
        cout<<endl<<setw(5)<<' '<<"Ha ocurrido un error con el archivo, por favor intentelo de nuevo...";
        return 1;
    }
}
bool archivoPuedeUsarse(char* nombre_archivo)
{
    // Me dice si inicialmente el archivo parece apto o no
    try //Este bloque lo que hace es intentar unas sentencias, en caso de ocurrir una excepción, pasa al bloque catch
    {
        ifstream archivo;
        archivo.open(nombre_archivo, ios::in); //Abro el archivo asociado al nombre que me pasaron
        return ((!archivo.is_open())||(archivo.fail())||(contarLineas(nombre_archivo)<4))?0:1;  //Digo si hay algún problema o si no es apto el archivo
    }
    catch (...)
    {
        return 0; //Como ocurre un error, devuelvo 0 para que no pueda usarse
    }
}
int contarLineas(char* nombre_archivo)
{
    //Me dice cuantas lineas hay en el archivo *Tener cuidado con el '\n'*
    ifstream archivo;
    archivo.open(nombre_archivo, ios::in);  //Abro el archivo asociado al nombre que me pasaron
    char * linea = new char [100];  //Para poder guardar la linea en alguna variable
    int contador = 0;   //Inicializo el contador de lineas
    while (!archivo.eof())
    {
        archivo.getline(linea, 100, '\n');  //Leo la linea
        if (!textoVacio(linea)) // Reviso que la linea no este vacía
            contador+=1;    //Añado uno a mi contador de lineas
    }
    delete linea;
    archivo.close();    //Cierro el archivo para que pueda usarse luego
    return contador;
}
char* getNombreArchivoHCalculo()
{
    char*error = new char [6];
    strcpy(error, "Error");
    try //Para asegurar que no ocurra ninguna clase de error
    {
        //Esta función retorna un char* con el nombre del archivo que tiene la(s) hoja(s) de calculo
        char*nombre = new char[TAM_MAX_NOMBRE_ARCHIVO+5];
        cout<<"Nombre del archivo que contiene las hojas de calculo: ";
        cin.getline(nombre, TAM_MAX_NOMBRE_ARCHIVO, '\n');
        char*txt = new char [5];
        strcpy(txt, ".txt");
        strcat(nombre, txt);
        return nombre;
    }
    catch(...)
    {
        return error;
    }
}
bool textoVacio ( char* linea )
{
    return (!strlen(linea)); // Si esta vacia la linea (Sin caracteres), me retornara 1, caso contrario me retorna 0
}
int leerNumeroDeHojasDeCalculo (char* nombre_archivo)
{
    try //Este bloque lo que hace es intentar unas sentencias, en caso de ocurrir una excepción, pasa al bloque catch
    {
        ifstream archivo;
        archivo.open(nombre_archivo, ios::in); //Abro el archivo asociado al nombre que me pasaron
        char*nhojas = new char[2]; //Hago espacio para almacenar el número
        archivo.getline(nhojas, 2, '\n');
        int num = atoi(nhojas);
        delete nhojas;
        return num;
    }
    catch (...)
    {
        return 0; //Como ocurre un error, devuelvo 0 para que no pueda usarse
    }
}
EstructuraHoja leerFilasColumnas(char * nombre_archivo, int numero_hoja)
{
    EstructuraHoja archivoE;
    archivoE.filas = 0;
    archivoE.columnas = 0;
    int hactual;
    try //Este bloque lo que hace es intentar unas sentencias, en caso de ocurrir una excepción, pasa al bloque catch
    {
        ifstream archivo;
        archivo.open(nombre_archivo, ios::in); //Abro el archivo asociado al nombre que me pasaron
        char*linea = new char[100]; //Hago espacio para almacenar la linea
        archivo.getline(linea, 100, '\n');    //Leo la linea que me indica el numero de columnas
        hactual = 1;
        while (hactual!=numero_hoja) // Para buscar  la hoja de la cual necesito la info
        {
            archivo.getline(linea, 100, '\n'); //Capturo la linea que contiene la info de lo hoja numero hactual
            char *puntero = strtok(linea, " "); // Separo la linea en tokens
            int *i = new int;
            *i = 1; // Uno columnas, dos filas
            while (puntero != NULL)
            {
                if (*i == 1)
                    archivoE.columnas = atoi(puntero);
                else if (*i == 2)
                    archivoE.filas =atoi(puntero);
                else throw 0; // Si hay algún fallo
                ++*i;
                puntero=strtok(NULL, " ");              
            }
            *i = 0;
            for ( int j = 1; j <= archivoE.filas; j++)//Paso a la siguiente hoja
            {
                archivo.getline(linea, 100, '\n');
            }
            hactual++;
            delete puntero;
            delete i;
        }
        archivo.getline(linea, 100, '\n');
        if(textoVacio(linea))
            throw 1;
        char *puntero = strtok(linea, " "); // Separo la linea en tokens
        int *i = new int;
        *i = 1; // Uno columnas, dos filas
        while (puntero != NULL)
        {
            if (*i == 1)
                archivoE.columnas = atoi(puntero);
            else if (*i == 2)
                archivoE.filas =atoi(puntero);
            else throw 0; // Si hay algún fallo
            ++*i;
            puntero=strtok(NULL, " ");              
        }
        delete puntero;
        delete i;
        return archivoE;
    }
    catch (...)
    {
        archivoE.filas = 0;
        archivoE.columnas = 0;
        return archivoE; //Como ocurre un error, devuelvo 0 en ambos casos para saber y que no pueda usarse
    }
}
void imprimir(char* texto) //Si no está vacía, imprime el texto
{
    if (!textoVacio(texto))
    {
        cout<<texto<<endl;
    }
}
void imprimirLineas(char* nombre) //Imprime todas las lineas de un archivo
{
    ifstream archivo;
    archivo.open(nombre, ios::in);
    char*line =new char[100];
    while(!archivo.eof())
    {
        archivo.getline(line, 100, '\n');
        imprimir(line);
    }
}
