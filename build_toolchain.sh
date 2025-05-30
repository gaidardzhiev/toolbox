#!/bin/sh

fgcc() {
	BINUTILS_VER="2.40"
	GCC_VER="13.2.0"
	GLIBC_VER="2.39"
	PREFIX="/usr/local/gcc-static"
	TARGET="x86_64-pc-linux-gnu"
	SRCDIR="/usr/src"
	MAKEFLAGS="-j$(nproc)"
	printf "do you want to build binutils, gcc and glibc form source? (y/n)\n"
	read Z
	case "$Z" in
		y|Y)
			printf "building binutils, gcc and glibc form.source...\n"
			cd "$SRCDIR" || return 1
			wget -c https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VER.tar.gz || return 2
			tar xf binutils-$BINUTILS_VER.tar.gz
			rm binutils-$BINUTILS_VER.tar.gz
			mkdir -p binutils-build
			cd binutils-build || return 3
			../binutils-$BINUTILS_VER/configure \
				--prefix="$PREFIX" \
				--disable-nls \
				--disable-werror \
				--enable-static \
				--enable-shared=no \
				--target="$TARGET" || return 4
			make $MAKEFLAGS all install || return 5
			cd "$SRCDIR" || return 6
			wget -c https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VER/gcc-$GCC_VER.tar.gz || return 7
			tar xf gcc-$GCC_VER.tar.gz
			rm gcc-$GCC_VER.tar.gz
			cd gcc-$GCC_VER || return 8
			./contrib/download_prerequisites || return 9
			cd "$SRCDIR" || return 10
			mkdir -p gcc-build
			cd gcc-build || return 11
			../gcc-$GCC_VER/configure \
				--prefix="$PREFIX" \
				--disable-multilib \
				--enable-languages=c \
				--disable-bootstrap \
				--disable-shared \
				--enable-static \
				--with-newlib \
				--disable-libssp \
				--disable-libquadmath \
				--disable-libgomp \
				--disable-libatomic \
				--disable-libstdcxx-pch || return 12
			make $MAKEFLAGS all-gcc all-target-libgcc install-gcc install-target-libgcc || return 13
			cd "$SRCDIR" || return 14
			wget -c https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.5.1.tar.xz || return 15
			tar xf linux-6.5.1.tar.xz
			rm linux-6.5.1.tar.xz
			cd linux-6.5.1 || return 16
			make ARCH=$(echo $TARGET | cut -d'-' -f1) INSTALL_HDR_PATH="$PREFIX/$TARGET" headers_install || return 17
			cd "$SRCDIR" || return 18
			wget -c https://ftp.gnu.org/gnu/libc/glibc-$GLIBC_VER.tar.gz || return 19
			tar xf glibc-$GLIBC_VER.tar.gz
			rm glibc-$GLIBC_VER.tar.gz
			mkdir -p glibc-build
			cd glibc-build || return 20
			CC="$PREFIX/bin/$TARGET-gcc" ../glibc-$GLIBC_VER/configure \
				--prefix="$PREFIX" \
				--with-headers="$PREFIX/$TARGET/include" \
				--disable-shared \
				--enable-static \
				--disable-multilib \
				--disable-profile \
				--disable-werror || return 21
			make $MAKEFLAGS || return 22
			make install || return 23
			cd "$SRCDIR" || return 24
			cd gcc-build || return 25
			make distclean
			../gcc-$GCC_VER/configure \
				--prefix="$PREFIX" \
				--disable-multilib \
				--enable-languages=c \
					--enable-static \
				--disable-shared \
				--with-system-zlib || return 26
			make $MAKEFLAGS || return 27
			make install || return 28
			$PREFIX/bin/gcc --version && return 0 || return 29
			;;
		n|N)
			printf "you chose NOT to build binutils, gcc and glibc from source...\n"
			;;
		*)
			printf "invalid response...\n"
			;;
	esac
}

fmake() {
	MK_VER="4.4"
	printf "do you want to build make form source? (y/n)\n"
	read X
	case "$X" in
		y|Y)
			printf "building make from source...\n"	
			cd /usr/src || return 30
			wget https://fosszone.csd.auth.gr/gnu/make/make-$MK_VER.tar.gz || return 31
			tar xf make-$MK_VER.tar.gz || return 32
			rm make-$MK_VER.tar.gz
			cd make-$MK_VER || return 33
			./build.sh || return 34
			cp make /usr/bin || return 35
			make -v && return 0 || return 36
			;;
		n|N)
			printf "you chose NOT to build make from source...\n"
			;;
		*)
			printf "invalid response...\n"
			;;
	esac

}

{ fgcc && printf "\n" && fmake; RET=$?; } || exit 1

[ "$RET" -eq 0 ] 2>/dev/null || printf "%s\n" "$RET"
