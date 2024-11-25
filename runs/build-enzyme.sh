export MPICH_CC=clang-15 \
PETSC_DIR=/projects/jed/petsc \
PETSC_ARCH=mpich-cpu-O \
ENZYME_LIB=/home/leila/Enzyme/enzyme/build15/Enzyme/ClangEnzyme-15.so \
CEED_DIR=/home/leila/libCEED/ # doesn't matter, main is good

make OPT='-Ofast -march=native -fopenmp' -B -j # with ratel branch leila/enzyme-clang-15 (9c24df11e1d5421c110c63a83d9f46128515b8e5)
