//Librerias externas
#include <mysql++/mysql++.h>//MySQL
#include <iostream>
using namespace std;

//************************************
//FUNCION: string NewTable(Base de datos,tabla)
//PARAMETROS: Tabla a crear en Base de datos
//RETORNO: Sentencia para crear nueva tabla en MySQL
//DESCRIPCION: Concatenar strings para creacion nueva tabla
//FUNCIONES INVOCADAS: n/a
//*************************************
string NewTable(string _db,string _tabla){
	
	return("CREATE TABLE IF NOT EXISTS `"+_db+"`.`"+_tabla+"` ("
                      "`id` bigint(20) NOT NULL AUTO_INCREMENT,"
                      "`time` time NOT NULL,"
                      "`date` date NOT NULL,"
                      "`temperatura` varchar(10) NOT NULL,"
                      "`temp_min` varchar(10) NOT NULL," 
                      "`temp_max` varchar(10) NOT NULL,"
                      "`Atemp_min` varchar(10) NOT NULL," 
                      "`Atemp_max` varchar(10) NOT NULL,"                    
                      "`humedad` varchar(10) NOT NULL,"
                      "`hum_min` varchar(10) NOT NULL," 
                      "`hum_max` varchar(10) NOT NULL,"
                      "`Ahum_min` varchar(10) NOT NULL," 
                      "`Ahum_max` varchar(10) NOT NULL,"  
                      "`aire` varchar(10) NOT NULL,"
                      "`ventilador` varchar(5) NOT NULL,"
                      "`calefaccion` boolean NOT NULL,"
                      "`humidificador` boolean NOT NULL,"
                      "`alarmaT` boolean NOT NULL,"
                      "`alarmaH` boolean NOT NULL,"
                      "`alarmaG` boolean NOT NULL,"
                      "`pause` boolean NOT NULL,"
                      "PRIMARY KEY (`id`)"
                      ") ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;");   
	} 

//************************************
//FUNCION: string obtainID(string _date,string _time,string db,string tabla,mysqlpp::Query query)
//PARAMETROS: Parametros de busqueda: fecha y hora. Datos de conexión.
//RETORNO: ID localizado
//DESCRIPCION: Localización del identificador de fecha y hora indicadas
//FUNCIONES INVOCADAS: MySQL functions
//*************************************	
	
	string obtainID(string _date,string _time,string db,string tabla,mysqlpp::Query query){
		
		//Date 
		string ID_date=_date.substr(0,4);//"|xxxx|xxxx" Split String
		ID_date+="/";
		ID_date+=_date.substr(4,2);//"xxxx|xx|xx..." Split String
		ID_date+="/";
		ID_date+=_date.substr(6,2);//"xxxxxx|xx|" Split String		
		// xxxx/xx/xx	
				
		//Time Inicio
		string ID_time=_time.substr(0,2);//"|xx|xxxx" Split String
		ID_time+=":";
		ID_time+=_time.substr(2,2);//"xx|xx|xx..." Split String
		ID_time+=":";
		ID_time+=_time.substr(4,2);//"xxxx|xx|" Split String	
		// xx:xx:xx		
		
		string ID;
		
		query.reset();
		query<<"SELECT id FROM `"+db+"`.`"+tabla+"` WHERE time='"+ID_time+"' AND date='"+ID_date+"'";
		mysqlpp::StoreQueryResult res=query.store();
		
		if (res.num_rows() >0)//Valores encontrados
		{
			mysqlpp::StoreQueryResult::iterator it=res.begin();		 
			mysqlpp::String resul=(it)->at(0);//Obtencion de temperatura 		
			resul.to_string(ID);				  					 
		}
		else
			ID="empty";	
			
		return ID;				
		}		
	
//************************************
//FUNCION: string jsonArray(string id0,string id1,string f,string db,string tabla,mysqlpp::Query query)
//PARAMETROS: Parametros de busqueda: ID0-ID1. Datos de conexión.
//RETORNO: jsonArray formado por time,campo1,campo2
//DESCRIPCION: Extracción de valores de MySQL comprendidos en un rango de IDs
//FUNCIONES INVOCADAS: MySQL functions
//*************************************	

	string jsonArray(string id0,string id1,string f1,string f2,string db,string tabla,mysqlpp::Query query){
		
		mysqlpp::String mysqlpp_field,mysqlpp_val;
		string _field,_val;
		string resul="{grafica:[";//inicio del jsonArray
		query.reset();
		string sentence;
		//2 parametros
		if(f2!="empty")sentence="SELECT time,"+f1+","+f2+" FROM `"+db+"`.`"+tabla+"` WHERE id>='"+id0+"' AND id<='"+id1+"'";
		//1 parametro
		else sentence="SELECT time,"+f1+" FROM `"+db+"`.`"+tabla+"` WHERE id>='"+id0+"' AND id<='"+id1+"'";
		//query<<"SELECT time,"+f1+","+f2+" FROM `"+db+"`.`"+tabla+"` WHERE id>='"+id0+"' AND id<='"+id1+"'";
		query<<sentence;
		mysqlpp::StoreQueryResult res=query.store();
		
		 if (res.num_rows() >0)//Comprueba existencia de valores
		   {

			 for (mysqlpp::StoreQueryResult::iterator it=res.begin(); it!=res.end(); ++it)
			   {
				 if(it!=res.begin())resul+=",";//Separador de nueva componente 
				 
				 resul+="{";//Nueva componente del jsonArray
				 
				 for (unsigned i=0; i<res.num_fields(); i++){
					 
				   mysqlpp_field=res.field_name(i);//String mysqlpp <<< MySQL
				   mysqlpp_field.to_string(_field);//string <<< String mysqlpp				   
				   			
				   mysqlpp_val=(it)->at(i);//String mysqlpp <<< MySQL
				   mysqlpp_val.to_string(_val);//string <<< String mysqlpp
				   
				   if(_field=="time"){//Formato Time hh:mm:ss->ss				   
						string aux_time=_val.substr(0,2);//"|xx|:xx:xx" Split String
						int time=stoi(aux_time)*3600;//Hour
						aux_time=_val.substr(3,2);//"xx:|xx|:xx..." Split String
						time+=stoi(aux_time)*60;//Minutes
						aux_time=_val.substr(6,2);//"xx:xx:|xx|" Split String
						time+=stoi(aux_time);//Seg				   
						_val=to_string(time);
					   }				   
				   
				   string component=_field+":"+_val;//componente del jsonArray	
				   
				   if(i!=(res.num_fields()-1))//Evalua si quedan más campos
						component+=",";//Separador de nuevo campo
											
				   resul+=component;
				   }
				 resul+="}";//cierre de componente del jsonArray
				   
			   }
			   resul+="]}";//cierre del jsonArray
		   }
		   else
				resul="empty";
		   	
		return resul;//Return jsonArray: {grafica:[{time: ,valor: },{...}]}				
		}
	
