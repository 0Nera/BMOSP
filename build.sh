#!/bin/sh
cd modules/
dos2unix */*.sh
cd helloworld/ && chmod +x build.sh && ./build.sh && cd ..
cd music/ && chmod +x build.sh && ./build.sh && cd ..
cd simd/ && chmod +x build.sh && ./build.sh && cd ..
cd cpubench/ && chmod +x build.sh && ./build.sh && cd ..
cd ..
python3 pbuild.py