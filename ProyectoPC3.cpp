/*Autores:
- Nicolás Bayona
- Pablo Bright
- Nicolás García
*/
//None but ourselves can free our minds --Bob Marley
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
# include <stack>
#include <cmath>
#include <iomanip> //Para setw())
const int TAM_MAX_NOMBRE_ARCHIVO = 20;
const int TAM_LINEA = 100;
const int MAX_CELDA = 20;
const int MAX_NOMBRE=20;
const int MAX_APELLIDO=20;
const int MAX_NOMBRE_CELDA=10;
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
    char *nombre; //A1...Z999,AA1...ZZ999,AAA1...ZZZ999
    char *dato; //Valor
};
struct CeldaCalculada
{
    char *nombre; //A1...Z999,AA1...ZZ999,AAA1...ZZZ999
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
    EstructuraHoja* hojas_sin_calcular;
    EstructuraInfoReporte reporte;
};
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
void generarReportes(EstructuraArchivo archivo);
char* getNombreArchivoInteresadosR(char* palabra);
void llenarDatosReporte(double& supIzq,double& infIzq,double& supDer,double& infDer,double& neta,EstructuraHoja* hoja);
int comparaCadenas(char* cadena1, char* cadena2);
bool autorizaReporte(EstructuraReporte*& reporte,int supDer,int infDer,int supIzq,int infIzq,int neta);
void espacioInfoReporte(EstructuraReporte*& reporte);
void pruebaInfoReporte();
void reverse(char str[], int length) ;
void iniciarPuntero(char*palabra, int tam_max);
char* itoa(int num, char* str, int base) ;
bool tieneNumero(char*dato);
void imprimirHoja(EstructuraHoja hoja);
bool calcularCeldaFormula(EstructuraHoja hoja, char*nombre_celda, int repeticion);
char* extraerDato(EstructuraHoja hoja, char*nombre_celda);
void eliminarPunteroEstructuraArchivo(EstructuraArchivo &archivo);
int main()
{
    HojasCalculo();
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
                generarReportes(*archivo);
                /*for(int w =0; w < archivo->numero_hojas;w++)
                    imprimirHojaC(archivo->hojas_sin_calcular[w]);*/
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
void iniciarPuntero(char*palabra, int tam_max)
{
    for(int i = 0; i < tam_max;i++)
        palabra[i]='\0';
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
void generarReportes(EstructuraArchivo archivo)
{
    int numHoja=-1;
    do
    {
        //Es importante señalar que es entre 0 y numero de hojas -1, la persona al realizarlo debe saber esto
        cout<<"El archivo cuenta con "<<(archivo.numero_hojas-1)<<" hojas de calculo(inicia en 0) , ingrese sobre cual desea hacer el reporte :";
        cin>>numHoja;
        cin.ignore(1);
    }while((numHoja>=archivo.numero_hojas)||(numHoja<0)); //Obtengo la hoja a la cual le haré indireccion para usar cada uno de sus elementos
    double supIzq=0,infIzq=0,supDer=0,infDer=0,neta=0; //Definimos las variables que se registraran en el archivo
    //No se si inforeporte viene con espacio declarado, si es el caso dado se activa esta funcion
    espacioInfoReporte(archivo.reporte.total_reportes);
    EstructuraHoja* hoja;
    hoja=(archivo.hojas_sin_calcular+numHoja); //Esto se hace para manejar más facilmente las operaciones punteros
    llenarDatosReporte(supIzq,infIzq,supDer,infDer,neta,hoja); //Lena cada una de los datos que se necesitan en el reporte
    archivo.reporte.numero_semanas=supDer;
    archivo.reporte.unidades_producidas=infDer;
    archivo.reporte.unidades_vendidas=supIzq;
    archivo.reporte.utilidad_operacional=infIzq;
    archivo.reporte.utilidad_neta=neta;

    autorizaReporte(archivo.reporte.total_reportes,supDer,infDer,supIzq,infIzq,neta); //Imprime todos los reportes


}
bool autorizaReporte(EstructuraReporte*& reporte,int supDer,int infDer,int supIzq,int infIzq,int neta)
{
    ifstream entrada;
    ofstream salida;
    char *palabra = new char[20];
    cout<<"Ingrese el nombre del archivo que desea buscar :";
    cin.getline(palabra,20,'\n');
    entrada.open(getNombreArchivoInteresadosR(palabra),ios::in);
    if(entrada.fail())
    {
        cout<<"Error al buscar el archivo";
        return false;
    }
    char* definitivo=new char[40];
    char* linea=new char[100];
    char* pch;
    bool fin=false;
    while(!entrada.eof()&&(!fin))
    {
        entrada.getline(linea,100,'\n');
        if (textoVacio(linea))
            fin=true;
        else
        {
            cout<<"-";
            pch=strtok(linea,", ");
            strcpy(reporte->nombre,pch);
            pch=strtok(NULL,", ");
            strcpy(reporte->apellido,pch);
            strcpy(definitivo,reporte->nombre);
            strcat(definitivo,reporte->apellido);
            strcpy(reporte->nombre_archivo,definitivo);
            cout<<"Para "<<reporte->nombre<<" "<<reporte->apellido<<" ingrese la ciudad :";
            cin.getline(reporte->ciudad,20,'\n');
            //Llamar la funcion que genera archivo
            salida.open(getNombreArchivoInteresadosR((reporte)->nombre_archivo),ios::out);
            salida<<"Super Calculo S.A."<<endl;
            salida<<(reporte)->nombre<<" "<<(reporte)->apellido<<endl;
            salida<<(reporte)->ciudad<<endl<<endl;
            salida<<"Despues de una analisis detallado de cada movimiento de efectivo realizado"<<endl;
            salida<<"en la semana "<<supDer<<" se obtuvieron los siguientes datos :"<<endl<<endl;
            salida<<'\t'<<"Unidades producidas: "<< infDer<<endl;
            salida<<'\t'<<"Unidades vendidas: "<<supIzq<<endl;
            salida<<'\t'<<"Utilidad Operacional: "<<infIzq<<endl;
            salida<<'\t'<<"Utilidad Neta: "<<neta<<endl<<endl;
            salida<<"Cordial Saludo."<<endl<<endl;
            salida<<"Departamento de Finanzas.";
            salida.close();
        }
    }
    delete definitivo;;
    delete linea;
    entrada.close();
    return true;
}
int comparaCadenas(char* cadena1, char* cadena2)
{ //Strcmp no funciono pero esta si
    int n=strlen(cadena2);
    bool done=true;
    for(int i=0;i<n;i++)
    {
        if(*(cadena1)!=*(cadena2))
            done=false;
    }
    if(done)
    {
        return 0;
    }
    else
        return -1;
}
void espacioInfoReporte(EstructuraReporte*& reporte)
{
    reporte=new EstructuraReporte[1];
    reporte->nombre=new char{20};
    reporte->apellido=new char[20];
    reporte->nombre_archivo=new char[20];
    reporte->ciudad=new char[20];
}
void pruebaInfoReporte()
{
    srand(time(NULL));
    //Esto es una prueba para ver que todo funciona correctamente
    EstructuraArchivo archivo;
    archivo.nombre_archivo=new char[20];
    archivo.numero_hojas=2;
    archivo.hojas_sin_calcular=new EstructuraHoja[2];
    strcpy(archivo.nombre_archivo,"Trial");
    //hojas calculadas
    (archivo.hojas_sin_calcular+0)->filas=3;
    (archivo.hojas_sin_calcular+0)->columnas=3;
    (archivo.hojas_sin_calcular+0)->celdas_calculadas=new CeldaCalculada*[3];
    for(int i=0; i<3; i++)
    {
        ((archivo.hojas_sin_calcular)->celdas_calculadas)[i]=new CeldaCalculada[3];
        for(int j=0; j<3; j++)
        {
            ((*(((archivo.hojas_sin_calcular)->celdas_calculadas)+i)+j))->dato=i*rand()%30;
        }
    }

    (archivo.hojas_sin_calcular+1)->filas=5;
    (archivo.hojas_sin_calcular+1)->columnas=5;
    (archivo.hojas_sin_calcular+1)->celdas_calculadas=new CeldaCalculada*[5];
    for(int i=0; i<5; i++)
    {
        ((archivo.hojas_sin_calcular+1)->celdas_calculadas)[i]=new CeldaCalculada[5];
        for(int j=0; j<5; j++)
        {
            ((*(((archivo.hojas_sin_calcular+1)->celdas_calculadas)+i)+j))->dato=i*rand()%30;
        }
    }
    generarReportes(archivo);
}
char* getNombreArchivoInteresadosR(char* palabra)
{
    char* extension=new char[5];
    strcpy(extension,".txt");
    char* nombre=new char[50];
    strcpy(nombre,palabra);
    strncat(nombre,extension,6);
    delete extension;
    return nombre;
}
void llenarDatosReporte(double& supIzq,double& infIzq,double& supDer,double& infDer,double& neta,EstructuraHoja* hoja)
{
    int f,c,filUti,colUti; //Definimos las filas y columas para realizar las validaciones del reporte

    f=(hoja+0)->filas;
    c=(hoja+0)->columnas;
    filUti=f/2;
    colUti=c/2;

    for(int i=0; i<f; i++) //Vamos a sacar los elementos requeridos para realizar el informe
    {
        for(int j=0; j<c; j++)
        {
            if(i==0&&j==0)
            {
                supIzq=((*(((hoja+0)->celdas_calculadas)+i)+j))->dato;
            }
            if(i==f-1&&j==0)
            {
                infIzq=((*(((hoja+0)->celdas_calculadas)+i)+j))->dato;
            }
            if(i==0&&j==c-1)
            {
                supDer= ((*(((hoja+0)->celdas_calculadas)+i)+j))->dato;
            }
            if(i==f-1&&j==c-1)
            {
                infDer=((*(((hoja+0)->celdas_calculadas)+i)+j))->dato;
            }
            if(i==filUti&&j==colUti)
            {
                neta=((*(((hoja+0)->celdas_calculadas)+i)+j))->dato;
            }
        }
    }
}
