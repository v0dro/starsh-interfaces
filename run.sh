#!/bin/bash


source ~/.bashrc

module load intel-mkl/2020.4.304/gcc-7.3.0-52gb

# export PKG_CONFIG_PATH=$HOME/gitrepos/hicma-x-dev/stars-h/build/installdir/lib/pkgconfig:$PKG_CONFIG_PATH

BLAS_INCLUDE="-DMKL_LP64 -I${MKLROOT}/include -DUSE_MKL"
BLAS_LIB="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl"

set -o xtrace

# rm a.out
rm *csv
g++ -std=c++11 ${BLAS_INCLUDE} $(pkg-config --cflags starsh) -c interfaces.cpp
g++ ${BLAS_LIB} interfaces.o $(pkg-config --libs starsh)  -o a.out
./a.out 1000 1 sin_kernel_2d
./a.out 1000 2 sqrexp_2d
./a.out 1000 3 sqrexp_3d
