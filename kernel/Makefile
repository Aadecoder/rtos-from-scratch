build:
	arm-none-eabi-gcc -c -mcpu=cortex-m3 -mthumb -g -O0 startup.c -o startup.o
	arm-none-eabi-gcc -c -mcpu=cortex-m3 -mthumb -g -O0 scheduler.c -o scheduler.o
	arm-none-eabi-ld -T linker.ld startup.o scheduler.o -o kernel.elf

run:
	qemu-system-arm -M lm3s6965evb -cpu cortex-m3 -nographic -kernel kernel.elf -S -s

debug:
	arm-none-eabi-gdb kernel.elf
