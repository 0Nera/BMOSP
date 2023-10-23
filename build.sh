#!/bin/sh
cd modules/
cd helloworld/ && chmod +x build.sh && ./build.sh && cd ..
cd music/ && chmod +x build.sh && ./build.sh && cd ..
cd ..
python3 pbuild.py