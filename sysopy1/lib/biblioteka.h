#pragma once
#include <stdio.h>
#include <malloc.h>

extern FILE* wc(char path[]); //printing out to tmpfile..... przeprowadzenie zliczenia lini, słów i znaków dla zadanych plików i zapisanie wyniku zliczania w pliku tymczasowym
extern void wcsystem(char* path, int num); // wc but using unix wc
extern void cat_path(char path[]);
extern void cat_file(FILE* file);

extern int* allocate3Ints(int* a, int* b, int* c);// utworzenie tablicy wskaźników w której będą przechowywane wskaźniki na bloki pamięci zawierające wyniki 
extern int save(int** table, int n, int index);
/*

zarezerwowanie bloku pamięci o rozmiarze odpowiadającym rozmiarowi pliku tymczasowego i zapisanie w tej pamięci jego zawartości,
  ustawienie w tablicy wskaźników wskazania na ten blok, funkcja powinna
  zwrócić indeks stworzonego bloku w tablicy,
usunięcie z pamięci bloku o zadanym indeksie
*/