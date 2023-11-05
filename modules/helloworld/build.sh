#/bin/sh
echo "Название: Hello world"
echo "Лицензия: Публичное достояние"
gcc -mcmodel=large -I../../modlib -O0 -finput-charset=UTF-8 -fexec-charset=cp1251 -c -fPIC -nostdlib main.c -o hello.o
gcc -mcmodel=large -Wl,--entry=init -fPIC -shared -nostdlib hello.o -o hello.ko
cp hello.ko ../bin/
echo "Сборка завершена, файл: hello.ko"
