# БМПОС: Базовая Модульная Платформа Операционных Систем

[![CI сборка](https://github.com/0Nera/BMOSP/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/0Nera/BMOSP/actions/workflows/build.yml)
[![CodeQL](https://github.com/0Nera/BMOSP/actions/workflows/codeql.yml/badge.svg?branch=master)](https://github.com/0Nera/BMOSP/actions/workflows/codeql.yml)
[![Github pages сайт](https://github.com/0Nera/BMOSP/actions/workflows/pages/pages-build-deployment/badge.svg?branch=pages)](https://github.com/0Nera/BMOSP/actions/workflows/pages/pages-build-deployment)

---

БМПОС - Базовая Модульная Платформа Операционных Систем для платформы x86_64 (BIOS/UEFI). Это отечественное программное обеспечение, созданное при поддержке Синапс ОС на языке программирования C.

БМПОС не является операционной системой. Это платформа для изучения.

Философия БМПОС - "всё есть модуль".

![Скриншот вывода ядра в эмуляторе Qemu](https://git.synapseos.ru/Aren/BMOSP/raw/branch/pages/assets/0_0.2.0.png)

## Список задач

Перенесено в файл `TODO.md`

## Партнеры

- Синапс ОС

## Помощь проекту

Мы рады предоставить вам полностью бесплатный доступ к **БМПОС** без каких-либо ограничений по распространению или монетизации. Если наш проект помог вам решить вашу проблему, мы будем благодарны за любую поддержку. Ниже вы найдете несколько вариантов для того, чтобы помочь нам.

### Доработка кодовой базы

Если вы владеете C/C++, ассемблером GAS, то вы можете доработать ядро проекта или написать свой пользовательский модуль.

Для добавления своего модуля используйте инструкцию `modules/README.md`.

### Указание авторства

Текущая лицензия позволяет вам использовать код ядра **БМПОС** в любых проектах.

Вы можете просто указать, что ваш проект использует технологии **БМПОС**. Это поможет распространить информацию о проекте и привлечь новых пользователей.

### Денежное пожертвование

Вы можете пожертвовать произвольную сумму на развитие проекта и при желании указать, какие из задач наиболее интересны для вас.

- Номер карты: 2200 7009 4662 4201
- Номер счета: 40817810400099892231
- БИК: 044525974
- Банк-получатель: АО «Т-Банк»

## Сборка и запуск

Перенесено в файл `docs/BUILD.md`

## Предупреждение

Использовать на свой страх и риск. Система расчитана на работу в эмулируемой среде Qemu под процессоры x86_64.
Мы не несем ответственности за нанесенный ущерб.

## Ресурсы

- <https://vk.com/BMOSP> Страница вконтакте
- <https://t.me/bmosp> Телеграм (активный форум)
- <https://bmosp.ru> Вебсайт (в процессе)
- <https://wiki.synapseos.ru/index.php?title=БМПОС> Страница на вики

### Зеркала

Для отправки своих изменений вы можете использовать следующие зеркала:

- <https://git.synapseos.ru/Aren/BMOSP> - доверенный сервер(главный репозиторий)
- <https://github.com/0Nera/BMOSP> - зеркало с CI
- <https://tvoygit.ru/0Nera/BMOSP> - зеркало
- <https://hub.mos.ru/synapseos/BMOSP> - неактивное зеркало

### Использованные ресурсы

- <https://github.com/limine-bootloader/limine> (BSD 2-Clause)
- <https://github.com/nothings/stb> (MIT, Общественное достояние)
- <https://en.wikipedia.org/wiki/CPUID>
- <https://github.com/klange/toaruos> (NCSA)
- <https://wiki.osdev.org/Model_Specific_Registers>
- <https://sandpile.org/x86/msr.htm>
