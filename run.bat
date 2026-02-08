
@echo off

call qemu-system-x86_64 -drive format=raw,unit=0,file=build/pone.img -bios bios/OVMF.fd -m 512M -vga std -name Pone -machine q35 -net none
