//////////////////////////////////////////////////////////
// CONTROL Y MONITORIZACIÓN DE UN INVERNADERO A TRAVÉS//// 
// DE UNA APLICACIÓN MÓVI                             ////
// Trabajo Final de Master Ingeniera Electromecanica  ////
// Esp. Mecatronica                                   ////
// Departamento de Electronica, Automatica e Infor.   ////
// Indusrial.                                         ////
// Escuela Tec. Superior de Ing. y Diseño Ind.        ////
// Universidad Politecnica de Madrid                  ////
// Alumno: ANDRES BARROSO GARCIA                      ////
// NºMatricula: 2053                                  ////
// Curso: 2014-2015                                   ////
// Fecha: 19-05-2015                                  ////
//////////////////////////////////////////////////////////

//Librerias externas
#include <iostream>
#include <stdio.h>
#include <pthread.h>//Hilos
#include <mysql++/mysql++.h>//MySQL
#include <SerialPort.h>//SerialPort
#include <ctime> //Date-Time
#include <termios.h>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>//Json
#include <boost/property_tree/json_parser.hpp>
#include <fstream>
//Librerias internas
#include "database.h"//Petciones MySQL
#include "date.h"//Fecha/Hora actual
#include "echo.h"//Encapsular password
#include "file.h"//Ficheros
#include "server_http.hpp"//Servicios REST
#include "client_http.hpp"
#include "InvernaderoServer.h"//Clase InvernaderoServer

using namespace SimpleWeb;
using namespace boost::property_tree;
using namespace std;

void Login();//Inializacion de serverICRA
int  ConnectArduino();//Conexion puerto serie
void * ClientAndroid(void *var);//Hilo Servicios REST


//Variables conexion MySQL
mysqlpp::Connection conn(false);
mysqlpp::Query query=conn.query();
string tabla;
string db;
string registro;
char server[] = "localhost";//Servidor
char user[10];  
char pass[10];

bool permiso=false;//Datos from Android
bool acabar=false;

InvernaderoServer icraServer;//Objeto invernadero

int main()
{
	pthread_t httpget;

	Login();//Identificacion. Conexion MySQL. Insercion SerialPort
	
	pthread_create(&httpget, NULL, ClientAndroid, NULL); // Hilo servicios REST
	cout<<"\033[1;34mLanzado hilo ClientAndroid\033[0m"<<endl;	
	
	ConnectArduino();//Intercomunicacion con placa
	
	pthread_exit(NULL); // Última función que debe ejecutar el main() siempre
	conn.disconnect();//Desconexion MySQL
	cout<<"END PROGRAM"<<endl;
	return 0;	
}

///////////////////////////////////////////////////
/////////////////FUNCIONES DEL PROGRAMA ///////////
///////////////////////////////////////////////////

//************************************
//FUNCION: void Login()
//PARAMETROS: n/a
//RETORNO: n/a
//DESCRIPCION: Salida por pantalla mensaje bienvenida. Solicitud user,pass,bd,tabla,puertoSerie
//FUNCIONES INVOCADAS: NewTable(base de datos, tabla)
//*************************************

void Login(){
	
	//Cabecera
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;
	cout<<"\033[0;32m***************\033[0m\033[1;32m      PROYECTO ICRA       \033[0m\033[0;32m***************\033[0m"<<endl;	
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;
	
	cout<<"\033[1;32mInvernadero Controlado Remotamente por Android\033[0m"<<endl;
	cout<<"\033[1;36mAutor: Barroso García, Andrés\033[0m"<<endl;
	cout<<"URL: http://icra.blogspot.es"<<endl;
	cout<<"Proyecto desarrollado como Trabajo Final de Máster Curso 2014/15"<<endl;
	cout<<"Máster Universitario en Ingeniería Electromecánica: Mecatrónica"<<endl;
	cout<<"Escuela Técnica Superior de Ingeniería y Diseño Industrial"<<endl;
	cout<<"Universidad Politécnica de Madrid"<<endl;
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;	
	
	bool ACESS=false;//Marca ID
	
	do{//IDentificacion		

		cout<<"\033[0;35mConectando con \033[0m"<<server<<endl;
		cout<<"Inserte USER: "<<endl;
		cin>>user;
		cin.ignore();//Necesario para el getline(); 	
			
		cout<<"Inserte PASS: "<<endl;	
			
		string pwd;
		echo( false );//Modo escritura oculta
		getline( cin, pwd );
		echo( true );//Modo escritura visible		
		strcpy(pass,pwd.c_str());
		
	
		if (conn.connect(NULL, server, user, pass)){//Conexion server
			ACESS=true;//ID OK
			
			cout<<"Inserte BD a crear: "<<endl;
			cin>>db;
				
			string consulta="SHOW DATABASES";
			query=conn.query(consulta);

			if (mysqlpp::StoreQueryResult res = query.store())
			{
				if (res.num_rows() > 0)
				{
					bool found=false;

					for (mysqlpp::StoreQueryResult::iterator it=res.begin(); it!=res.end(); ++it)
					  {
						if ((it)->at(0)==db)//Consulta existencia de base datos insertada
						{
						 found=true;
						 break;
						}
					  }
					if (found)//SI EXISTE LA BD...
					{  
					 cout << "La base de datos se encuentra" << endl;
					 cout << "Inserte nombre de la tabla a crear" <<endl;
					 cin >> tabla;
					 query.reset();
					 query<<"SELECT * FROM `"+db+"`.`"+tabla+"`";
					 res=query.store();
					 if (res.num_rows() >0)//SI EXISTE LA TABLA...
					   {

						 for (mysqlpp::StoreQueryResult::iterator it=res.begin(); it!=res.end(); ++it)
						   {
							 for (unsigned i=0; i<res.num_fields(); i++)
							   cout <<res.field_name(i)<<" = "<< (it)->at(i) << endl;//Salida por pantalla
							   cout << endl;
						   }
						  cout<<"Se necesita una nueva tabla para iniciar el sistema"<<endl;
						  ACESS=false;//RESET ID						 
					   }
					 else{//SI NO EXISTE...
					  
						if (conn.select_db(db))
						{
						 cout << "Creando tabla" << endl;
						 query.reset();
						 
						 query=conn.query(NewTable(db,tabla)); //Creacion tabla
								   
						}
						else
							cout << "Fallo al seleccionar la base de datos" << endl; 
					 					 
					  }
					}
					else//SI NO EXISTE BD...
					{ 
						cout << "Creando base de datos" << endl;
						if (conn.create_db(db))//Creacion base datos
						  {
							cout << "Seleccionando base de datos" << endl;
							if (conn.select_db(db))
							  {
								cout << "Inserte nombre de la tabla a crear" <<endl;
								cin >> tabla;
								cout << "Creando tabla" << endl;
								query.reset();
									 
								query=conn.query(NewTable(db,tabla));//Creacion tabla
					
								}
							else
								cout << "Fallo al seleccionar la base de datos" << endl;
						   }
					}
				}				
				else
					cout << "Fallo al obtener bases de datos" << endl;				
			}		   
		}      
		else{//Fallo conexion server
			cout <<"Fallo al conectarse a la BD: "<<conn.error()<<endl;
			ACESS=false;//RESET ID
		}
			
	}while(ACESS==false);//Bucle Peticion IDentificacion
	
	registro=File(db,tabla);//Creacion fichero registro plantacion	
	icraServer.SetPort();
}

//************************************
//FUNCION: void ConnectArduino()
//PARAMETROS: n/a
//RETORNO: n/a
//DESCRIPCION: Comunicacion SerialPort Arduino. Escritura en base de datos.
//FUNCIONES INVOCADAS: n/a
//*************************************

int ConnectArduino(){

 	char time[15],date[15];//cadenas de tiempo
 	
    cout << "Serial port: " << icraServer.GetPort() << endl;
	cout << "Opening..." << endl;
	//-- Open the serial port
	//-- Serial port configuration: 9600 baud, 8N1
	SerialPort serial_port(icraServer.GetPort());
	cout<<"\033[1;32mConnected\033[0m"<<endl;

	if (query.execute())//Conectado a tabla en BD
	{			
		SerialPort::DataBuffer buffer;	
		///RECEIVE Arduino: Initial Adjustment from Arduino
		try {
		serial_port.Open(SerialPort::BAUD_9600,
						 SerialPort::CHAR_SIZE_8,
						 SerialPort::PARITY_NONE,
						 SerialPort::STOP_BITS_1,
						 SerialPort::FLOW_CONTROL_NONE);
		}
		catch (SerialPort::OpenFailed E) {
			cerr << "Error opening the serial port" << endl;
		}
		try {
			serial_port.Write("x");//ID cabecera
			serial_port.Read(buffer,icraServer.GetCMD_0().size(),5000);
		}
		catch (SerialPort::ReadTimeout E) {
			cout << "TIMEOUT!";
		}				
		serial_port.Close();
		
		//Extraccion de datos
		for(int i=0;i<buffer.size();i++) {
			char val[1];
			val[0]=buffer[i];//lectura componente buffer
			icraServer.AdqAdjustment(val[0]);
		}		
		
		///BUCLE DE COMUNICACION
		while(1){		  
			if(permiso){
				///SEND Arduino: Valores Android								
				try {
					serial_port.Open(SerialPort::BAUD_9600,
									 SerialPort::CHAR_SIZE_8,
									 SerialPort::PARITY_NONE,
									 SerialPort::STOP_BITS_1,
									 SerialPort::FLOW_CONTROL_NONE);
				}
				catch (SerialPort::OpenFailed E) {
					cerr << "Error opening the serial port" << endl;
					//return NULL;
					break;
				}				
				
				serial_port.Write(icraServer.SendMessage());					
				permiso=false;
				serial_port.Close();
			}//Fin permiso	
					
			///RECEIVE Arduino: Estado de variables						
			try {
				serial_port.Open(SerialPort::BAUD_9600,
								 SerialPort::CHAR_SIZE_8,
								 SerialPort::PARITY_NONE,
								 SerialPort::STOP_BITS_1,
								 SerialPort::FLOW_CONTROL_NONE);
			}
			catch (SerialPort::OpenFailed E) {
				cerr << "Error opening the serial port" << endl;
				break;
			}			
			try {
				serial_port.Write("z");//ID cabecera
				serial_port.Read(buffer,icraServer.GetCMD().size(),5000);
			}
			catch (SerialPort::ReadTimeout E) {
				cout << "TIMEOUT!";
				break;
			}				
			
			//Extraccion de datos
			for(int i=0;i<buffer.size();i++) {
				char val[1];
				val[0]=buffer[i];
				icraServer.AdqData(val[0]);
			}
			
			//Obtencion Fecha/Hora actual	
			Date(time,date);			
		
			//Monitor
			icraServer.Monitor();
			cout<<" "<<time<<endl;
			cout<<" "<<date<<endl;
			cout<<"--------------------"<<endl;				
							
			//MySQL					
			query.reset();
			query << icraServer.InsertMySQL(tabla,time,date);
			if(!query.execute())
				cout << "Fallo al introducir datos"<<endl;										 

			serial_port.Close();
			if(acabar)break;			
		}//Fin Bucle de comunicacion	
		cout<<"FALLO SINCRONIZACION"<<endl;
	}
	else
		cout << "Error al crear tabla"<<endl;	
	
	return 0;
}
	
//************************************
//FUNCION: void ClientAndroid()
//PARAMETROS: n/a
//RETORNO: n/a
//DESCRIPCION: Servicios REST GET. Enlace usuarios Android
//FUNCIONES INVOCADAS: n/a
//*************************************

void *ClientAndroid(void *var){
	
	int port=8080;
	Server<HTTP> server(port, 4);
	
	///GET Bit de vida	
    server.resource["^/life/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {			
		string json="{bit: 101}";	      
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };     
    ///GET Monitor
    server.resource["^/monitor/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string json=icraServer.JsonMonitor();	      
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    }; 	    
    ///GET Registro
    server.resource["^/registro/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string json="{reg: "+registro+"}";		      
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    }; 	     
    ///GET Login
    server.resource["^/login/([0-z]+)/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
    
		string login_user=request->path_match[1];
		string login_pass=request->path_match[2];
		string login_level;
			  
		query.reset();
		query<<"SELECT level FROM `"+db+"`.`"+"users"+"` WHERE user='"+login_user+"' AND pass='"+login_pass+"'";
		mysqlpp::StoreQueryResult res=query.store();
		
		if (res.num_rows() >0)//User-Pass correctos
		{
			mysqlpp::StoreQueryResult::iterator it=res.begin();		 
			mysqlpp::String resul=(it)->at(0);//Obtencion del nivel de permiso 		
			resul.to_string(login_level);		  					 
		}
		else
			login_level="empty";
		
		string json="{level: "+login_level+"}";				  
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };       
    ///GET Grafica     
    server.resource["^/grafica/([A-z]+)/([A-z]+)/([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string field1=request->path_match[1];//variable 1 a buscar
		string field2=request->path_match[2];//variable 2 a buscar			
		string begin_id=obtainID(request->path_match[3],request->path_match[4],db,tabla,query);//ID inicial rango
		string end_id=obtainID(request->path_match[5],request->path_match[6],db,tabla,query);//ID final rango
		string json;
		
		if(begin_id=="empty"||end_id=="empty")json="empty";
		else json=jsonArray(begin_id,end_id,field1,field2,db,tabla,query);//Obtain jsonArray
				  
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };      
    ///GET Rangos de Actuadores
    server.resource["^/rangos/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {	
		string json=icraServer.JsonRangos();			  
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };    
    ///GET Panel Alarma
    server.resource["^/panel/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {	
		string json=icraServer.JsonPanel();		      
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };       	     
    ///GET Rangos de Alarmas
    server.resource["^/alarmas/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string json=icraServer.JsonAlarmas();	
	    response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };    	
    ///GET Temperatura.t0 Rango inferior trabajo
    server.resource["^/t0/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string t0=request->path_match[1];    
		string json=icraServer.JsonSetLim("t0",t0);
		permiso=true;      
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };     
    ///GET Temperatura.t1 Rango superior trabajo
    server.resource["^/t1/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string t1=request->path_match[1];    
		string json=icraServer.JsonSetLim("t1",t1);
		permiso=true;       
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };       
    ///GET Temperatura.T0 Rango inferior alarma
    server.resource["^/t0a/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string T0=request->path_match[1];    
		string json=icraServer.JsonSetLim("t0a",T0);
		permiso=true;      
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };   
    ///GET Temperatura.T1 Rango superior alarmas
    server.resource["^/t1a/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string T1=request->path_match[1];    
		string json=icraServer.JsonSetLim("t1a",T1);
		permiso=true;       
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };    
    ///GET Humedad.h0 Rango inferior trabajo
    server.resource["^/h0/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string h0=request->path_match[1];    
		string json=icraServer.JsonSetLim("h0",h0);
		permiso=true;        
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };
    ///GET Humedad.h1 Rango superior trabajo
    server.resource["^/h1/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string h1=request->path_match[1];    
		string json=icraServer.JsonSetLim("h1",h1);
		permiso=true;        
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };       
    ///GET Humedad.H0 Rango inferior alarma
    server.resource["^/h0a/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string H0=request->path_match[1];    
		string json=icraServer.JsonSetLim("h0a",H0);
		permiso=true;  
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    }; 
    ///GET Humedad.H1 Rango superior trabajo
    server.resource["^/h1a/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string H1=request->path_match[1];    
		string json=icraServer.JsonSetLim("h1a",H1);
		permiso=true;         
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };     
	///GET Ventilador.velocidad
    server.resource["^/vent/([0-9]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string speed=request->path_match[1];
		string json=icraServer.JsonSetVent(speed);
		permiso=true;  	  
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };   
   ///GET Interruptor.estado
    server.resource["^/switch/([0-1]+)/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		string sw=request->path_match[1];
		string json=icraServer.JsonSetSwitch(sw);
		permiso=true;       
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
    };
   ///GET Return
    server.resource["^/return/?$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
		acabar=true;
		string json="APAGANDO";	      
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" <<json;
		pthread_exit((void *)"Off solicitado");
    };    
    
	///GET Servidor
	server.default_resource["^/?(.*)$"]["GET"]=[](ostream& response, shared_ptr<Server<HTTP>::Request> request) {
        string filename="web/";
        
        string path=request->path_match[1];
        
        //Replace all ".." with "." (so we can't leave the web-directory)
        size_t pos;
        while((pos=path.find(".."))!=string::npos) {
            path.erase(pos, 1);
        }
        
        filename+=path;
        ifstream ifs;
        //A simple platform-independent file-or-directory check do not exist, but this works in most of the cases:
        if(filename.find('.')==string::npos) {
            if(filename[filename.length()-1]!='/')
                filename+='/';
            filename+="index.html";
        }
        ifs.open(filename, ifstream::in);
        
        if(ifs) {
            ifs.seekg(0, ios::end);
            size_t length=ifs.tellg();
            
            ifs.seekg(0, ios::beg);

            //The file-content is copied to the response-stream. Should not be used for very large files.
            response << "HTTP/1.1 200 OK\r\nContent-Length: " << length << "\r\n\r\n" << ifs.rdbuf();

            ifs.close();
        }
        else {
            string content="Could not open file "+filename;
            response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
        }
    };
    
    thread server_thread([&server](){server.start();});    
    this_thread::sleep_for(chrono::seconds(1));        
    server_thread.join();
}
