#include <sstream>
#include <string>

class InvernaderoServer{
	
	private:
		//Variables Arduino
		string port;	
		char t[10],h[10],a[10],panel[5],vent[2],calef[2],humidi[2],state[2];
		//Format messages from ARDUINO	
		string t0,t1,T0,T1,h0,h1,H0,H1,mode,estado;				
		string CMD;//Data
		string CMD_0;//Adjuntment
	public:
		InvernaderoServer();
		void SetPort();
		string GetPort(){return port;}
		string GetCMD_0(){return CMD_0;}
		string GetCMD(){return CMD;}
		string SendMessage();
		void AdqAdjustment(char );		
		void AdqData(char );
		void Monitor();	
		string InsertMySQL(string ,char [],char []);	
		string JsonMonitor();	
		string JsonRangos();
		string JsonPanel();		
		string JsonAlarmas();		
		string JsonSetLim(string ,string);	
		string JsonSetVent(string );
		string JsonSetSwitch(string );
	};	
	
InvernaderoServer::InvernaderoServer(){
	t0="00.00";
	t1="00.00";
	T0="00.00";
	T1="00.00";
	h0="00.00";
	h1="00.00";
	H0="00.00";
	H1="00.00";
	mode="0";
	estado="0";		
	//Format messages from ARDUINO	
	CMD="Txx.xxHxx.xxAxxxVxCxWxPxxxSx";
	CMD_0="axx.xxbxx.xxcxx.xxdxx.xxexx.xxfxx.xxgxx.xxhxx.xxixjx";						
}	

void InvernaderoServer::SetPort(){
	
	//Solicitud de puertoSerie
	cout<<"\033[1;31mCompruebe que ICRA esté conectado al puerto serie del equipo\033[0m"<<endl;
	cout<<"Inserte Serial Port empleado:"<<endl;
	cin>>port;		
}

string InvernaderoServer::SendMessage(){			
	//ID cabecera
	string message="y";
	//Parametros usuario Android
	message+="a";message+=t0;
	message+="b";message+=t1;
	message+="c";message+=T0;
	message+="d";message+=T1;			
	message+="e";message+=h0;
	message+="f";message+=h1;
	message+="g";message+=H0;
	message+="h";message+=H1;
	message+="i";message+=mode;
	message+="j";message+=estado;	
	cout<<"Send: "<<message<<endl;			
	return message;
	
}

void InvernaderoServer::AdqAdjustment(char a){
	
	string *p;//puntero recepcion
	stringstream ss;
	string c;
	ss<<a;
	ss>>c;
	char _t0[6],_t1[6],_T0[6],_T1[6],_h0[6],_h1[6],_H0[6],_H1[6];
	bool i;//indice puntero p[ii] 
	//Identificacion de valores			
	if(c=="a"){//Temperatura: Rango inferior trabajo
		p=&t0;//asignacion del puntero
		i=false;//reset indice
	}
	else if(c=="b"){//Temperatura: Rango superior trabajo
		p=&t1;
		i=false;//reset indice
	}
	else if(c=="c"){//Temperatura: Rango inferior alarma
		p=&T0;
		i=false;//reset indice
	}
	else if(c=="d"){//Temperatura: Rango superior alarma
		p=&T1;
		i=false;//reset indice
	}
	else if(c=="e"){//Humedad: Rango inferior trabajo
		p=&h0;
		i=false;//reset indice
	}
	else if(c=="f"){//Humedad: Rango superior trabajo
		p=&h1;
		i=false;//reset indice
	}
	else if(c=="g"){//Humedad: Rango inferior alarma
		p=&H0;
		i=false;//reset indice
	}
	else if(c=="h"){//Humedad: Rango superior alarma
		p=&H1;//Parada general
		i=false;//reset indice
	}	
	else if(c=="i"){//Humedad: Rango superior alarma
		p=&mode;//Parada general
		i=false;//reset indice
	}	
	else if(c=="j"){//Humedad: Rango superior alarma
		p=&estado;//Parada general
		i=false;//reset indice
	}		
	else {//Recogida del valor
		if(i==false){
			*p=c;
			i=true;
		}
		else *p+=c;
	}			
}
	
void InvernaderoServer::AdqData(char c){
	
	char *p;//puntero recepcion
	int i;//indice puntero p[ii] 
	//Identificacion de valores			
	if(c=='T'){//Temperatura
		p=t;//asignacion del puntero
		i=0;//reset indice
	}
	else if(c=='H'){//Humedad
		p=h;
		i=0;
	}
	else if(c=='A'){//Aire
		p=a;
		i=0;
	}
	else if(c=='V'){//Ventilador
		p=vent;
		i=0;
	}
	else if(c=='C'){//Calefaccion
		p=calef;
		i=0;
	}
	else if(c=='W'){//Humidificador
		p=humidi;
		i=0;
	}
	else if(c=='P'){//Alarmas
		p=panel;
		i=0;
	}
	else if(c=='S'){//Estado
		p=state;//Parada general
		i=0;
	}	
	else {//Recogida del valor
		if(c==' ')c='0';//Sustitucion ' ' por '0'
		p[i++]=c;//Obtencion del valor
		p[i]='\0';//Fin de cadena
	}			
}

void InvernaderoServer::Monitor(){
	
	//SALIDA POR MONITOR
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;
	cout<<"\033[0;32m***************\033[0m\033[1;32m      PROYECTO ICRA       \033[0m\033[0;32m***************\033[0m"<<endl;	
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;
	cout<<"..."<<endl;
	cout<<" Receive: "<<endl;
	cout<<endl;												
	cout<<" Niveles Temperatura: "<<t<<" ºC"<<endl;
	cout<<" Niveles Humedad: "<<h<<" %"<<endl;
	cout<<" Estado del aire: "<<a<<endl;
	cout<<" Modo del ventilador: "<<vent<<endl;
	cout<<" Sistema de calefaccion: "<<calef<<endl;
	cout<<" Humidificador: "<<humidi<<endl;
	cout<<" ALARMAS: "<<panel<<endl;		
	cout<<" PAUSE: "<<state<<endl;	
						 
}

string InvernaderoServer::InsertMySQL(string tabla,char time[],char date[]){
	
	string aux="INSERT INTO `"+tabla+"` VALUES (NULL, '"+time+"', '"+date+"', '"+t+"', '"+t0+"', '"+t1+
	"', '"+T0+"', '"+T1+"', '"+h+"', '"+h0+"', '"+h1+"', '"+H0+"', '"+H1+"', '"+a+"', '"+vent+"', '"+calef+
	"', '"+humidi+"', '"+panel[0]+"', '"+panel[1]+"', '"+panel[2]+"', '"+state+"');";
	return(aux);	
		
}

string InvernaderoServer::JsonMonitor(){
	
	//text Android
	/*strcpy(t,"25.60");
	strcpy(h,"50.00");
	strcpy(a,"220");
	strcpy(calef,"0");
	strcpy(humidi,"1");
	strcpy(vent,"3");
	strcpy(panel,"001");*/
//////////////

	if(a[0]=='0'){	//Cambio de formato (a<100): 0xx->xx	
		a[0]=a[1];
		a[1]=a[2];
		a[2]='\0';		
		}
		
	if(a[0]=='0'){	//Cambio de formato (a<10): 0x->x	
		a[0]=a[1];
		a[1]='\0';		
		}	
	
	string json="{temp: ";
	json+=t;
	json+=", hum: ";
	json+=h;
	json+=", air: ";
	json+=a;
	json+=", calef: ";
	json+=calef;
	json+=", humidi: ";
	json+=humidi;		
	json+=", vent: ";
	json+=vent;	
	json+=", Atemp: ";
	json+=panel[0];		
 	json+=", Ahum: ";
	json+=panel[1];	
	json+=", Aair: ";
	json+=panel[2];
	json+=", switch: ";//Switch Android
	json+=estado;	
	json+="}";	
	
	return json;	
	}
	
string InvernaderoServer::JsonRangos(){
	
	string json="{t0: ";
	json+=t0;
	json+=", t1: ";
	json+=t1;
	json+=", h0: ";
	json+=h0;
	json+=", h1: ";
	json+=h1;	
	json+="}";	
	
	return json;
	}

string InvernaderoServer::JsonPanel(){
	
	string json="{Atemp: ";
	json+=panel[0];		
 	json+=", Ahum: ";
	json+=panel[1];	
	json+=", Aair: ";
	json+=panel[2];
	json+="}";
	
	return json;	
	}

string InvernaderoServer::JsonAlarmas(){
	
	string json="{temp: ";
	json+=t;
	json+=", t0a: ";
	json+=T0;
	json+=", t1a: ";
	json+=T1;
	json+=", hum: ";
	json+=h;
	json+=", h0a: ";
	json+=H0;
	json+=", h1a: ";
	json+=H1;	
	json+="}";	
	
	return json;
	}
	
string InvernaderoServer::JsonSetLim(string param,string val){
	
	string *p;
	string json="empty";
	
	if(param=="t0")p=&t0;
	else if(param=="t1")p=&t1;
	else if(param=="t0a")p=&T0;
	else if(param=="t1a")p=&T1;
	else if(param=="h0")p=&h0;	
	else if(param=="h1")p=&h1;
	else if(param=="h0a")p=&H0;
	else if(param=="h1a")p=&H1;		
	else p=NULL;
	
	float f=stof(val);//String to Float
	f/=100;//Receive xxxx. Convert to: xx.xx
	val=to_string(f);//Float to String	
	
	if(p!=NULL){
		*p=val.substr(0,5);//"xx.xx|xxx..." Split String		 
		json="{";
		json+=param;
		json+=":";
		json+=(*p);
		json+="}";
	}	
	return json;
}

string InvernaderoServer::JsonSetVent(string speed){
       
	mode=speed;      
	string json="{vent:";
	json+=mode;
	json+="}";	
	
	return json;
}

string InvernaderoServer::JsonSetSwitch(string sw){
	
	estado=sw;	
	string json="{switch:";//Swtich Android
	json+=estado;
	json+="}";	
	
	return json;
}
