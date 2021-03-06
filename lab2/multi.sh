# 1. What does the following statement do?___________________________________
arm-none-eabi-as -mcpu=arm926ej-s -g ts.s -o ts.o

# 2. What does the following statement do?___________________________________
arm-none-eabi-gcc -c -mcpu=arm926ej-s -g t.c -o t.o

# 3. What does the following statement do?___________________________________
arm-none-eabi-ld -T t.ld ts.o t.o strlib -o t.elf

# 4. What does the following statement do?___________________________________
arm-none-eabi-objcopy -O binary t.elf t.bin

rm *.o *.elf

echo ready to go?
read dummy

qemu-system-arm -M realview-pbx-a9 -m 128M -kernel t.bin -serial mon:stdio \
-serial /dev/pts/4 -serial /dev/pts/3 -serial /dev/pts/5