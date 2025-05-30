#!/bin/sh

ftcc() {
	tcc -v && {
        	printf "tcc is installed...\n";
		return 0;
	} || {
		cd /usr/src;
		git clone https://github.com/TinyCC/tinycc;
		cd tinycc;
		./configure \
			--enable-cross;
		make;
		make test;
		make install;
		tcc -v && return 0 || return 16
	}
}

fmake() {
	VER="4.4"
	make -v	&& {
		printf "make is installed...\n";
		return 0;
	} || {
		cd /usr/src;
		wget https://fosszone.csd.auth.gr/gnu/make/make-$VER.tar.gz;
		tar xf make-$VER.tar.gz;
		rm make-$VER.tar.gz;
		cd make-$VER;
		./build.sh;
		cp make /usr/bin;
		make -v && return 0 || return 8
	}
}

{ fmake && printf "\n" && ftcc; RET=$?; } || exit 1

[ "$RET" -eq 0 ] 2>/dev/null || printf "%s\n" "$RET"
