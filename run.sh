#!/bin/sh
qemu-system-x86_64 -name "БМПОС" -cpu max -m 1G -smp 1 -hda bmosp.hdd  --no-reboot
#qemu-system-x86_64 -name "БМПОС" -cpu max -m 1G -smp 1 -cdrom bmosp.iso -boot d  --no-reboot
#qemu-system-x86_64 -name "БМПОС" -cpu max -m 1G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd -d int --no-reboot
#qemu-system-x86_64 -name "БМПОС" -cpu host -m 1G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd 
#qemu-system-x86_64 -name "БМПОС" -enable-kvm -cpu host -m 6G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd -d int --no-reboot