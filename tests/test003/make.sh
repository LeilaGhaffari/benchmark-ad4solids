rm *.o
rm exec.out

gfortran -c tapenade.f90 -o tapenade-f.o
g++ -c tapenade.cpp -o tapenade.o
g++ -c main.cpp -o main.o
g++ tapenade-f.o tapenade.o main.o -lgfortran -o exec.out


