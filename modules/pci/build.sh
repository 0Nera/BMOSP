#/bin/sh
echo "Название: PCI"
echo "Лицензия: Публичное достояние"
gcc -I../../modlib -O0 -finput-charset=UTF-8 -fexec-charset=cp1251 -c -fPIC -nostdlib main.c -o pci.o
gcc  -Wl,--entry=init -fPIC -shared -nostdlib pci.o -o pci.ko
cp pci.ko ../bin/
echo "Сборка завершена, файл: pci.ko"
