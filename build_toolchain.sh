#!/bin/sh

if
	tcc --version;
then
        printf "\n"
else
	git clone https://github.com/TinyCC/tinycc
	cd tinycc
	./configure
	make
	make test
	make install
fi

if
	make --version;
then
	printf "\n"
else
	wget https://fosszone.csd.auth.gr/gnu/make/make-4.4.tar.gz
	tar xf make-4.4.tar.gz
	rm make-4.4.tar.gz
	cd make-4.4
	./build.sh
	cp make /usr/bin
	make --version
fi