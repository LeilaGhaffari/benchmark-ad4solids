# benchmark-ad4solids

This is a benchmark test for various Automatic Differentiation (AD) tools for deriving constitutive models in solid mechanics.

The AD tools used in this benchmark suit are:

*   ADOL-C
*   Tapenade
*   Enzyme (C)

in addition to the hand-coded implementation of the *Neo-Hookean Hyper-elasticity* model in current configuration.

## Download and Install

### Enzyme-AD

Enzyme-AD can be [cloned from GitHub](https://github.com/EnzymeAD/Enzyme).

```console
$ git clone https://github.com/EnzymeAD/Enzyme
```

Follow the [Enzyme documentation](https://enzyme.mit.edu/Installation/) to build Enzyme.

### ADOL-C

ADOL-C can be [cloned from GitHub](https://github.com/coin-or/ADOL-C).

```console
$ git clone https://github.com/coin-or/ADOL-C
```

Follow the [ADOL-C instructions](https://github.com/coin-or/ADOL-C/blob/master/README) to build ADOL-C.

### Building

The environment variables `ENZYME_LIB`, `ADOLC_LIB`, and `ADOLC_INCLUDE` must be set to build the benchmark.
Assuming you have cloned `benchmark-ad4solids`, export these environment variables:

```console
$ export ENZYME_LIB=[path to Enzyme] ADOLC_LIB=[path to ADOL-C lib directory] ADOLC_LIB=[path to ADOL-C include directory]
```

Build with:
```console
$ make OPT="-Ofast -march=native"
```

## Run

Create a `csv` file that contains `n` number of data (quadrature) points with:

```console
$ python3 random-data.py n
```

and, run the tests with:
```console
$ ./build/elasticity-exec
```