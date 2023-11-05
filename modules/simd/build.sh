#/bin/sh
echo "Название: SIMD"
echo "Лицензия: Публичное достояние"
gcc -I../../modlib -O0 -finput-charset=UTF-8 -fexec-charset=cp1251 -c -fPIC -nostdlib main.c -o simd.o
gcc  -Wl,--entry=init -fPIC -shared -nostdlib simd.o -o simd.ko
cp simd.ko ../bin/
echo "Сборка завершена, файл: simd.ko"
