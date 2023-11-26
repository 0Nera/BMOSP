#/bin/sh
echo "Название: SIMD"
echo "Лицензия: Публичное достояние"

CC="gcc"
ARCH_FLAGS="-ffreestanding -O0 -g -fPIC -shared -nostdlib "

if [ -d "../../sdk" ]; then
    CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o simd.o
$CC $ARCH_FLAGS -T ../link.ld -Wl,--entry=init,--build-id=none simd.o -o simd.ko

cp simd.ko ../bin/
echo "Сборка завершена, файл: simd.ko"
