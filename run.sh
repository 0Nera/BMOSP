#!/bin/sh
#qemu-system-x86_64 -cpu max -m 1G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd -d int --no-reboot
#qemu-system-x86_64 -cpu host -m 1G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd 
qemu-system-x86_64 -enable-kvm -cpu host -m 6G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd -d int --no-reboot