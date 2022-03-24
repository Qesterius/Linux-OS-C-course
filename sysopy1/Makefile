TARGET=./target
GCC =gcc
FLAGS = -c -Wall
RM = rm -rf

all: build cleanaft test
build: biblioteka.o times.o main.o
	$(GCC) biblioteka.o times.o main.o -o main
build_dynamic: 
	$(GCC) -fPIC -c lib/biblioteka.c
	$(GCC) -shared -o libbiblioteka.so biblioteka.o
	$(GCC) main.c –lbiblioteka –L./ -o main



biblioteka.o:
	$(GCC) $(FLAGS) lib/biblioteka.c
main.o:
	$(GCC) $(FLAGS) main.c
times.o:
	$(GCC) $(FLAGS) times.c


test: main.exe  
	./main.exe raport2.txt create_table 10 wc_files files/medium.txt files/medium.txt save 2 wc_files files/medium.txt files/medium.txt save 2 remove_block 1 wc_files files/small.txt files/small.txt files/small.txt save 3 remove_block 1 
	cat raport2.txt
cleanaft:
	rm *.o



