#ifndef __OUTILS_H__
#define __OUTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

extern void dec_to_hex(int dec, char *hexadecimal);
extern int char_to_int(char hex[0]);
extern void print_vecteur64(int vecteur[64]);
extern void print_vecteur64uint(uint32_t vecteur[64]);
extern void init_vecteur64(int vecteur[64]);
extern void init_matrice88(int matrice[8][8]);
extern void print_matrice88(int matrice[8][8]);
extern uint8_t saturation(float pixel);
extern uint32_t **allocation_matrice(int n, int m);
extern uint32_t ***allocation_matrice3d(int n, int m, int k);
#endif
