#/bin/sh
echo "Название: CPUBENCH"
echo "Лицензия: Публичное достояние"
gcc -I../../modlib -O0 -finput-charset=UTF-8 -fexec-charset=cp1251 -c -fPIC -nostdlib main.c -o cpubench.o
gcc  -Wl,--entry=init -fPIC -shared -nostdlib cpubench.o -o cpubench.ko
cp cpubench.ko ../bin/
echo "Сборка завершена, файл: cpubench.ko"
