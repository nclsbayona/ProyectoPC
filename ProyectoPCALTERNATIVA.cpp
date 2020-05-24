#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <sstream>
#include <cmath>
#include <queue>
#include <stack>
#include <iomanip> //Para setw()
const int TAM_MAX_NOMBRE_ARCHIVO = 20;
const int TAM_LINEA = 100;
const int MAX_CELDA = 20;
const int MAX_NOMBRE_CELDA = 7;
const int MAX_NOMBRE=25;
const int MAX_APELLIDO=20;
using namespace std;
struct EstructuraReporte
{
    char *nombre_archivo; //El nombre del archivo donde se guardará un reporte en específico
    char *nombre; //El nombre del interesado
    char *apellido; //Apellido del interesado
    char *ciudad;//Ciudad donde se ubica el reporte
};
struct EstructuraInfoReporte
{
    //Info para el reporte
    double numero_semanas; 
    double unidades_producidas;
    double unidades_vendidas;
    double utilidad_operacional;
    double utilidad_neta;
    int nreportes;
    EstructuraReporte *total_reportes;
};
struct CeldaSinCalcular
{
    char *nombre; //A...Z,AA...ZZ,AAA...ZZZ
    char *dato; //Valor
};
struct CeldaCalculada
{
    char *nombre; //A...Z,AA...ZZ,AAA...ZZZ
    double dato;//Valor
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
    EstructuraInfoReporte reporte;
};
void imprimirHojaC(EstructuraHoja hoja);
bool  llenarEstructuraReporte (EstructuraReporte &reporte, char*linea, int xs, EstructuraInfoReporte &data);
bool HojasCalculo();
void imprimir(char* texto);
char* getNombreArchivoHCalculo();
int contarLineas(char* nombre_archivo);
bool archivoPuedeUsarse(char* nombre_archivo);
bool textoVacio ( char* linea );
int leerNumeroDeHojasDeCalculo (char* nombre_archivo);
void imprimirLineas(char* nombre);
EstructuraHoja leerFilasColumnas(char * nombre_archivo, int numero_hoja);
bool leerCeldas(EstructuraArchivo  archivo);
bool calcularCeldas(EstructuraArchivo  archivo);
bool generarReportes(EstructuraArchivo &archivo);
char* getNombreArchivoInteresadosR();
char* generarNombreCelda(int posicion, int fila);
void reverse(char str[], int length) ;
void iniciarPuntero(char*palabra, int tam_max);
char* itoa(int num, char* str, int base) ;
bool tieneNumero(char*dato);
void imprimirHoja(EstructuraHoja hoja);
bool calcularCeldaFormula(EstructuraHoja hoja, char*nombre_celda, int repeticion);
char* extraerDato(EstructuraHoja hoja, char*nombre_celda);
bool llenarInfoReportes(EstructuraArchivo &archivo);
bool llenarTotalReportes(EstructuraArchivo &archivo);
void eliminarPunteroEstructuraArchivo(EstructuraArchivo &archivo);
int main()
{
   return HojasCalculo();
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
        for(int i=0; i<(int)(strlen(nombre));i++)
            if(nombre[i]==' ')
                nombre[i]='_';
        char*txt = new char [5];
        strcpy(txt, ".txt");
        strcat(nombre, txt);
        delete error;
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
        char*nhojas = new char[5]; //Hago espacio para almacenar el número
        archivo.getline(nhojas, 5, '\n');
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
        delete linea;
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
    delete line;
}
bool HojasCalculo()
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
                delete hoja_de_descarte;
                for ( int i = 1; i <=archivo->numero_hojas; ++i)
                {
                    archivo->hojas_sin_calcular[i-1].celdas = new CeldaSinCalcular*[archivo->hojas_sin_calcular[i-1].columnas];
                    for (int j = 0; j < archivo->hojas_sin_calcular[i-1].filas; j++)
                      archivo->hojas_sin_calcular[i-1].celdas[j] = new CeldaSinCalcular[archivo->hojas_sin_calcular[i-1].columnas];
                }
                if(leerCeldas(*archivo))
                    throw 1;
                if(calcularCeldas(*archivo))
                    throw 1;
                /*for(int w =0; w < archivo->numero_hojas;w++)
                    imprimirHojaC(archivo->hojas_sin_calcular[w]);*/
                if(generarReportes(*archivo))
                    throw 1;
                if(llenarInfoReportes(*archivo))
                    throw 1;
                if (llenarTotalReportes(*archivo))
                    throw 1;
                eliminarPunteroEstructuraArchivo(*archivo);
            }
            catch(...)
            {
                eliminarPunteroEstructuraArchivo(*archivo);
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
bool leerCeldas(EstructuraArchivo  archivoE)
{
    try //Este bloque lo que hace es intentar unas sentencias, en caso de ocurrir una excepción, pasa al bloque catch
    {
        ifstream archivo;
        archivo.open(archivoE.nombre_archivo, ios::in); //Abro el archivo asociado al nombre
        char*linea = new char[100]; //Hago espacio para almacenar la linea
        archivo.getline(linea, 100, '\n');    //Leo la linea que me indica el numero de hojas
        int i = 0; //Contador de columnas
        for (int j=0;j<archivoE.numero_hojas; j++)
        {
            //cout<<"Hoja "<<j+1<<endl;
            archivo.getline(linea, 100, '\n');    //Leo la linea que me indica el numero de filas y columnas
            for (int w=0;w<archivoE.hojas_sin_calcular[j].filas;w++)
            {
                i = 0; //Contador de columnas
                archivo.getline(linea, 100, '\n');    //Leo la linea de la fila
                //cout<<linea<<endl;
                //cout<<"Fila: "<<w+1<<endl;
                char *puntero = strtok(linea, " "); // Separo la linea en tokens
                try
                {
                    while ((puntero != NULL) && (i<archivoE.hojas_sin_calcular[j].columnas))
                    {
                        archivoE.hojas_sin_calcular[j].celdas[w][i].nombre=new char[MAX_CELDA];
                        strcpy(archivoE.hojas_sin_calcular[j].celdas[w][i].nombre, generarNombreCelda(i, w));
                        archivoE.hojas_sin_calcular[j].celdas[w][i].dato = new char[MAX_CELDA];
                        strcpy(archivoE.hojas_sin_calcular[j].celdas[w][i].dato, puntero);
                        puntero=strtok(NULL, " ");
                        //cout<<"\tDato "<< archivoE.hojas_sin_calcular[j].celdas[w][i].dato <<" nombre "<<archivoE.hojas_sin_calcular[j].celdas[w][i].nombre<<endl;
                        i++;
                    }
                    if ((i != archivoE.hojas_sin_calcular[j].columnas)||(puntero!=NULL))
                    {
                        throw 2;
                    }
                }
                catch(...)
                {
                    return 1; //Error
                }
            delete puntero;
            }
        }
        return 0;
    }
    catch (...)
    {
        return 1;
    }
}
char* palabraAMinusculas(char*palabraNormal)
{
    int x=strlen(palabraNormal);
    char*palabraMinusculas=new char[x+1];
    iniciarPuntero(palabraMinusculas, x+1);
    for (int i=0; i<(int)(x); i++)
    {
        palabraMinusculas[i]=' ';
        if (palabraNormal[i]<97)
            palabraMinusculas[i]=palabraNormal[i]+32;
        else
            palabraMinusculas[i]=palabraNormal[i];
    }
    return palabraMinusculas;
}
void llenarDinamico(char*&palabra)
{
    int t=strlen(palabra);
    char*nuevapalabra=new char[t+1];
    iniciarPuntero(nuevapalabra, t+1);
    strcpy(nuevapalabra, palabra);
    delete palabra;
    palabra=nuevapalabra;
    return;
}
bool  llenarEstructuraReporte (EstructuraReporte &reporte, char*linea, int xs, EstructuraInfoReporte &data)
{
    char*nombreapellido=new char[MAX_NOMBRE];
    iniciarPuntero(nombreapellido, MAX_NOMBRE);
    llenarDinamico(nombreapellido);
    nombreapellido=strtok(linea, " ");
    reporte.nombre=new char[MAX_NOMBRE];
    iniciarPuntero(reporte.nombre, MAX_NOMBRE);
    strcpy(reporte.nombre, nombreapellido);
    llenarDinamico(reporte.nombre);
    nombreapellido=strtok(NULL, " ");
    reporte.apellido=new char[MAX_NOMBRE];
    iniciarPuntero(reporte.apellido, MAX_NOMBRE);
    strcpy(reporte.apellido, nombreapellido);
    llenarDinamico(reporte.apellido);
    nombreapellido=strtok(NULL, " ");
    if (nombreapellido!=NULL)
        return 1;
    cout<<"Ingrese la ciudad de "<<reporte.nombre<<" "<<reporte.apellido<<": ";
    reporte.ciudad=new char[15];
    iniciarPuntero(reporte.ciudad, 15);
    cin.getline(reporte.ciudad, 15, '\n');
    llenarDinamico(reporte.ciudad);
    char*elnombre=new char [MAX_NOMBRE];
    strcpy(elnombre,palabraAMinusculas(reporte.nombre));
    llenarDinamico(elnombre);
    char*elapellido=palabraAMinusculas(reporte.apellido);
    llenarDinamico(elapellido);
    char*elarchivo=new char[strlen(elnombre)+strlen(elapellido)+5];
    iniciarPuntero(elarchivo, strlen(elnombre)+strlen(elapellido)+5);
    strcpy(elarchivo, elnombre);
    strcat(elarchivo, elapellido);
    strcat(elarchivo, ".txt");
    llenarDinamico(elarchivo);
    reporte.nombre_archivo=new char[strlen(elarchivo) +1];
    iniciarPuntero(reporte.nombre_archivo, strlen(elarchivo) +1);
    strcpy(reporte.nombre_archivo, elarchivo);
    delete nombreapellido;
    delete elnombre;
    delete elapellido;
    delete elarchivo;
    return 0;
}
bool llenarTotalReportes(EstructuraArchivo &archivo)
{
    try
    {
        ofstream Interesado;
        for(int i=0; i<archivo.reporte.nreportes; i++)
        {
            Interesado.open(archivo.reporte.total_reportes[i].nombre_archivo, ios::app);
            if (Interesado.is_open())
            {
                Interesado<<"SÚPER CÁLCULO S.A.\n"<<archivo.reporte.total_reportes[i].nombre<<" "<<archivo.reporte.total_reportes[i].apellido<<endl;
                Interesado<<archivo.reporte.total_reportes[i].ciudad;
                Interesado<<"\nDespués de un análisis detallado de cada movimiento de efectivo realizado en la semana "<<archivo.reporte.numero_semanas;
                Interesado<<" se obtuvieron los siguientes datos:"<<endl<<'\t'<<"Unidades producidas "<<archivo.reporte.unidades_producidas<<endl; 
                Interesado<<'\t'<<"Unidades vendidas "<<archivo.reporte.unidades_vendidas<<endl<<'\t'<<"Utilidad operacional "<<archivo.reporte.utilidad_operacional;
                Interesado<<endl<<'\t'<<"Utilidad Neta "<<archivo.reporte.utilidad_neta<<endl<<"Cordial Saludo.\nDepartamento de Finanzas.";
                Interesado.close();
            }
            else
                return 1;
        }
        cout<<"Reportes generados correctamente"<<endl;
        return 0;
    }
    catch(...)
    {
        return 1;
    }
}
bool llenarInfoReportes(EstructuraArchivo &archivo)
{
    /*
     numero_semanas <Súperior derecha>
     unidades_producidas <inferior derecha>
     unidades_vendidas <Súper ior izquierda>
     utilidad_operacional <inferior izquierda>
     utilidad_neta <(filas/2, columnas/2)>
     */
     int nhoja;
     cout<<"Ingrese un numero de hoja de las "<<archivo.numero_hojas<<" disponibles: ";
     cin>>nhoja;
     if(nhoja<1||nhoja>archivo.numero_hojas)
        return 1;
    nhoja--;
    //imprimirHojaC(archivo.hojas_sin_calcular[nhoja]);
    archivo.reporte.numero_semanas=archivo.hojas_sin_calcular[nhoja].celdas_calculadas[0][archivo.hojas_sin_calcular[nhoja].columnas-1].dato;
    archivo.reporte.unidades_producidas=archivo.hojas_sin_calcular[nhoja].celdas_calculadas[archivo.hojas_sin_calcular[nhoja].filas-1][archivo.hojas_sin_calcular[nhoja].columnas-1].dato;
    archivo.reporte.unidades_vendidas=archivo.hojas_sin_calcular[nhoja].celdas_calculadas[0][0].dato;
    archivo.reporte.utilidad_operacional=archivo.hojas_sin_calcular[nhoja].celdas_calculadas[archivo.hojas_sin_calcular[nhoja].filas-1][0].dato;
    archivo.reporte.utilidad_neta=archivo.hojas_sin_calcular[nhoja].celdas_calculadas[archivo.hojas_sin_calcular[nhoja].filas/2][archivo.hojas_sin_calcular[nhoja].columnas/2].dato;
    //cout<<archivo.reporte.numero_semanas<<endl<<archivo.reporte.unidades_producidas<<endl<<archivo.reporte.unidades_vendidas<<endl<<archivo.reporte.utilidad_operacional<<endl<<archivo.reporte.utilidad_neta<<endl;
    return 0;
}
bool generarReportes(EstructuraArchivo &archivo)
{
    char*nombre;
    ifstream ar;
    nombre=getNombreArchivoInteresadosR();
    ar.open(nombre, ios::in);
    archivo.reporte.nreportes=contarLineas(nombre);
    cout<<archivo.reporte.nreportes<<endl;
    if (!archivo.reporte.nreportes || ar.fail())
        return 1;
    archivo.reporte.total_reportes=new EstructuraReporte[archivo.reporte.nreportes];
    char**matrizPalabras=new char*[archivo.reporte.nreportes];
    int i=0;
    matrizPalabras[i]=new char[MAX_NOMBRE];
    ar.getline(matrizPalabras[i], MAX_NOMBRE, '\n');
    while ((!ar.eof())&&(ar.good())&&(!textoVacio(matrizPalabras[i])))
    {
        if (i>=archivo.reporte.nreportes)
            return 1;
        else
        {
            i++;
            matrizPalabras[i]=new char[MAX_NOMBRE];
            iniciarPuntero(matrizPalabras[i], MAX_NOMBRE);
            ar.getline(matrizPalabras[i], MAX_NOMBRE, '\n');
        }
    }
    int xs=contarLineas(nombre);
    archivo.reporte.nreportes=xs;
    for(i=0;i<xs;i++)
    {
        if(llenarEstructuraReporte(archivo.reporte.total_reportes[i], matrizPalabras[i], xs, archivo.reporte))
            return 1;
    }
    archivo.reporte.nreportes=xs;
    return 0;
}
char* getNombreArchivoInteresadosR()
{
    char*error = new char [6];
    strcpy(error, "Error");
    try //Para asegurar que no ocurra ninguna clase de error
    {
        //Esta función retorna un char* con el nombre del archivo que tiene la(s) hoja(s) de calculo
        char*nombre = new char[TAM_MAX_NOMBRE_ARCHIVO+5];
        cout<<"Nombre del archivo que contiene los nombres de los interesados: ";
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
void iniciarPuntero(char*palabra, int tam_max)
{
    for(int i = 0; i < tam_max;i++)
        palabra[i]='\0';
}
char *generarNombreCelda( int posicion, int fila)
{
    char abecedario[26]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    char*nombre = new char[MAX_NOMBRE_CELDA];
    iniciarPuntero(nombre, MAX_NOMBRE_CELDA);
    int labc = 26;
    int l = 1;
    if (posicion > (labc-1))
        l++;
    if (posicion > (pow(labc, 2)+labc-1))
        l++;
    if (posicion > (pow(labc, 3) -1))
        l++;
    if (l==1)
        nombre[0]=abecedario[posicion%labc];
    else if (l == 2)
    {
        nombre[0]=abecedario[(posicion/labc)-1];
        nombre[1]=abecedario[posicion%labc];
    }
    char*nnombre = new char[MAX_NOMBRE_CELDA+1];
    strcpy(nnombre, nombre);
    delete nombre;
    itoa(fila+1, nombre, 10);
    strcat(nnombre, nombre);
    return nnombre;
    delete nnombre;
}
void reverse(char str[], int length) 
{ 
    int start = 0; 
    int end = length -1; 
    while (start < end) 
    { 
        swap(*(str+start), *(str+end)); 
        start++; 
        end--; 
    } 
} 
// Implementation of itoa() 
char* itoa(int num, char* str, int base) 
{ 
    int i = 0; 
    bool isNegative = false;   
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
    // In standard itoa(), negative numbers are handled only with  
    // base 10. Otherwise numbers are considered unsigned. 
    if (num < 0 && base == 10) 
    { 
        isNegative = true; 
        num = -num; 
    }  
    // Process individual digits 
    while (num != 0) 
    { 
        int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    }  
    // If number is negative, append '-' 
    if (isNegative) 
        str[i++] = '-'; 
    str[i] = '\0'; // Append string terminator 
    // Reverse the string 
    reverse(str, i); 
    return str; 
}
bool tieneNumero(char*dato)
{
    try
    {
        double *n=new double;
        *n=atof(dato);
        if (strlen(dato)>2)
            throw 1;
        if(!dato)
            throw 1;
        delete n;
        return 1;
    }
    catch(...)
    {
        return 0;
    }
}
void imprimirHoja(EstructuraHoja hoja)
{
    try
    {
        for(int i=0; i<hoja.filas;i++)
        {
            for(int j=0;j<hoja.columnas;j++)
            {
                cout<<setw(15)<<"Nombre "<<hoja.celdas[i][j].nombre<<" -> "<<hoja.celdas[i][j].dato;
            }
            cout<<endl;
        }
    }
    catch(...)
    {
        return;
    }
}
void imprimirHojaC(EstructuraHoja hoja)
{
    try
    {
        for(int i=0; i<hoja.filas;i++)
        {
            for(int j=0;j<hoja.columnas;j++)
            {
                cout<<setw(15)<<"Nombre "<<hoja.celdas_calculadas[i][j].nombre<<" -> "<<setw(10)<<hoja.celdas_calculadas[i][j].dato;
            }
            cout<<endl;
        }
    }
    catch(...)
    {
        return;
    }
}
int buscarFila(EstructuraHoja hoja, char*nombre_celda)
{
    bool found = false;
    int i,j;
    for(i=0; (i<hoja.filas)&&(!found);i++)
        {
            for(j=0;(j<hoja.columnas)&&(!found);j++)
            {
                if (!strcmp(hoja.celdas_calculadas[i][j].nombre, nombre_celda))
                {
                    found = true;
                }
            }
        }
        if (!found)
        {
            i = 1e-10;
            return i;
        }
        return i;
}
int buscarCol(EstructuraHoja hoja, char*nombre_celda)
{
    bool found = false;
    int i,j;
    for(i=0; (i<hoja.filas)&&(!found);i++)
        {
            for(j=0;(j<hoja.columnas)&&(!found);j++)
            {
                if (!strcmp(hoja.celdas_calculadas[i][j].nombre, nombre_celda))
                {
                    found = true;
                }
            }
        }
        if (!found)
        {
            j = 1e-10;
            return j;
        }
        return j;
}
double nuevoExtraerDato(EstructuraHoja hoja, char*nombre_celda)
{
    double ret;
    bool found = false;
    int i,j;
    for(i=0; (i<hoja.filas)&&(!found);i++)
        {
            for(j=0;(j<hoja.columnas)&&(!found);j++)
            {
                if (!strcmp(hoja.celdas_calculadas[i][j].nombre, nombre_celda))
                {
                    ret = (hoja.celdas_calculadas[i][j].dato);
                    found = true;
                }
            }
        }
        if (!found)
        {
            ret = -1e-10;
            return ret;
        }
        return ret;
}
char* extraerDato(EstructuraHoja hoja, char*nombre_celda)
{
    char*linea=new char[MAX_CELDA];
    bool found = false;
    int i,j;
    for(i=0; (i<hoja.filas)&&(!found);i++)
        {
            for(j=0;(j<hoja.columnas)&&(!found);j++)
            {
                if (!strcmp(hoja.celdas_calculadas[i][j].nombre, nombre_celda))
                {
                    strcpy(linea, hoja.celdas[i][j].dato);
                    found = true;
                }
            }
        }
        if (!found)
        {
            strcpy(linea, "Mal");
            return linea;
        }
        return linea;
}
bool calcularCeldaFormula(EstructuraHoja hoja, char*nombre_celda, int repeticion)
{
    stack <char*> datos_por_sumar;
    double resultado=0;
    char*datos=new char[MAX_CELDA];
    char*punteroI=new char[MAX_CELDA];
    strcpy(datos, extraerDato(hoja, nombre_celda));
    punteroI=strtok(datos,"=");
    //cout<<datos<<endl;
    //cout<<punteroI<<endl;
    datos=strtok(punteroI,"+");
    while(datos!=NULL)
    {
        datos_por_sumar.push(datos);
        datos=strtok(NULL,"+");
    } 
    int i=buscarFila(hoja, nombre_celda);
    int j=buscarCol(hoja, nombre_celda);
    //cout<<"Fila "<<i-1<<" col "<<j-1<<endl;
    if((i==1e-10)||(j==1e-10))
        return 1;
     double*a=new double;
    while(!datos_por_sumar.empty())
    {
        if(!strcmp("Mal", extraerDato(hoja,datos_por_sumar.top())))
            return 1;
        *a=nuevoExtraerDato(hoja, datos_por_sumar.top());
        /*if (strlen(retorno)>2)
        {
            if(*a==-1e-10)
                return 1;
        }*/
        resultado+=*a;
        //else
            //resultado +=atof(retorno);
        datos_por_sumar.pop();
        if(datos_por_sumar.empty())
        {
            hoja.celdas_calculadas[i-1][j-1].dato=resultado;
        }
    }
    hoja.celdas_calculadas[i-1][j-1].dato=resultado;
    //delete datos;
    //delete punteroI;
    return 0;
}
void eliminarPunteroEstructuraArchivo(EstructuraArchivo &archivo)
{
    for(int i = 0; i < archivo.numero_hojas; i++)
    {
        for (int j = 0; j < archivo.hojas_sin_calcular[i].filas;j++)
        {
            for(int w=0;w<archivo.hojas_sin_calcular[i].columnas;w++)
            {
                delete archivo.hojas_sin_calcular[i].celdas[j][w].nombre;
                delete archivo.hojas_sin_calcular[i].celdas[j][w].dato;
            }
            delete archivo.hojas_sin_calcular[i].celdas[j];
        }
        delete archivo.hojas_sin_calcular[i].celdas;
    }
    delete archivo.hojas_sin_calcular;
    for (int y=0; y<archivo.reporte.nreportes; y++)
    {
        delete archivo.reporte.total_reportes[y].nombre_archivo;
        delete archivo.reporte.total_reportes[y].nombre;
        delete archivo.reporte.total_reportes[y].apellido;
        delete archivo.reporte.total_reportes[y].ciudad;
    }
    delete archivo.reporte.total_reportes;
    delete archivo.nombre_archivo;
    delete &archivo;
}
bool calcularCeldas(EstructuraArchivo  archivoE)
{
    try //Este bloque lo que hace es intentar unas sentencias, en caso de ocurrir una excepción, pasa al bloque catch
    {
        ifstream archivo;
        archivo.open(archivoE.nombre_archivo, ios::in); //Abro el archivo asociado al nombre
        for (int j=0;j<archivoE.numero_hojas; j++)
        {
            /*cout<<"Hoja "<<j+1<<endl;
            imprimirHoja(archivoE.hojas_sin_calcular[j]);*/
            archivoE.hojas_sin_calcular[j].celdas_calculadas=new CeldaCalculada*[archivoE.hojas_sin_calcular[j].filas];
            for (int w=0;w<archivoE.hojas_sin_calcular[j].filas;w++)
            {
                //w me indica la fila actual
                try
                {
                    archivoE.hojas_sin_calcular[j].celdas_calculadas[w]=new CeldaCalculada[archivoE.hojas_sin_calcular[j].columnas];
                    for (int y = 0; y<archivoE.hojas_sin_calcular[j].columnas;y++ )
                    {
                        //y me indica la columna
                        archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].nombre=new char[strlen(archivoE.hojas_sin_calcular[j].celdas[w][y].nombre)+1];
                        strcpy(archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].nombre, archivoE.hojas_sin_calcular[j].celdas[w][y].nombre);
                        //cout<<archivoE.hojas_sin_calcular[j].celdas[w][y].nombre<<' '<<archivoE.hojas_sin_calcular[j].celdas[w][y].dato<<'-'<<archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].nombre<<endl;
                        if (tieneNumero(archivoE.hojas_sin_calcular[j].celdas[w][y].dato))
                        {
                            archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].dato=atof(archivoE.hojas_sin_calcular[j].celdas[w][y].dato);
                            //cout<<archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].dato<<endl;
                            //archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].nombre=archivoE.hojas_sin_calcular[j].celdas[w][y].nombre;
                            //cout<<archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].nombre<<endl;
                        }
                        else
                        {
                            archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].dato=0;
                            //calcularCeldaFormula(archivoE.hojas_sin_calcular[j], archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].nombre, 0);                    
                        }
                    }
                }
                catch(...)
                {
                    return 1; //Error
                }
            }
        }
        /*catch(...)
        {  
            return 1; //Error
        }*/
        for (int j=0;j<archivoE.numero_hojas; j++)
        {
            for (int w=0;w<archivoE.hojas_sin_calcular[j].filas;w++)
            {
                //w me indica la fila actual
                try
                {
                    for (int y = 0; y<archivoE.hojas_sin_calcular[j].columnas;y++ )
                    {
                        //y me indica la columna
                        //cout<<archivoE.hojas_sin_calcular[j].celdas[w][y].nombre<<' '<<archivoE.hojas_sin_calcular[j].celdas[w][y].dato<<'-'<<archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].nombre<<endl;
                        if (!tieneNumero(archivoE.hojas_sin_calcular[j].celdas[w][y].dato))
                            if(calcularCeldaFormula(archivoE.hojas_sin_calcular[j], archivoE.hojas_sin_calcular[j].celdas_calculadas[w][y].nombre, 0))
                                throw 1;
                    }
                }
                catch(...)
                {
                    return 1; //Error
                }
            }
            /*cout<<"Hoja "<<j+1<<endl;*/
                //imprimirHojaC(archivoE.hojas_sin_calcular[j]);
        }
        cout<<endl;
        
        cout<<endl;
        return 0;
    }
    catch(...)
    {
        return 1; //Error
    }
}
