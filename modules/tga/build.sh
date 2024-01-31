#/bin/sh
echo "Название: TGA"
echo "Лицензия: Публичное достояние"

CC="gcc"
ARCH_FLAGS="-fno-stack-protector -ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o tga.o
$CC $ARCH_FLAGS -Wl,--entry=init,--build-id=none -T ../link.ld tga.o -L../../modlib/lib/ -lmod -o tga.ko

cp boot.jpg ../bin/
cp boot.tga ../bin/
cp tga.ko ../bin/
echo "Сборка завершена, файл: tga.ko"
