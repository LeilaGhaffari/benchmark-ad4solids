#!/usr/bin/python3
if __name__ == '__main__':
  import sys
  import os
  sys.path.insert(0, os.path.abspath('config'))
  import configure
  configure_options = [
    '--with-strict-petscerrorcode',
    '--download-ctetgen',
    '--download-triangle',
    '--download-mumps',
    '--download-parmetis',
    '--download-metis',
    '--download-scalapack',
    '--download-superlu_dist',
    '--with-fortran-bindings=0',
    '--with-debugging=0',
    '--with-hip=0',
    '--with-cuda=0',
    '--with-hipc=0',
    '--with-cudac=0',
    '--download-openmpi',
    '-download-openmpi-configure-arguments="--disable-io-romio"',
    '--with-cc=clang-15',
    '--with-cxx=clang++-15',
    'COPTFLAGS=-O3 -march=native',
    'CXXOPTFLAGS=-O3 -march=native',
    'PETSC_ARCH=arch-clang-O',
  ]
  configure.petsc_configure(configure_options)

# build ratel with:
export ENZYME_LIB=/projects/ratel/Enzyme/enzyme/build/Enzyme/ClangEnzyme-15.so
export PETSC_DIR=/home/leila/petsc PETSC_ARCH=arch-clang-O CEED_DIR=/home/leila/libCEED
make -B WITH_ADOLC=1 OMPI_CXX=clang++ CXX=mpicxx -j OPT="-Ofast -march=native"

# run with:
mpirun.openmpi -np 10 build/ex01-static -options_file examples/ymls/ex01-static-elasticity-neo-hookean-initial-ad-adolc.yml
