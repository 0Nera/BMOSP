# Сборка из исходного кода

## Установка SDK

Перед сборкой рекомендуем установить готовый пакет SDK вместе с libc6

```bash
sudo apt update
sudo apt install unzip
./scripts/get_sdk.sh
```

## Ubuntu 18.04+

```bash
sudo apt install clang-format python3 git qemu-system-x86 qemu-system-gui gdisk dos2unix xorriso libc6 gcc make curl mtools
git clone https://git.synapseos.ru/Aren/BMOSP.git
cd BMOSP/
chmod +x *.sh
./build_mods.sh
./build.sh
```

## Astra Linux

На текущий момент доступна только сборка ISO образов
В qemu недоступен флаг `-cpu max`, просто уберите его при запуске

```bash
sudo apt install clang-format python3.7 git qemu-system-x86 gdisk dos2unix xorriso libc6 gcc make curl mtools
git clone https://git.synapseos.ru/Aren/BMOSP.git
cd BMOSP/
chmod +x *.sh
./build_mods.sh
./build.sh
```

## Запуск

## Qemu

Стандартная конфигурация загрузки с HDD под x86_64(AMD64) UEFI

```bash
qemu-system-x86_64 -name "БМПОС" -cpu max -m 128M -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd
```

Стандартная конфигурация загрузки с ISO образа под x86_64(AMD64) UEFI

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
