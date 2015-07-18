//Librerias externas
#include <iostream>
#include <stdio.h>
#include <fstream> //Fichero
using namespace std;

//************************************
//FUNCION: void File(string db,string tabla)
//PARAMETROS: Nombre de base de datos y tabla empleados
//RETORNO: String con el nombre del fichero txt
//DESCRIPCION: Crear fichero .txt registro de nueva plantacion.
//FUNCIONES INVOCADAS: n/a
//*************************************

string File(string db,string tabla){
	
	char id[30];
	string registro=db+"_"+tabla+".txt";//Fichero registro descripcion
	string descrip;
		
	strcpy(id,registro.c_str());	
	ofstream fs(id); //Nuevo fichero
	
	cin.ignore();//Necesario para el getline(); 
	
	cout<<"Inserte descripción de la plantacion (ENTER para finalizar)"<<endl;
	
	getline(cin,descrip);	
	fs<<descrip<<endl;	
	
	cout<<"ENTER para cerrar registro: "<<registro<<endl;
	cin.ignore();//Necesario para el getline(); 

	fs.close();	
	cout<<id<<" Guardado con exito"<<endl;	
	return registro;
	}
