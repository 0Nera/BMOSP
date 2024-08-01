#!/bin/bash

# URL исходного репозитория
SOURCE_REPO="https://github.com/0Nera/BMOSP"

# Добавляем исходный репозиторий как удаленный
git remote add source $SOURCE_REPO

# Получаем все ветки и теги из исходного репозитория
git fetch source

# Синхронизируем все ветки и теги с исходным репозиторием
for branch in $(git branch -r | grep 'source/' | sed 's/source\///'); do
    git checkout -b $branch source/$branch
done

git push --all origin
git push --tags origin

# Удаляем удаленный репозиторий
git remote remove source