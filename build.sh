#!/bin/sh
cd modules/
mkdir -p bin
dos2unix */*.sh
#cd helloworld/ && chmod +x build.sh && ./build.sh && cd ..
#cd music/ && chmod +x build.sh && ./build.sh && cd ..
cd simd/ && chmod +x build.sh && ./build.sh && cd ..
cd cpubench/ && chmod +x build.sh && ./build.sh && cd ..
cd pci/ && chmod +x build.sh && ./build.sh && cd ..
cd pci_data/ && chmod +x build.sh && ./build.sh && cd ..
cd ..
python3 pbuild.py