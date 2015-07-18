//************************************
//FUNCION: void Date(Hora , Fecha)
//PARAMETROS: Puntero de hora y fecha actual
//RETORNO: n/a
//DESCRIPCION: Actualiza por referencia la hora/fecha actual
//FUNCIONES INVOCADAS: n/a
//*************************************	
	
void Date(char *t,char *d){
	
	time_t tSac = time(NULL);// instante actual
	struct tm* tmP = localtime(&tSac);
	char min[3],sec[3];
	char mon[3],day[3];
			
	sprintf(t, "%d", tmP->tm_hour);
	sprintf(min, "%d", tmP->tm_min);
	sprintf(sec, "%d", tmP->tm_sec);
			
	sprintf(d, "%d", tmP->tm_year+1900);
	sprintf(mon, "%d", tmP->tm_mon+1);
	sprintf(day, "%d", tmP->tm_mday);	
			
	strcat(t,":");
	strcat(t,min);
	strcat(t,":");
	strcat(t,sec);
			
	strcat(d,"/");
	strcat(d,mon);
	strcat(d,"/");
	strcat(d,day);	
	}
