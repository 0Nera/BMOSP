#/bin/sh
echo "Название: DOOM"
echo "Лицензия: Публичное достояние"

CC=${CC:-gcc}
ARCH_FLAGS="-fno-stack-protector -ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi

if [ ! -f "PureDOOM.h" ]; then
	wget https://github.com/Daivuk/PureDOOM/raw/refs/heads/master/PureDOOM.h
fi
if [ ! -f "doom1.wad" ]; then
	wget https://github.com/Daivuk/PureDOOM/raw/refs/heads/master/doom1.wad
fi

$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o doom.o
$CC $ARCH_FLAGS -Wl,--entry=init,--build-id=none -T ../link.ld doom.o -L../../modlib/lib/ -lmod -o doom.ko

cp doom.ko ../bin/
cp doom1.wad ../bin/
echo "Сборка завершена, файл: doom.ko"
