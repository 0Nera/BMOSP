#/bin/sh
echo "Название: PCI"
echo "Лицензия: Публичное достояние"


CC=${CC:-gcc}
ARCH_FLAGS="-fno-stack-protector -ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o pci.o
$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c pci_data.c -o pci_data.o
$CC $ARCH_FLAGS -Wl,--entry=init,--build-id=none -T ../link.ld pci.o -L../../modlib/lib/ -lmod -o pci.ko
$CC $ARCH_FLAGS -Wl,--entry=init,--build-id=none -T ../link.ld pci_data.o -L../../modlib/lib/ -lmod -o pci_data.ko

cp pci.ko ../bin/
cp pci_data.ko ../bin/
cp pci_vendors.txt ../bin/

echo "Сборка завершена, файл: pci.ko"
