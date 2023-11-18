#!/bin/sh

url="https://github.com/0Nera/i686-elf-tools/releases/download/13.2.0/x86_64-elf-tools-linux.zip"
filename="x86_64-elf-tools-linux.zip"
foldername="sdk"

# Проверяем, существует ли архив
if [ ! -f "$filename" ]; then
    # Скачиваем архив
    wget "$url"
fi

# Распаковываем содержимое в папку sdk/
unzip "$filename" -d "$foldername"