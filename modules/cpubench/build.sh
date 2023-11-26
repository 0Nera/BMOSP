#/bin/sh
echo "Название: CPUBENCH"
echo "Лицензия: Публичное достояние"

CC="gcc"
ARCH_FLAGS="-ffreestanding -O0 -g -fPIC -shared -nostdlib "

if [ -d "../../sdk" ]; then
    CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o cpubench.o
$CC $ARCH_FLAGS -T ../link.ld -Wl,--entry=init,--build-id=none cpubench.o -o cpubench.ko
cp cpubench.ko ../bin/
echo "Сборка завершена, файл: cpubench.ko"
