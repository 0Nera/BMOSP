#/bin/sh

CC="gcc"
AR="ar"
ARCH_FLAGS="-ffreestanding -O0 -g -fPIC -static -nostdlib "

if [ -d "../../sdk" ]; then
	CC="../../sdk/bin/x86_64-elf-gcc"
	AR="../../sdk/bin/x86_64-elf-ar"
fi

mkdir -p bin

$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c system.c -o bin/system.o
$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c tool.c -o bin/tool.o
$AR -rcs ../../modlib/lib/libmod.a ../../modlib/lib/bin/system.o ../../modlib/lib/bin/tool.o
ranlib ../../modlib/lib/libmod.a