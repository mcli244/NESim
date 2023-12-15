#!/bin/bash
###
 # @Author: your name
 # @Date: 2021-09-29 11:44:09
 # @LastEditors: Please set LastEditors
 # @Description: In User Settings Edit
 # @FilePath: make.sh
### 


if [ $# -eq 0 ]; then
    echo "Usage: ./make.sh [arch] arch:arm or x86 or aarch64"
    exit 1
fi



if [ $1 == "arm" ]; then
    echo "arch:arm"
    echo "arch:arm"
    # export CXX=arm-linux-gnueabi-g++
    # export CC=arm-linux-gnueabi-gcc
    # export LD=arm-linux-gnueabi-ld
    # export OBJCOPY=arm-linux-gnueabi-objcopy
    # export OBJDUMP=arm-linux-gnueabi-objdump
    # export AR=arm-linux-gnueabi-ar
    # export STRIP=arm-linux-gnueabi-strip
    # export RANLIB=arm-linux-gnueabi-ranlib

    source /home/share/otafifth/tmp/build.sh
    
    export build_dir=build_$1

elif [ $1 == "x86" ]; then
    echo "arch:x86"
    export CC=gcc
    export LD=ld
    export AR=ar
    export STRIP=strip
    export OBJCOPY=objcopy
    export OBJDUMP=objdump
    export RANLIB=ranlib
    export CXX=g++

    export build_dir=build_$1
elif [ $1 == "aarch64" ]; then
    echo "arch:aarch64"
    export STAGING_DIR=/home/share/exDisk_1/pack/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu
    export PATH=$PATH:$STAGING_DIR/bin
    export SYSROOT=$STAGING_DIR/aarch64-linux-gnu
    
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$STAGING_DIR/lib/x86_64-linux-gnu:$SYSROOT/lib
    echo "LD_LIBRARY_PATH:$LD_LIBRARY_PATH"
    export CC="aarch64-linux-gnu-gcc --sysroot=$SYSROOT"
    export LD="aarch64-linux-gnu-ld --sysroot=$SYSROOT"
    export AR="aarch64-linux-gnu-ar"
    export STRIP="aarch64-linux-gnu-strip"
    export OBJCOPY="aarch64-linux-gnu-objcopy"
    export OBJDUMP="aarch64-linux-gnu-objdump"
    export RANLIB="aarch64-linux-gnu-ranlib"
    export CXX="aarch64-linux-gnu-g++ --sysroot=$SYSROOT"

    export build_dir=build_$1
else
    echo "Usage: ./make.sh [arch] arch:arm or x86"
    exit 1
fi 

if [ $# -eq 2 ]; then
    if [ $2 == "cp" ]; then
        echo "cp "${build_dir}"/deploy/bin/otafifth_demo /home/share/tftp_dir/"
        cp ${build_dir}/deploy/bin/otafifth_demo  /home/share/tftp_dir/
        exit
    fi
fi

echo CC     =$CC       
echo LD     =$LD       
echo AR     =$AR       
echo STRIP  =$STRIP    
echo OBJCOPY=$OBJCOPY  
echo OBJDUMP=$OBJDUMP  
echo RANLIB =$RANLIB   
echo CXX    =$CXX

# $PATH
$CC -v
if [ -d ${build_dir} ]; then
    echo ${build_dir}" dir exist"
else
    mkdir ${build_dir}
fi
cd ${build_dir}
cmake ..
make
# make install