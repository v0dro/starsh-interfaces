#!/bin/bash


source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

export PKG_CONFIG_PATH=/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib:$LD_LIBRARY_PATH

BLAS_INCLUDE="-DMKL_LP64 -I${MKLROOT}/include -DUSE_MKL"
BLAS_LIB="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl"

set -o xtrace

# rm a.out
rm *csv
g++ -std=c++11 -O3 ${BLAS_INCLUDE} $(pkg-config --cflags starsh) -c interfaces.cpp
g++ -o a.out interfaces.o ${BLAS_LIB} /home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib/libstarsh.a
# ./a.out 1000 1 sin_kernel_2d
gdb -ex run --args ./a.out 57126 5 laplace_file 2048
# ./a.out 8192 5 laplace_3d 2048
# ./a.out 1000 3 sqrexp_3d
# ./a.out 1000 4 exp_3d
