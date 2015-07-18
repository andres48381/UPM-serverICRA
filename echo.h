//************************************
//FUNCION: void echo(bool on)
//PARAMETROS: ON/OFF Visibilidad caracteres
//RETORNO: n/a
//DESCRIPCION: Ocultar caracteres escritos por teclado
//FUNCIONES INVOCADAS: n/a
//*************************************
	
void echo( bool on = true )
  {
  struct termios settings;
  tcgetattr( STDIN_FILENO, &settings );
  settings.c_lflag = on
                   ? (settings.c_lflag |   ECHO )
                   : (settings.c_lflag & ~(ECHO));
  tcsetattr( STDIN_FILENO, TCSANOW, &settings );
  }
