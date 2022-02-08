#!/bin/bash

source ~/.bashrc

module load intel-mkl/2020.4.304/gcc-7.3.0-52gb

export PKG_CONFIG_PATH=$HOME/gitrepos/hicma-x-dev/stars-h/build/installdir/lib/pkgconfig:$PKG_CONFIG_PATH

BLAS_INCLUDE="-DMKL_LP64 -I${MKLROOT}/include -DUSE_MKL"
BLAS_LIB="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl"


g++ -std=c++11 ${BLAS_INCLUDE} $(pkg-config --cflags starsh) ${BLAS_LIB} \
    $(pkg-config --libs starsh) interfaces.cpp
./a.out 100 10 3
