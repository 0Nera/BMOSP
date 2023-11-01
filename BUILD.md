# Сборка и запуск

Для сборки нужен gcc собранный под архитектуру x86_64. Clang не рекомендуется для использования.

## Сборка из исходного кода

Понадобится Python версии 3.7 и новее. Для сборки на Windows используйте WSL или виртуальную машину.

### Ubuntu 18.04+

```bash
sudo apt install clang-format python3 git qemu-system-x86 gdisk dos2unix xorriso
git clone https://git.synapseos.ru/Aren/BMOSP.git
cd BMOSP/
chmod +x build.sh
./build.sh
```

### Astra Linux

На текущий момент доступна только сборка ISO образов
В qemu недоступен флаг `-cpu max`, просто уберите его при запуске

```bash
sudo apt install clang-format python3.7 git qemu-system-x86 gdisk dos2unix xorriso
git clone https://git.synapseos.ru/Aren/BMOSP.git
cd BMOSP/
python3.7 pbuild.py
```

### ArchLinux

Модули грузятся с ошибкой

```bash
yay -S clang-format
sudo pacman -S python3 git qemu-system-x86 xorriso
git clone https://git.synapseos.ru/Aren/BMOSP.git
cd BMOSP/
chmod +x build.sh
./build.sh
```

## Запуск

Помимо использования VirtualBox, Qemu, VMware и других эмуляторов допускается запись hdd (raw) образа на флэшку в режиме dd.

### Qemu

Стандартная конфигурация загрузки с HDD

```bash
qemu-system-x86_64 -name "БМПОС" -cpu max -m 128M -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd
```

Стандартная конфигурация загрузки с ISO образа

```bash
qemu-system-x86_64 -name "БМПОС" -cpu max -m 128M -smp 1 -cdrom bmosp.iso -boot d --no-reboot
```

Или

```bash
chmod +x run.sh
./run.sh
```

## Предупреждение

Использовать на свой страх и риск. Система расчитана на работу в эмулируемой среде Qemu под процессоры x86_64.
Мы не несем ответственности за нанесенный ущерб.
