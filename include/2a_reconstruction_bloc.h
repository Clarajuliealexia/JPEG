#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../include/huffman.h"
#include "../include/jpeg_reader.h"
#include "../include/outils.h"

#ifndef PI
 #define PI 3.14
#endif

/*(a) Reconstruction de chaque bloc*/

/*2(a)i Extraction et décompression*/

/* magnitude: Obtention de la valeur de magnitude à
   partir des bits lus (position dans la classe de magnitude) */
int magnitude(int classe, uint32_t position);

/*2(a)ii Quantification inverse*/

void quantification_inverse(struct jpeg_desc *jdesc, int *bloc, int *iquant, int index);
void quantification_inverse_progressif(struct jpeg_desc *jdesc, int bloc, int *iquant, int index);

/*2(a)iii Réorganisation zig-zag*/

/* zigzag: reorganisation zigzag vecteur[64] en matrice[8][8] */
void zigzag(int **matrice, int *vecteur);

/*2(a)iv Calcul de la transformée en cosinus discrète inverse (iDCT)*/

/* Calcul du coefficient C : utile pour idct_naif */
float c(int x);

/* idct_naif : première version naïve de
 la transformée en cosinus discrète inverse :
 permet de passer d'un bloc fréquentiel à un bloc spatial */
void idct_naif(int **frequence, uint8_t **spatial);

/* Fonctions nécessaires pour l'algorithme de Loeffler */
void butterfly_unit(float *x0, float *x1);
void rotator_unit(float *x0, float *x1,float k, int n);

/* Implémentation de l'algorithme de Loeffler
    sur un vecteur[8] */
void algo_loeffler(float *vecteur);

/* Transformée en cosinus discrète inverse optimisée sur
   une matrice[8][8] à l'aide de l'algorithme de Loeffler */
void idct_rapide(int **frequence, uint8_t **spatial);
