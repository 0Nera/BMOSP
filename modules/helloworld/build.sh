#/bin/sh
echo "Название: Hello world"
echo "Лицензия: Публичное достояние"
gcc -I../../modlib -O0 -finput-charset=UTF-8 -fexec-charset=cp1251 -c -fPIC -nostdlib main.c -o hello.o
gcc  -Wl,--entry=init -fPIC -shared -nostdlib hello.o -o hello.so
echo "Сборка завершена, файл: hello.so"