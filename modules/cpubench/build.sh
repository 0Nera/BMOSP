#/bin/sh
echo "Название: CPUBENCH"
echo "Лицензия: Публичное достояние"

CC=${CC:-gcc}
ARCH_FLAGS="-fno-stack-protector -ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o cpubench.o
$CC $ARCH_FLAGS -Wl,--entry=init,--build-id=none -T ../link.ld cpubench.o -L../../modlib/lib/ -lmod -o cpubench.ko
cp cpubench.ko ../bin/
echo "Сборка завершена, файл: cpubench.ko"
