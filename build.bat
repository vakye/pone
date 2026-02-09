
@echo off

if not exist build mkdir build

set CompileFlags=-target x86_64-unknown-windows -O0 -ffreestanding -mno-red-zone -nostdlib
set LinkFlags=-Wl,-entry:UEFIBoot,-subsystem:efi_application

pushd build
call clang %CompileFlags% "..\code\uefi.c" -o BOOTX64.efi %LinkFlags%
call ..\tools\write_gpt -ds 29 -i pone.img
popd
