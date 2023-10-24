# БМПОС: Базовая Модульная Платформа Операционных Систем

[![CI сборка](https://github.com/0Nera/BMOSP/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/0Nera/BMOSP/actions/workflows/build.yml)
[![Github pages сайт](https://github.com/0Nera/BMOSP/actions/workflows/pages/pages-build-deployment/badge.svg?branch=pages)](https://github.com/0Nera/BMOSP/actions/workflows/pages/pages-build-deployment)

БМПОС - Базовая Модульная Платформа Операционных Систем для платформы x86_64 (BIOS/UEFI). Это отечественное программное обеспечение, созданное при поддержке Синапс ОС на языке программирования C.

БМПОС не является операционной системой. Это платформа для изучения.

Философия БМПОС - "всё есть модуль".

![Скриншот вывода ядра в эмуляторе Qemu](https://0nera.github.io/BMOSP/assets/0_0.1.231.png)

## Реализовано

Ядро:

- [x] Менеджер памяти
- [x] Менеджер видеопамяти
- [ ] Менеджер потоков
- [x] Загрузчик модулей
- [ ] Буфферы ввода-вывода

Модули:

- [ ] Оболочка ввода-вывода

Драйвера:

- [ ] PS/2 (Клавиатура)
- [ ] SATA (AHCI) (Чтение)
- [ ] EXT2

Документация:

- [X] Системные вызовы
- [ ] Ядро
- [ ] Стандартная библиотека

Вебсайт:

- [X] Домен
- [ ] Движок сайта
- [ ] Документация
- [ ] Статьи
- [ ] Примеры

Примеры:

- [ ] Пример модуля
- [ ] Пример драйвера
- [ ] Пример программы-модуля
- [ ] Пример модуля ввода-вывода

Видеоуроки:

- [ ] Основы языка C
- [ ] Основы ассемблера GAS
- [ ] Настройка окружения
- [ ] Сборка из исходного кода
- [ ] Привет мир!
- [ ] Написание драйвера

Общая работа:

- [ ] Ядро
- [ ] Модули
- [ ] Драйвера
- [ ] Документация
- [ ] Вебсайт
- [ ] Примеры
- [ ] Видеоуроки

### Второстепенные задачи

Модули:

- [ ] Отладчик
- [ ] JavaScript
- [ ] Lua
- [ ] SQLite
- [ ] Криптограф

Драйвера:

- [ ] PS/2 (Мышь)
- [ ] SATA (AHCI) (Запись)
- [ ] SIMD (SSE, SSE2, SSE3, SSE4, AVX) - отдельный драйвер для инициализации SIMD инструкций

## Партнеры

- ООО "НПО ТЕ-ОН"

## Сборка из исходного кода

### Ubuntu 18.04+

```bash
sudo apt install clang-format python3 git qemu-system-x86 gdisk dos2unix
git clone https://git.synapseos.ru/Aren/BMOSP.git
cd BMOSP/
chmod +x build.sh
./build.sh
```

### ArchLinux

```bash
yay -S clang-format
sudo pacman -S python3 git qemu-system-x86
git clone https://git.synapseos.ru/Aren/BMOSP.git
cd BMOSP/
chmod +x build.sh
./build.sh
```

## Запуск

### Qemu

Стандартная конфигурация

```bash
qemu-system-x86_64 -cpu max -m 1G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd  -name "БМПОС"
```

Или

```bash
chmod +x run.sh
./run.sh
```

## Предупреждение

Использовать на свой страх и риск. Система расчитана на работу в эмулируемой среде Qemu под процессоры x86_64.
Мы не несем ответственности за нанесенный ущерб.

## Ресурсы

- <https://vk.com/BMOSP> Страница вконтакте
- <https://t.me/bmosp> Телеграм
- <https://bmosp.ru> Вебсайт
- <https://wiki.synapseos.ru/index.php?title=БМПОС> Страница на вики

### Зеркала

- <https://git.synapseos.ru/Aren/BMOSP> - доверенный сервер(главный репозиторий)
- <https://github.com/0Nera/BMOSP> - зеркало
- <https://tvoygit.ru/0Nera/BMOSP> - зеркало
- <https://hub.mos.ru/synapseos/BMOSP> - неактивное зеркало

### Использованные ресурсы

- <https://github.com/limine-bootloader/limine> (BSD 2-Clause)
- <https://github.com/nothings/stb> (MIT, Общественное достояние)
- <https://en.wikipedia.org/wiki/CPUID>
- <https://github.com/klange/toaruos> (NCSA)
- <https://wiki.osdev.org/Model_Specific_Registers>
- <https://sandpile.org/x86/msr.htm>
