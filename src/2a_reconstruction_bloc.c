#include "../include/2a_reconstruction_bloc.h"

/*(a) Reconstruction de chaque bloc*/

      /* 2(a)i Extraction et décompression */

/* int magnitude: Obtention de la valeur de magnitude à
  partir des bits lus (position dans la classe de magnitude) */
int magnitude(int classe, uint32_t position){
  if (position <= (1 << classe)-1 - (1 << (classe-1))){
    return -(1 << classe) + 1 + position;
  }
  else {
    position = position - ((1 << classe)-1 - (1 << (classe - 1)));
    return (1 << (classe-1)) + position -1;
  }
}

/*2(a)ii Quantification inverse*/

/*Quantification inverse: se fait en place */
void quantification_inverse(struct jpeg_desc *jdesc, int *bloc, int *iquant, int index) {
  uint32_t *table;
  table = get_quantization_table (jdesc, index);
  for (int i = 0; i < 64; i++){
    iquant[i] = table[i]*bloc[i];
  }
}

/*2(a)iii Réorganisation zig-zag*/
void zigzag(int **matrice, int *vecteur){
  int compteur = 0;
  for(int8_t i = 0; i < 16; i++){
    int8_t j = 0;
    if(i < 8){
      j = 0;
    }
    else{
      j = i - 8 + 1;
    }
    while(j <= i && j < 8){
      if (i % 2 == 1){
        matrice[i-j][j] = vecteur[compteur];
        compteur +=1;
      }
      else{
        matrice[j][i-j] = vecteur[compteur];
        compteur +=1;
      }
      j += 1;
    }
  }
}

/*2(a)iv Calcul de la transformée en cosinus discrète inverse (iDCT)*/

float c(int x){
  if (x == 0){
    return 1/sqrt(2);
  }
  else {
    return 1;
  }
}

/* IDCT : première version naïve */
void idct_naif(int **frequence, uint8_t **spatial){
  for (int x = 0; x < 8; x++){
    for (int y = 0; y < 8; y++) {
      float sommel = 0;
      for (int lambda = 0; lambda < 8; lambda++){
        float sommem = 0;
        for (int mu = 0; mu < 8; mu++){
          sommem += c(lambda)*c(mu)*cos((2*x+1)*PI*lambda/16)*cos((2*y+1)*PI*mu/16)*frequence[lambda][mu];
        }
        sommel += sommem;
      }
      spatial[x][y] = saturation(sommel/4 + 128);
    }
  }
}

/* Implémentation de IDCT optimisée */
void butterfly_unit(float *x0, float *x1){
  float temporaire_0 = *x0;
  float temporaire_1 = *x1;
  *x0= (temporaire_0 + temporaire_1)/2;
  *x1 = (temporaire_0 - temporaire_1)/2;
}

void rotator_unit(float *x0, float *x1,float k, int n){
  float locale_0 = *x0;
  float locale_1 = *x1;
  *x0 = locale_0*(1/k)*cos((n*PI)/16)-locale_1*(1/k)*sin((n*PI)/16);
  *x1 = locale_1*(1/k)*cos((n*PI)/16)+locale_0*(1/k)*sin((n*PI)/16);
}


void algo_loeffler(float *vecteur){
  /* Permet de calculer une seule fois la valeur de
  racine(8) et racine(2) à la place de 8 fois dans
  la boucle */
  float racine_2 = sqrt(2);
  float racine_8 = sqrt(8);

  for(int i = 0; i < 8; i++){
    vecteur[i] *= racine_8;
  }

  /* STAGE 1 */
  vecteur[3] /= racine_2;
  vecteur[5] /= racine_2;
  butterfly_unit(&vecteur[1], &vecteur[7]);

  /* STAGE 2 */
  butterfly_unit(&vecteur[0], &vecteur[4]);
  rotator_unit(&vecteur[2], &vecteur[6],racine_2, 6);
  butterfly_unit(&vecteur[7], &vecteur[5]);
  butterfly_unit(&vecteur[1], &vecteur[3]);

  /* STAGE 3 */
  butterfly_unit(&vecteur[0], &vecteur[6]);
  butterfly_unit(&vecteur[4], &vecteur[2]);
  rotator_unit(&vecteur[7], &vecteur[1],1,3);
  rotator_unit(&vecteur[3], &vecteur[5],1,1);

  /* STAGE 4 */
  butterfly_unit(&vecteur[0], &vecteur[1]);
  butterfly_unit(&vecteur[4], &vecteur[5]);
  butterfly_unit(&vecteur[2], &vecteur[3]);
  butterfly_unit(&vecteur[6], &vecteur[7]);

  /* PERMUTATION */
  float vecteur_locale[8];
    vecteur_locale[0] = vecteur[0];
    vecteur_locale[7] = vecteur[1];
    vecteur_locale[2] = vecteur[2];
    vecteur_locale[5] = vecteur[3];
    vecteur_locale[1] = vecteur[4];
    vecteur_locale[6] = vecteur[5];
    vecteur_locale[3] = vecteur[6];
    vecteur_locale[4] = vecteur[7];

  for(int i = 0; i < 8; i++){
    vecteur[i] = vecteur_locale[i];
  }
}

void idct_rapide(int **frequence, uint8_t **spatial){
  /* On crée une matrice temporaire de type float sur laquelle
     on effectue tous les calculs */
  float matrice_locale[8][8];
  for(int i =0; i < 8; i++){
    for(int j = 0; j< 8; j++){
      matrice_locale[i][j] = (float)frequence[i][j];
    }
    /* On applique l'algorithme de Loeffler sur chacune des
       lignes de la matrice */
    algo_loeffler(matrice_locale[i]);
  }
  /* On transpose la matrice afin de pouvoir executer de
     nouveau l'algorithme de Loeffler sur les colonnes */
  transposee_matrice(matrice_locale);
  for(int i = 0; i < 8; i++){
    algo_loeffler(matrice_locale[i]);
  }
  /* On retranspose la matrice temporaire afin d'obtenir
     la matrice initiale */
  transposee_matrice(matrice_locale);
  for(int i =0; i < 8; i++){
    for(int j = 0; j< 8; j++){
      /* On sature si besoin et caste en uint8_t chacun
        des coefficients de la matrice locale */
      spatial[i][j] = saturation(matrice_locale[i][j] + 128);
    }
  }
}
