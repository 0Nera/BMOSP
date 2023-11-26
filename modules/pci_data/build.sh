#/bin/sh
echo "Название: список устройств и производителей для PCI"
echo "Лицензия: Публичное достояние"
gcc -I../../modlib -O0 -finput-charset=UTF-8 -fexec-charset=cp1251 -c -fPIC -nostdlib -Wno-builtin-declaration-mismatch main.c -o pci_data.o
gcc  -Wl,--entry=init,--build-id=none -fPIC -shared -nostdlib  pci_data.o -o pci_data.ko
cp pci_data.ko ../bin/
cp pci_vendors.txt ../bin/
echo "Сборка завершена, файл: pci_data.ko"
