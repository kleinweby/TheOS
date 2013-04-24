#!/bin/bash

PLATFORM="x86"

# Find base dir
pushd `dirname $0` > /dev/null
BASEDIR=`pwd`
popd > /dev/null

TOOLCHAIN_PATCHES=$BASEDIR/patches
TOOLCHAIN_DIR=$BASEDIR/$PLATFORM.toolchain
TEMP_DIR=
trap on_exit EXIT

LLVM_REVERSION=180190
CLANG_REVERSION=180187
COMPILER_RT_REVERSION=180184
CURL_OPTIONS="-L -i -f -#"
TOOLCHAIN_VERSION=$(git rev-list -1 HEAD -- "$BASEDIR")
TOOLCHAIN_URL=
TOOLCHAIN_PRECOMPILED=0
BINUTILS_VERSION=2.23.2
MAKE_JOBS=1
HOST=

function on_exit {
	rm -rf "$TEMP_DIR"
	log "On exit"
}

function log {
	echo -e "=> \033[00;32m$@\033[00m"
}

function detect_host {
	HOST=$(echo "$(uname)_$(uname -m)" | tr '[A-Z]' '[a-z]')

	if [ -e /proc/cpuinfo ]; then 
		MAKE_JOBS=$(grep -c ^processor /proc/cpuinfo)
		TEMP_DIR=$(mktemp -d --tmpdir theos-toolchain-XXXXXX)
	elif [[ "$HOST" =~ "darwin_*" ]]; then
		MAKE_JOBS=$(sysctl -n hw.ncpu)
		TEMP_DIR=$(mktemp -d t theos-toolchain)
	fi

	TOOLCHAIN_URL="https://chrspeich-theos.s3.amazonaws.com/toolchain/$HOST/$PLATFORM-$TOOLCHAIN_VERSION.tar.xz"
}

function download_precompiled_toolchain {
	log "Checking for precompiled toolchain..."
	curl $CURL_OPTIONS -o "$TEMP_DIR/toolchain.tar.xz" "$TOOLCHAIN_URL"
	if [[ $? -eq 0 ]]; then
		TOOLCHAIN_PRECOMPILED=1
	else
		TOOLCHAIN_PRECOMPILED=0
	fi
}

function download_binutils {
	log "Download binutils"
	curl -o "$TEMP_DIR/binutils.tar.gz" "http://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz" || exit 1
	log "Unpack binutils"
	mkdir "$TEMP_DIR/binutils"
	tar xf "$TEMP_DIR/binutils.tar.gz" -C "$TEMP_DIR/binutils" --strip=1 || exit 1
	rm "$TEMP_DIR/binutils.tar.gz"
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

function upload_toolchain {
	log "Try to upload the toolchain"

	pushd "$TOOLCHAIN_DIR"
	tar c . | xz > "$TEMP_DIR/$PLATFORM-$TOOLCHAIN_VERSION.tar.xz"
	popd

	travis-artifacts upload --target-path "toolchain/$HOST/" --path "$TEMP_DIR/$PLATFORM-$TOOLCHAIN_VERSION.tar.xz"
}

log "Prepare toolchain for $PLATFORM"

rm -rf "$TOOLCHAIN_DIR"

detect_host

echo "$TEMP_DIR"
exit 1

download_precompiled_toolchain

if [[ $TOOLCHAIN_PRECOMPILED -eq 1 ]]; then
	log "Found a precompiled version..."
else
	log "No precompiled version avaiable. Compile the toolchain..."
	download_binutils
	download_llvm
	download_clang

	patch_llvm_clang

	compile_binutils
	compile_llvm_clang

	upload_toolchain
fi
