#!/bin/sh

cd modules/
mkdir -p bin
dos2unix */*.sh
chmod +x */build.sh

for dir in */; do
	if [ $dir != "bin/" ]; then
		cd $dir && ./build.sh && cd ..
	fi
done

cd ..

python3 pbuild.py