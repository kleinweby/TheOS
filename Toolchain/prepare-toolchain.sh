#!/bin/bash

PLATFORM="x86"

# Find base dir
pushd `dirname $0` > /dev/null
BASEDIR=`pwd`
popd > /dev/null

TOOLCHAIN_PATCHES=$BASEDIR/patches
TOOLCHAIN_DIR=$BASEDIR/$PLATFORM
TEMP_DIR="test-build" #$(mktemp -d -t theos-toolchain)
trap on_exit EXIT

LLVM_REVERSION=180190
CLANG_REVERSION=180187
COMPILER_RT_REVERSION=180184
TOOLCHAIN_VERSION=$(git rev-list -1 HEAD -- "$BASEDIR")
BINUTILS_VERSION=2.23.2
MAKE_JOBS=1
HOST=

function on_exit {
	#rm -rf "$TEMP_DIR"
	log "On exit"
}

function log {
	echo -e "=> \033[00;32m$@\033[00m"
}

function detect_host {
	HOST=$(echo "$(uname)_$(uname -m)" | tr '[A-Z]' '[a-z]')

	if [ -e /proc/cpuinfo ]; then 
		MAKE_JOBS=$(grep -c ^processor /proc/cpuinfo)
	elif [ "$HOST" = "darwin" ]; then
		MAKE_JOBS=$(sysctl -n hw.ncpu)
	fi
}

function download_binutils {
	log "Download binutils"
	#curl -o "$TEMP_DIR/binutils.tar.gz" "http://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz" || exit 1
	log "Unpack binutils"
	mkdir "$TEMP_DIR/binutils"
	tar xf "$TEMP_DIR/binutils.tar.gz" -C "$TEMP_DIR/binutils" --strip=1 || exit 1
	#rm "$TEMP_DIR/binutils.tar.gz"
}

function download_llvm {
	pushd "$TEMP_DIR"
	svn co "http://llvm.org/svn/llvm-project/llvm/trunk"@"$LLVM_REVERSION" llvm || exit 1
	popd
}

function download_clang {
	pushd "$TEMP_DIR/llvm"
	pushd "tools"
	svn co "http://llvm.org/svn/llvm-project/cfe/trunk"@"$CLANG_REVERSION" clang || exit 1
	popd
	#pushd "projects"
	#svn co -r COMPILER_RT_REVERSION http://llvm.org/svn/llvm-project/compiler-rt/trunk compiler-rt || exit 1
	#popd
	popd
}

function patch_llvm_clang {
	log "Patch llvm and clang"
	pushd "$TEMP_DIR/llvm"
	patch -p0 < "$TOOLCHAIN_PATCHES/llvm.patch" || exit 1
	pushd "tools/clang"
	patch -p0 < "$TOOLCHAIN_PATCHES/clang.patch" || exit 1
	popd
	popd
}

function compile_binutils {
	pushd "$TEMP_DIR/binutils"
	# We don't want to prefix the comipler, as it will be contained in a platform dir
	./configure --prefix="$TOOLCHAIN_DIR" --target=i386-unkown-elf --with-build-sysroot="$TOOLCHAIN_DIR" --program-prefix= --without-doc || exit 1
 	make -j$MAKE_JOBS || exit 1
 	make install || exit 1
}

function compile_llvm_clang {
	log "Compile LLVM/CLANG"
	mkdir "$TEMP_DIR/llvm-build"
	pushd "$TEMP_DIR/llvm-build"
	../llvm/configure --target=i386-unkown-theos --prefix="$TOOLCHAIN_DIR" --with-build-sysroot="$TOOLCHAIN_DIR" --program-prefix= --disable-docs --enable-shared --disable-static --enable-optimized --disable-assertions --disable-debug-runtime --disable-expensive-checks --enable-bindings=none --enable-targets=x86 || exit 1
 	make -j$MAKE_JOBS || exit 1
	make install || exit 1
	popd
}

log "Prepare toolchain for $PLATFORM"

#rm -rf "$TOOLCHAIN_DIR"

detect_host

#download_binutils
#download_llvm
#download_clang

#patch_llvm_clang

#compile_binutils
#compile_llvm_clang

