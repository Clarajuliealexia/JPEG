#include "../include/outils.h"


void dec_to_hex(int dec, char *hexadecimal){

  if(dec<0){
    dec_to_hex(65536+dec, hexadecimal);

  }
  else{
    long quotient, remainder;
    int j = 0;
    quotient = dec;
    while (quotient != 0)
    {
    remainder = quotient % 16;          //step 1
    if (remainder < 10)
    hexadecimal[j++] = 48 + remainder;   //step 2
    else
    hexadecimal[j++] = 55 + remainder;   //step 3
    quotient = quotient / 16;            //step 4
    }
    // printf("en hex: ");
    // for (i = j; i >= 0; i--){
    //   printf("%c", hexadecimal[i]);
    // }
    // printf("\n");
  }

  //step 6
  // for (i = j; i >= 0; i--)
  // printf("%c", hexadecimal[i]);
  // return 0;
}

int char_to_int(char hex[0]){
  switch (hex[0]){
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'A':
      return 10;
    case 'B':
      return 11;
    case 'C':
      return 12;
    case 'D':
      return 13;
    case 'E':
      return 14;
    case 'F':
      return 15;
    default:
      return false;
  }
}

void print_vecteur64(int *vecteur){
  for (int i = 0; i < 64; i++){
    printf("%x  ", vecteur[i]);
  }
  printf("\n");
}

void print_vecteur64uint(uint32_t *vecteur){
  for (int i = 0; i < 64; i++){
    printf("%x  ", vecteur[i]);
  }
  printf("\n");
}

void init_vecteur64(int *vecteur){
  for (int i = 0; i < 64; i++){
    vecteur[i] = 0;
  }
}

void init_matrice88(int **matrice){
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++ ){
      matrice[i][j] = 0;
    }
  }
}

void init_matrice88_uint(uint8_t **matrice){
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++ ){
      matrice[i][j] = 0;
    }
  }
}

void print_matrice88(int **matrice){
  for (int j = 0; j < 8; j++){
    for (int i = 0; i < 8; i++){
      printf("%x ", matrice[i][j]);
    }
    printf("\n");
  }
}

void print_matrice88_uint(uint8_t **matrice){
  for (int j = 0; j < 8; j++){
    for (int i = 0; i < 8; i++){
      printf("%x ", matrice[i][j]);
    }
    printf("\n");
  }
}

void print_matrice88_float(float **matrice){
  for (int j = 0; j < 8; j++){
    for (int i = 0; i < 8; i++){
      printf("%f ", matrice[i][j]);
    }
    printf("\n");
  }
}

uint8_t saturation(float pixel){
  if (pixel < 0){
    return 0;
  }
  else if (pixel > 255){
    return 255;
  }
  return (uint8_t)pixel;
}

uint32_t **allocation_matrice(int n, int m){
  /*allocation des lignes*/
  uint32_t ** matrice = malloc(sizeof(uint32_t*)*n);
  /*allocation des colonnes*/
  for (int i = 0; i < n; i++){
    matrice[i] = malloc(sizeof(uint32_t)*m);
  }
  return matrice;
}

uint8_t **allocation_matrice_64(){
  /*allocation des lignes*/
  uint8_t ** matrice = malloc(sizeof(uint8_t*)*8);
  /*allocation des colonnes*/
  for (int i = 0; i < 8; i++){
    matrice[i] = malloc(sizeof(uint8_t)*8);
  }
  return matrice;
}

void free_matrice_64(uint8_t **matrice){
  /*on free chaque colonne*/
  for (int i = 0; i < 8; i++){
    free(matrice[i]);
  }
  /*on free les lignes*/
  free(matrice);
}


void free_matrice(uint32_t **matrice, int n){
  /*on free chaque colonne*/
  for (int i = 0; i < n; i++){
    free(matrice[i]);
  }
  /*on free les lignes*/
  free(matrice);
}

uint8_t *** my_malloc_ligne_of_ptr(uint32_t taille){
  uint8_t ***ligne_Y = malloc(sizeof(uint8_t**)*taille);
  for(uint32_t id = 0; id < taille; id++){
    ligne_Y[id] = allocation_matrice_64();
  }
  return ligne_Y;
}

void my_free_ligne_of_ptr(uint8_t taille, uint8_t ***ligne_Y){
  for(uint32_t id = 0; id < taille; id++){
    free_matrice_64(ligne_Y[id]);
  }
  free(ligne_Y);
}

void transposee_matrice(float matrice[8][8]){
  float matrice_locale[8][8];
  for(int i = 0; i <8 ; i++){
    for(int j = 0; j < 8; j++){
      matrice_locale[i][j] = matrice[i][j];
    }
  }
  for(int i = 0; i <8 ; i++){
    for(int j = 0; j < 8; j++){
      matrice[i][j] = matrice_locale[j][i];
    }
  }
}
