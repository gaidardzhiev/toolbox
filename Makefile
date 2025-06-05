CC=musl-gcc
BIN=toolbox
all: $(BIN)

$(BIN): %: %.c
	$(CC) -o $@ $< -static

clean:
	rm $(BIN)

install:
	cp toolbox /usr/bin/toolbox
	cp toolbox.1 /usr/share/man/man1/
