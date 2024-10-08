#!/bin/sh
qemu-system-x86_64 -name "БМПОС" -cpu max -m 128M -smp 1         \
 -serial file:serial.log                                        \
 -drive file=bmosp.iso,if=none,id=sata_drive -device ahci       \
 -device virtio-blk-pci,drive=sata_drive                        \
 -rtc base=localtime,clock=host                                 \
 --no-reboot -no-shutdown -boot d                               \
 -net nic,model=pcnet # AMD PCnet Am79C970
#qemu-system-x86_64 -name "БМПОС" -cpu max -m 1G -smp 1 -hda bmosp.hdd  --no-reboot
#qemu-system-x86_64 -name "БМПОС" -cpu max -m 1G -smp 1 -cdrom bmosp.iso -boot d  --no-reboot
#qemu-system-x86_64 -name "БМПОС" -cpu max -m 1G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd -d int --no-reboot
#qemu-system-x86_64 -name "БМПОС" -cpu host -m 1G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd 
#qemu-system-x86_64 -name "БМПОС" -enable-kvm -cpu host -m 6G -smp 1 -bios ovmf/OVMF.fd -hda bmosp.hdd -d int --no-reboot