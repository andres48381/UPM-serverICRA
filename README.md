# Aplicación serverICRA #

## Proyecto C++ para el desarrollo de la aplicación servidor ##

Compilación:

g++ -std=c++11 -Iserial -I /usr/include/mysql++/ -I /usr/include/mysql -c -o serverICRA.o serverICRA.cpp

g++ -o serverICRA serverICRA.o -lserial -pthread -lmysqlpp -lboost_system

![755618196-servidor-dedicado-linux.png](https://bitbucket.org/repo/EMGX8y/images/609094637-755618196-servidor-dedicado-linux.png)