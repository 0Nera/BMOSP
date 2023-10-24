#/bin/sh
echo "Название: Мелодия из тетриса"
echo "Лицензия: Публичное достояние"
gcc -I../../modlib -O0 -finput-charset=UTF-8 -fexec-charset=cp1251 -c -fPIC -nostdlib main.c -o music.o
gcc  -Wl,--entry=init -fPIC -shared -nostdlib music.o -o music.so
echo "Сборка завершена, файл: music.so"
