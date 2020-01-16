#ifndef __OUTILS_H__
#define __OUTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

extern void dec_to_hex(int dec, char *hexadecimal);
extern int char_to_int(char hex[0]);
extern void print_vecteur64(int *vecteur);
extern void print_vecteur64uint(uint32_t *vecteur);
extern void init_vecteur64(int *vecteur);
extern void init_matrice88(int **matrice);
extern void init_matrice88_uint(uint8_t **matrice);
extern void print_matrice88(int **matrice);
extern void print_matrice88_uint(uint8_t **matrice);
extern void print_matrice88_float(float **matrice);
extern uint8_t saturation(float pixel);
extern uint32_t **allocation_matrice(int n, int m);
extern uint8_t **allocation_matrice_64();
extern void free_matrice_64(uint8_t **matrice);
extern void free_matrice(uint32_t **matrice, int n);
extern uint8_t *** my_malloc_ligne_of_ptr(uint32_t taille);
void my_free_ligne_of_ptr(uint8_t taille, uint8_t ***ligne_Y);
extern void transposee_matrice(float matrice[8][8]);

#endif
