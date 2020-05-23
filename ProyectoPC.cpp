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
#include <iomanip> //Para setw())
const int TAM_MAX_NOMBRE_ARCHIVO = 20;
const int TAM_LINEA = 100;
const int MAX_CELDA = 20;
const int MAX_NOMBRE=20;
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
void leerCeldas(EstructuraArchivo  archivo);
void calcularCeldas(EstructuraArchivo  archivo);
void generarReportes(EstructuraArchivo archivo);
char* getNombreArchivoInteresadosR(char* palabra);
void llenarDatosReporte(double& supIzq,double& infIzq,double& supDer,double& infDer,double& neta,EstructuraHoja* hoja);
int comparaCadenas(char* cadena1, char* cadena2);
int autorizaReporte(EstructuraReporte*& reporte); //Esta es la funcion getNombreArchivoInteresadosR pero retorna 0 o 1
void espacioInfoReporte(EstructuraReporte*& reporte);
void pruebaInfoReporte();
int main()
{
    pruebaInfoReporte();

   // return HojasCalculo();*/
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
void leerCeldas(EstructuraArchivo  archivoE)
{

}
void calcularCeldas(EstructuraArchivo  archivo)
{

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

    EstructuraHoja* hoja;
    hoja=(archivo.hojas_sin_calcular+numHoja); //Esto se hace para manejar más facilmente las operaciones punteros
    llenarDatosReporte(supIzq,infIzq,supDer,infDer,neta,hoja); //Lena cada una de los datos que se necesitan en el reporte
    archivo.reporte.numero_semanas=supDer;
    archivo.reporte.unidades_producidas=infDer;
    archivo.reporte.unidades_vendidas=supIzq;
    archivo.reporte.utilidad_operacional=infIzq;
    archivo.reporte.utilidad_neta=neta;
    //No se si inforeporte viene con espacio declarado, si es el caso dado se activa esta funcion
    espacioInfoReporte(archivo.reporte.total_reportes);
    if(autorizaReporte(archivo.reporte.total_reportes)==1)
    {
        ofstream salida;
        salida.open(getNombreArchivoInteresadosR((archivo.reporte.total_reportes)->nombre_archivo),ios::out);
        salida<<"Super Calculo S.A."<<endl;
        salida<<(archivo.reporte.total_reportes)->nombre<<" "<<(archivo.reporte.total_reportes)->apellido<<endl;
        salida<<(archivo.reporte.total_reportes)->ciudad<<endl<<endl;
        salida<<"Despues de una analisis detallado de cada movimiento de efectivo realizado"<<endl;
        salida<<"en la semana "<<supDer<<" se obtuvieron los siguientes datos :"<<endl<<endl;
        salida<<'\t'<<"Unidades producidas: "<< infDer<<endl;
        salida<<'\t'<<"Unidades vendidas: "<<supDer<<endl;
        salida<<'\t'<<"Utilidad Operacional: "<<infIzq<<endl;
        salida<<'\t'<<"Utilidad Neta: "<<neta<<endl<<endl;
        salida<<"Cordial Saludo."<<endl<<endl;
        salida<<"Departamento de Finanzas.";
        salida.close();

    }
    else
    {
        cout<<"No puede realizar el reporte porque no se encuentra registrado";
    }



}
int autorizaReporte(EstructuraReporte*& reporte)
{
    ifstream entrada;
    char *palabra = new char[20];
    cout<<"Ingrese el nombre del archivo que desea buscar(No olvide la extension .txt) :";
    cin.getline(palabra,20,'\n');
    entrada.open(palabra,ios::in);
    if(entrada.fail())
    {
        cout<<"Error al buscar el archivo";
        return 0;
    }
    cout<<"Ingrese informacion del interesado en el reporte :";
    cout<<endl<<"Nombre :";
    cin.getline(reporte->nombre,20,'\n');
    cout<<"Apellido :";
    cin.getline(reporte->apellido,20,'\n');
    cout<<"Ciudad :";
    cin.getline(reporte->ciudad,20,'\n');
    char* definitivo=new char[40];
    strcpy(definitivo,reporte->nombre);
    strcat(definitivo,reporte->apellido);
    strcpy(reporte->nombre_archivo,definitivo);
    char* linea=new char[100];
    bool nombreCmp=false,apellidoCmp=false;
    char* pch;
    while(!entrada.eof())
    {
        cout<<"-";
        entrada.getline(linea,100,'\n');
        nombreCmp=false;
        apellidoCmp=false;
        pch=strtok(linea,", ");
        if(comparaCadenas(pch,reporte->nombre)==0)
        {
            nombreCmp=true;
        }
        pch=strtok(NULL,", ");
        if(comparaCadenas(pch,reporte->apellido)==0)
        {
            apellidoCmp=true;
        }
        if(nombreCmp==true&&apellidoCmp==true)
        {
            cout<<"Permitido hacer el registro"<<endl;
            entrada.close();
            return 1; //Esta registrado y puede generarse el archivo
        }
    }
    delete definitivo;
    delete pch;
    delete linea;
    entrada.close();
    return 0;

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
    archivo.reporte;
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
