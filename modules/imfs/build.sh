#/bin/sh
echo "Название: IMFS"
echo "Лицензия: CC BY-NC 4.0"

CC="gcc"
ARCH_FLAGS="-fno-stack-protector -ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o imfs.o
$CC $ARCH_FLAGS -Wl,--entry=init,--build-id=none -T ../link.ld imfs.o -L../../modlib/lib/ -lmod -o imfs.ko

cp imfs.ko ../bin/
echo "Сборка завершена, файл: imfs.ko"
