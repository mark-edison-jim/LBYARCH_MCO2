nasm -f win64 -o mco2ASM.obj mco2ASM.asm
gcc -m64 -c mco2_.c -o mco2_.obj
gcc -m64 -o program_.exe mco2_.obj mco2ASM.obj -lgcc
program_.exe