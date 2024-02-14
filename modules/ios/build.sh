#/bin/sh
echo "Название: IOS"
echo "Лицензия: CC BY-NC 4.0"

CC=${CC:-gcc}
ARCH_FLAGS="-fno-stack-protector -ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
fi


$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c main.c -o ios.o
$CC $ARCH_FLAGS -Wl,--entry=init,--build-id=none -T ../link.ld ios.o -L../../modlib/lib/ -lmod -o ios.ko

cp ios.ko ../bin/
echo "Сборка завершена, файл: ios.ko"
