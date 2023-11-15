#!/bin/bash

mkdir -p sdk
cd sdk

echo "Updating..."

sudo apt-get update
sudo apt-get -y install build-essential g++ make bison flex texinfo libgmp-dev libmpc-dev libmpfr-dev libisl-dev
if [ ! -d "gcc-12.3.0" ]; then
    echo "Installing gcc..."
    wget https://ftp.gnu.org/gnu/gcc/gcc-12.3.0/gcc-12.3.0.tar.xz
    tar -xf gcc-12.3.0.tar.xz
fi
cd gcc-12.3.0/
echo "Configure..."
contrib/download_prerequisites
mkdir -p build
cd build
pwd
../configure --target=x86_64-elf --prefix=$PWD/cross --disable-nls --enable-languages=c --without-headers
echo "Build GCC..."
make all-gcc -j$(nproc)
echo "Build all-target-libgcc..."
make all-target-libgcc -j$(nproc)
echo "install-gcc..."
make install-gcc
echo "install-target-libgcc..."
make install-target-libgcc