#Sript para compilación y ejecución de serverICRA

echo "Limpiando versiones anteriores..."
rm serverICRA.o
rm /var/www/serverICRA/serverICRA
echo "Compilando versión actual..."
g++ -std=c++11 -Iserial -I /usr/include/mysql++/ -I /usr/include/mysql -c -o serverICRA.o serverICRA.cpp
g++ -o serverICRA serverICRA.o -lserial -pthread -lmysqlpp -lboost_system
echo "Instalando versión actual..."
cp serverICRA /var/www/serverICRA
echo "Lanzando aplicación..."
./serverICRA
