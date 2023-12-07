#/bin/sh
echo "Название: PCI"
echo "Лицензия: Публичное достояние"


CC="gcc"
ARCH_FLAGS="-ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o pci.o
$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c pci_data.c -o pci_data.o
$CC $ARCH_FLAGS -T ../link.ld -Wl,--entry=init,--build-id=none pci.o -o pci.ko
$CC $ARCH_FLAGS -T ../link.ld -Wl,--entry=init,--build-id=none pci_data.o -o pci_data.ko

cp pci.ko ../bin/
cp pci_data.ko ../bin/
cp pci_vendors.txt ../bin/

echo "Сборка завершена, файл: pci.ko"
