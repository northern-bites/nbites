#!/bin/sh

set -o nounset

echo "Configuring and compiling the cross-compiler for the nao. Please enter your user name and password when promted, and be ready for a long wait"

TMP_DIR=/tmp/nao-cross-build
####
AL_DIR=/usr/local/nao
####
CROSS_BASE=$AL_DIR/crosstoolchain

echo "Please enter your ldap username: "
read USER

echo "Please enter your ldap password: "
read -s PASSWORD

PREFIX=$AL_DIR/crosstoolchain/cross/
TARGET=i586-linux
SYSROOT=$CROSS_BASE/staging/geode-linux

MAKE_OPTIONS=-j2

# update these when re-building the cross compiler
SOURCES_PATH=https://robocup.bowdoin.edu/files/software/nao/cross_compiler_stuff/gcc_source

SDK_FILE=linux-nao-SDK-1.10.tar.gz
CTC_FILE=nao-cross-toolchain-1.10.10.tar.bz2
BINUTILS_FILE=binutils-2.21.tar.bz2
GCC_FILE=gcc-4.5.2.tar.bz2

# stupid libraries that are needed by stupid GCC
GMP_FILE=gmp-4.3.2.tar.bz2
MPFR_FILE=mpfr-3.0.0.tar.bz2
MPC_FILE=mpc-0.9.tar.bz2

echo Using USER=$USER.

echo "Making temp directory at $TMP_DIR"
mkdir $TMP_DIR
cd $TMP_DIR

echo "Downloading Nao SDK"
test -e $SDK_FILE || curl -Oku $USER:$PASSWORD https://robocup.bowdoin.edu/files/software/nao/NaoQi/1.10/${SDK_FILE}
echo "Downloading Nao Cross-Compilation package"
test -e $CTC_FILE || curl -Oku $USER:$PASSWORD https://robocup.bowdoin.edu/files/software/nao/cross_compiler_stuff/${CTC_FILE}
echo "Downloading binutils source"
test -e $BINUTILS_FILE || curl -Oku $USER:$PASSWORD ${SOURCES_PATH}/${BINUTILS_FILE}
echo "Downloading GMP library source"
test -e $GMP_FILE || curl -Oku $USER:$PASSWORD ${SOURCES_PATH}/${GMP_FILE}
echo "Downloading mpfr library source"
test -e $MPFR_FILE || curl -Oku $USER:$PASSWORD ${SOURCES_PATH}/${MPFR_FILE}
echo "Downloading MPC library source"
test -e $MPC_FILE || curl -Oku $USER:$PASSWORD ${SOURCES_PATH}/${MPC_FILE}
echo "Downloading gcc source"
test -e $GCC_FILE || curl -Oku $USER:$PASSWORD ${SOURCES_PATH}/${GCC_FILE}

echo "Unpacking SDK"
if [ ! -e $AL_DIR ]; then
  mkdir -p $AL_DIR && \
    tar --strip 1 -C $AL_DIR -xzf $SDK_FILE || exit 1 # this might not be right
fi
echo "Unpacking Cross Compiler base"
if [ ! -e $CROSS_BASE ]; then
  mkdir -p $CROSS_BASE && \
    tar -C $CROSS_BASE -xjf $CTC_FILE || exit 1
fi

# if we change any library version(s) UPDATE THESE LINES
echo "Unpacking binutils"
rm -rf binutils-2.21 && tar -xjf $BINUTILS_FILE || exit 1
echo "Unpacking gcc"
rm -rf gcc-4.5.2 && tar -xjf $GCC_FILE || exit 1
echo "Unpacking GMP"
rm -rf gmp-4.3.2 && tar -xjf $GMP_FILE || exit 1
echo "Unpacking MPFR"
rm -rf mpfr-3.0.0 && tar -xjf $MPFR_FILE || exit 1
echo "Unpacking MPC"
rm -rf mpc-0.9 && tar -xjf $MPC_FILE || exit 1

echo "Building binutils"
pushd binutils-2.21 && ./configure --prefix=$PREFIX \
                                   --target=$TARGET \
                                   --with-sysroot=$SYSROOT \
                                   --disable-nls \
                                   --disable-werror &&
                       make $MAKE_OPTIONS && \
                       make install && \
                       popd || exit 1

echo "Building gmp"
mkdir build-gmp-4.3.2
pushd build-gmp-4.3.2 && ../gmp-4.3.2/configure --prefix=$PREFIX &&\
                         make $MAKE_OPTIONS && \
                         make install && \
                         popd || exit 1

echo "Building mpfr"
mkdir build-mpfr-3.0.0
pushd build-mpfr-3.0.0 && ../mpfr-3.0.0/configure --prefix=$PREFIX \
                                --with-gmp-include=$PREFIX/include \
                                --with-gmp-lib=$PREFIX/lib &&\
                         make $MAKE_OPTIONS && \
                         make install && \
                         popd || exit 1

echo "Building mpc"
mkdir build-mpc
pushd build-mpc && ../mpc-0.9/configure --prefix=$PREFIX \
                                --with-mpfr-include=$PREFIX/include \
                                --with-mpfr-lib=$PREFIX/lib \
                                --with-gmp-include=$PREFIX/include \
                                --with-gmp-lib=$PREFIX/lib/ && \
                         make $MAKE_OPTIONS && \
                         make install && \
                         popd || exit 1

echo "Building gcc"
mkdir build-gcc
pushd build-gcc && ../gcc-4.5.2/configure --prefix=$PREFIX \
                               --target=$TARGET \
                               --with-gmp-include=$PREFIX/include \
                               --with-gmp-lib=$PREFIX/lib \
                               --with-mpfr-include=$PREFIX/include \
                               --with-mpfr-lib=$PREFIX/lib \
                               --with-mpc-include=$PREFIX/include \
                               --with-mpc-lib=$PREFIX/lib \
                               --with-gnu-as \
                               --with-gnu-ld \
                               --with-as=$PREFIX/bin/$TARGET-as \
                               --with-ld=$PREFIX/bin/$TARGET-ld \
                               --with-sysroot=$SYSROOT \
                               --enable-threads=posix \
                               --enable-languages=c,c++ \
                               --disable-nls \
                               --disable-libmudflap \
                               --disable-libssp \
                               --disable-libgomp &&\
                   make $MAKE_OPTIONS && \
                   make install && \
                   popd || exit 1

echo "Setting up final symlinks"
ln -s $PREFIX/. $PREFIX/geode || exit 1

echo "Done!"
