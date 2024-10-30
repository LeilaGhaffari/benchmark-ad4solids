rm *.o
rm exec.out

gfortran -c helper.f90 -o helper-f.o
g++ -c helper.cpp -o helper.o
g++ -c main.cpp -o main.o
g++ helper-f.o helper.o main.o -lgfortran -o exec.out

./exec.out
