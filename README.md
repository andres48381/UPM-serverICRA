# Aplicación serverICRA #

## Proyecto C++ para el desarrollo de la aplicación servidor ##

Compilación:

g++ -std=c++11 -Iserial -I /usr/include/mysql++/ -I /usr/include/mysql -c -o serverICRA.o serverICRA.cpp

g++ -o serverICRA serverICRA.o -lserial -pthread -lmysqlpp -lboost_system 