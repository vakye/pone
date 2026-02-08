
@echo off

if not exist build mkdir build

set CompileFlags=%CompileFlags% /nologo /Zi /FC /std:c11 /GS- /Gs999999 /Od /Oi /W4 /WX /wd4101 /wd4189 /wd4100
set LinkFlags=/incremental:no /opt:icf /opt:ref /nodefaultlib /subsystem:efi_application /entry:EntryUEFI

pushd build
call cl %CompileFlags% /Fe:BOOTX64.efi "..\code\uefi_pone.c" /link %LinkFlags% %UEFILinkFlags%
call ..\tools\write_gpt -i pone.img
popd
