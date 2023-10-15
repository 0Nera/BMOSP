# БМПОС: Базовая Модульная Платформа Операционных Систем

[![CI сборка](https://github.com/0Nera/BMOSP/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/0Nera/BMOSP/actions/workflows/build.yml)
[![Github pages сайт](https://github.com/0Nera/BMOSP/actions/workflows/pages/pages-build-deployment/badge.svg?branch=pages)](https://github.com/0Nera/BMOSP/actions/workflows/pages/pages-build-deployment)

БМПОС - Базовая Модульная Платформа Операционных Систем для платформы x86_64 (BIOS/UEFI). Это отечественное программное обеспечение, созданное при поддержке Синапс ОС на языке программирования C++.

## Реализовано 

Ядро:
- [x] Менеджер памяти
- [x] Менеджер видеопамяти
- [ ] Менеджер потоков
- [x] Загрузчик модулей


Модули:
- [ ] Оболочка ввода-вывода


Драйвера:
- [ ] COM
- [ ] PS/2 (Клавиатура)
- [ ] SATA (ACHI) (Чтение)
- [ ] EXT2

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
- [ ] SATA (ACHI) (Запись)
- [ ] ISOFS (ISO 9660)
- [ ] SIMD (SSE, SSE2, SSE3, SSE4, AVX)

## Партнеры

- ООО "НПО ТЕ-ОН"

## Предупреждение

Использовать на свой страх и риск. Система расчитана на работу в эмулируемой среде Qemu под процессоры x86_64.
Мы не несем ответственности за нанесенный ущерб.

## Ресурсы

- <https://vk.com/mseos> Страница вконтакте
- <https://mseos.ru> Вебсайт
- <https://wiki.synapseos.ru/index.php?title=БМПОС> Страница на вики

### Зеркала

- <https://git.synapseos.ru/Aren/BMOSP> - доверенный сервер(главный репозиторий)
- <https://github.com/0Nera/BMOSP> - зеркало
- <https://tvoygit.ru/0Nera/BMOSP> - зеркало
- <https://hub.mos.ru/synapseos/BMOSP> - неактивное зеркало

### Использованные ресурсы

- https://github.com/limine-bootloader/limine (BSD 2-Clause)
- https://github.com/nothings/stb (MIT, Общественное достояние)
- https://en.wikipedia.org/wiki/CPUID
- https://github.com/klange/toaruos (NCSA)
- https://wiki.osdev.org/Model_Specific_Registers
- https://sandpile.org/x86/msr.htm