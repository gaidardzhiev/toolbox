#!/bin/sh

export PREFIX="/usr/local/musl"
export BIN="$PREFIX"
export LIB="$PREFIX"
export GETNUMCPUS=`grep -c '^processor' /proc/cpuinfo`
export JOBS='-j '$GETNUMCPUS''
export MUSL="musl-1.2.5"
export DIR="/usr/src/"

[ -d "$PREFIX" ] && { printf "$PREFIX exists...\n"; exit 0; }

[ ! -d "$DIR" ] && mkdir -p "$DIR"

fget() {
	if cd $DIR; then
		wget https://musl.libc.org/releases/$MUSL.tar.gz
		tar xf $MUSL.tar.gz
		rm $MUSL.tar.gz
		cd $MUSL
		return 0
	else
		return 8
	fi
}

fbuild() {
	if ./configure \
		--prefix=$PREFIX \
		--exec-prefix=$BIN \
		--syslibdir=$LIB \
		--disable-shared; then
		make
		make install
		cp $BIN/bin/musl-gcc /usr/bin/
		return 0
	else
		return 16
	fi
}

ftry() {
	if cd $TMP; then
		printf '#include <stdio.h>\nint main(int argc,char**argv){printf("hello %%d\\n",argc);}\n' > hello.c
		musl-gcc -static -Os hello.c
		./a.out
		file a.out
		size a.out
		ldd a.out
		objdump -p a.out | grep NEEDED
		rm a.out
		return 0
	else
		return 32
	fi
}

{ fget && fbuild && ftry; RET=$?; } || {
	printf "something's wrong in here somewhere...\n";
	exit 1;
}

[ "$RET" -eq 0 ] 2>/dev/null || printf "%s\n" "$RET"
