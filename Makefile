CC=gcc
BINS=toolbox
all: $(BINS)

$(BINS): %: %.c
	$(CC) -o $@ $< -static

clean:
	rm $(BINS)
	[ ! -f prand ] || rm prand

install:
	cp toolbox /usr/bin/toolbox

.SILENT: prand
prand:
	echo prand.c - produces high-quality randomness using RDRAND CPU instruction that generates entropy directly from hardware using thermal noise and other physical phenomena
	grep -q "rdrand" /proc/cpuinfo && gcc -march=native -o prand prand.c || echo no RDRAND instruction...
