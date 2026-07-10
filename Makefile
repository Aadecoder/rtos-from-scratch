CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m4 -mthumb -nostdlib -nostartfiles -O0 -g

SRCS = startup.c main.c rtos.c
OBJS = $(SRCS:.c=.o)

all: rtos.elf

rtos.elf: $(OBJS)
	$(CC) $(CFLAGS) -T linker.ld -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

qemu:
	qemu-system-arm -machine mps2-an385 -cpu cortex-m4 \
	  -kernel rtos.elf -nographic -serial mon:stdio \
	  -S -gdb tcp::1234

gdb:
	gdb-multiarch rtos.elf \
	  -ex "target remote :1234" \
	  -ex "load" \
	  -ex "break main"

clean:
	rm -f *.o *.elf

