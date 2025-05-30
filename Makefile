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
	cp toolbox.1 /usr/share/man/man1/
