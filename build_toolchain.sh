#!/bin/sh

fgcc() {
	BINUTILS="2.40"
	GCC="13.2.0"
	GLIBC="2.39"
	PREFIX="/usr/local/gcc-static"
	TARGET="x86_64-pc-linux-gnu"
	SRCDIR="/usr/src"
	GETNUMCPUS=`grep -c '^processor' /proc/cpuinfo`
	JOBS='-j '$GETNUMCPUS''
	printf "do you want to build %s binutils, gcc and glibc form source? (y/n)\n" "$TARGET"
	read Z
	case "$Z" in
		[y]*)
			printf "building %s binutils, gcc and glibc form.source...\n" "$TARGET"
			cd "$SRCDIR" || return 1
			wget -c https://ftp.gnu.org/gnu/binutils/binutils-"$BINUTILS".tar.gz || return 2
			tar xfv binutils-"$BINUTILS".tar.gz
			rm binutils-"$BINUTILS".tar.gz
			mkdir -p binutils-build
			cd binutils-build || return 3
			../binutils-"$BINUTILS"/configure \
				--prefix="$PREFIX" \
				--disable-nls \
				--without-docs \
				--disable-werror \
				--enable-static \
				--enable-shared=no \
				--target="$TARGET" || return 4
			make "$JOBS" && make install || return 5
			cd "$SRCDIR" || return 6
			wget -c https://ftp.gnu.org/gnu/gcc/gcc-"$GCC"/gcc-"$GCC".tar.gz || return 7
			tar xfv gcc-"$GCC".tar.gz
			rm gcc-"$GCC".tar.gz
			cd gcc-"$GCC" || return 8
			./contrib/download_prerequisites || return 9
			cd "$SRCDIR" || return 10
			mkdir -p gcc-build
			cd gcc-build || return 11
			../gcc-"$GCC"/configure \
				--prefix="$PREFIX" \
				--disable-multilib \
				--enable-languages=c \
				--disable-bootstrap \
				--disable-shared \
				--enable-static \
				--with-newlib \
				--without-docs \
				--disable-libssp \
				--disable-libquadmath \
				--disable-libgomp \
				--disable-libatomic \
				--disable-libstdcxx-pch || return 12
			make "$JOBS" && make install || return 13
			cd "$SRCDIR" || return 14
			wget -c https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.5.1.tar.xz || return 15
			tar xfv linux-6.5.1.tar.xz
			rm linux-6.5.1.tar.xz
			cd linux-6.5.1 || return 16
			make ARCH=$(echo "$TARGET" | cut -d'-' -f1) INSTALL_HDR_PATH="$PREFIX/$TARGET" headers_install || return 17
			cd "$SRCDIR" || return 18
			wget -c https://ftp.gnu.org/gnu/libc/glibc-"$GLIBC".tar.gz || return 19
			tar xfv glibc-"$GLIBC".tar.gz
			rm glibc-"$GLIBC".tar.gz
			mkdir -p glibc-build
			cd glibc-build || return 20
			CC="$PREFIX/bin/$TARGET-gcc" ../glibc-"$GLIBC"/configure \
				--prefix="$PREFIX" \
				--with-headers="$PREFIX/$TARGET/include" \
				--disable-shared \
				--enable-static \
				--without-docs \
				--disable-multilib \
				--disable-profile \
				--disable-werror || return 21
			make "$JOBS" || return 22
			make install || return 23
			cd "$SRCDIR" || return 24
			cd gcc-build || return 25
			make distclean
			../gcc-"$GCC"/configure \
				--prefix="$PREFIX" \
				--disable-multilib \
				--enable-languages=c \
					--enable-static \
				--disable-shared \
				--with-system-zlib || return 26
			make "$JOBS" || return 27
			make install || return 28
			"$PREFIX"/bin/gcc --version && return 0 || return 29
			;;
		[n]*)
			printf "you chose NOT to build binutils, gcc and glibc from source...\n"
			;;
		*)
			printf "invalid response...\n"
			printf "(yes/no)\n"
			;;
	esac
}

fmake() {
	MAKE="4.4"
	printf "do you want to build make form source? (y/n)\n"
	read X
	case "$X" in
		[y]*)
			printf "building make from source...\n"	
			cd /usr/src || return 30
			wget https://fosszone.csd.auth.gr/gnu/make/make-"$MAKE".tar.gz || return 31
			tar xf make-"$MAKE".tar.gz || return 32
			rm make-"$MAKE".tar.gz
			cd make-"$MAKE" || return 33
			./build.sh || return 34
			cp make /usr/bin || return 35
			make -v && return 0 || return 36
			;;
		[n]*)
			printf "you chose NOT to build make from source...\n"
			;;
		*)
			printf "invalid response...\n"
			printf "(yes/no)\n"
			;;
	esac
}

{ fgcc && printf "\n" && fmake; RET=$?; } || exit 1

[ "$RET" -eq 0 ] 2>/dev/null || printf "%s\n" "$RET"
