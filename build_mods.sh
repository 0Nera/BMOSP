#!/bin/sh

dos2unix *.sh

cd modlib/lib/
dos2unix build.sh
chmod +x build.sh
./build.sh
cd ../..

cd modules/
mkdir -p bin
dos2unix */*.sh
chmod +x */build.sh

for dir in */; do
	if [ $dir != "bin/" ]; then
		cd $dir
		./build.sh
		cd ..
	fi
done

cd ..
