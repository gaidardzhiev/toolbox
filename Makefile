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

strip:
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag toolbox
