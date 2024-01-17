#/bin/sh
echo "Название: TGA"
echo "Лицензия: Публичное достояние"

CC="gcc"
ARCH_FLAGS="-ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o tga.o
$CC $ARCH_FLAGS -T ../link.ld -Wl,--entry=init,--build-id=none tga.o -o tga.ko

cp boot.jpg ../bin/
cp boot.tga ../bin/
cp tga.ko ../bin/
echo "Сборка завершена, файл: tga.ko"
