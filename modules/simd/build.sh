#/bin/sh
echo "Название: SIMD"
echo "Лицензия: Публичное достояние"

CC="gcc"
ARCH_FLAGS="-ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o simd.o
$CC $ARCH_FLAGS -Wl,--entry=init,--build-id=none -T ../link.ld simd.o -L../../modlib/lib/ -lmod -o simd.ko

cp simd.ko ../bin/
echo "Сборка завершена, файл: simd.ko"
