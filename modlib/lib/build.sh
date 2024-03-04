#/bin/sh
CC=${CC:-gcc}
AR=${AR:-ar}
ARCH_FLAGS="-fno-stack-protector -ffreestanding -O0 -g -fPIC -static -nostdlib "

mkdir -p bin

$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c system.c -o bin/system.o
$CC $ARCH_FLAGS -I../../modlib -finput-charset=UTF-8 -fexec-charset=cp1251 -c tool.c -o bin/tool.o
$AR -rcs ../../modlib/lib/libmod.a ../../modlib/lib/bin/system.o ../../modlib/lib/bin/tool.o
ranlib ../../modlib/lib/libmod.a