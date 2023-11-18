#/bin/sh
echo "Название: PCI"
echo "Лицензия: Публичное достояние"


CC="gcc"
ARCH_FLAGS="-ffreestanding -O0 -g -fPIC -shared -nostdlib"

if [ -d "../../sdk" ]; then
    CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o pci.o
$CC $ARCH_FLAGS -T ../link.ld -Wl,--entry=init pci.o -o pci.ko

cp pci.ko ../bin/
echo "Сборка завершена, файл: pci.ko"
