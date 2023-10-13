#/bin/sh
echo "Название: Hello world"
echo "Лицензия: Публичное достояние"
g++ -c -fPIC -nostdlib main.cpp -o hello.o
gcc -shared -nostdlib hello.o -o hello.so -Wl,--entry=_start
echo "Сборка завершена, файл: hello.so"
