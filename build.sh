#!/bin/sh
cd modules/
cd helloworld/ && ./build.sh && cd ..
cd music/ && ./build.sh && cd ..
cd ..
python3 pbuild.py