#/bin/sh
echo "Название: PS/2"
echo "Лицензия: Публичное достояние"


CC="gcc"
ARCH_FLAGS="-fno-stack-protector -ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o ps2.o
$CC $ARCH_FLAGS -Wl,--entry=init,--build-id=none -T ../link.ld ps2.o -L../../modlib/lib/ -lmod -o ps2.ko

cp ps2.ko ../bin/

echo "Сборка завершена, файл: ps2.ko"
