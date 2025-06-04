CC=musl-gcc
BINS=toolbox
all: $(BINS)

$(BINS): %: %.c
	$(CC) -o $@ $< -static

clean:
	rm $(BINS)

install:
	cp toolbox /usr/bin/toolbox
	cp toolbox.1 /usr/share/man/man1/
