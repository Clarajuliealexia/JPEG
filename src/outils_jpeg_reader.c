#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>


void init(struct jpeg_desc *jdesc){
  jdesc->marker_JPEG = false;
  jdesc->taille_app0 = 0;
  jdesc->ignored_app0 = 0;
  jdesc->nombre_table_quantification = 0;
  jdesc->taille_dqt = 0;
  jdesc->table_quantification = NULL;
  jdesc->precision_dqt = NULL;
  jdesc->taille_sof0 = 0;
  jdesc->precision_sof0 = 0;
  jdesc->hauteur = 0;
  jdesc->largeur = 0;
  jdesc->nombre_composante = 0;
  jdesc->info_composante = NULL;
  jdesc->taille_dht = 0;
  jdesc->index_table = 0;
  jdesc->nombre_code_huffman = NULL;
  jdesc->table_huffman =  NULL;
  jdesc->taille_sos = 0;
  jdesc->nb_compo = 0;
  jdesc->info_compo = NULL;
  jdesc->ignoreg_sos = 0;
}

void SOI(struct bitstream *stream, struct jpeg_desc *jdesc){
  printf("[SOI]\t ");
  /*On vérifie que le fichier est bien un fichier JPEG*/
  jdesc->marker_JPEG = true;
  printf("marker found \n");
}

void app0(struct bitstream *stream, struct jpeg_desc *jdesc){
  printf("[APP0]\t ");
  uint32_t taille_app0;
  read_bitstream(stream, 16, &taille_app0, false);
  jdesc->taille_app0 = taille_app0;
  printf("length %u bytes \n", jdesc->taille_app0);
  /*On lit JFIF*/
  uint32_t j;
  read_bitstream(stream, 8, &j, false);
  //printf("%c \n", j);
  uint32_t f;
  read_bitstream(stream, 8, &f, false);
  //printf("%c \n", f);
  uint32_t i;
  read_bitstream(stream, 8, &i, false);
  //printf("%c \n", i);
  uint32_t ff;
  read_bitstream(stream, 8, &ff, false);
  //printf("%c \n", ff);
  uint32_t o;
  read_bitstream(stream, 8, &o, false);
  //printf("%c \n", o);
  printf("\t JFIF application \n");
  // nombre d'octets ignorés
  jdesc->ignored_app0 = jdesc->taille_app0 - 7; //2 pour la taille et 5 pour JFIF
  printf("\t other parameters ignoreg (%u bytes)\n", jdesc->ignored_app0);
  uint32_t inutile;
  /*on passe le reste du bloc*/
  for (int i = 0; i < 8; i++){
    read_bitstream(stream, jdesc->ignored_app0, &inutile, false);
  }
}

void dqt(struct bitstream * stream, struct jpeg_desc *jdesc){
  printf("[DQT]\t ");
  /*longueur du bloc*/
  uint32_t taille_dqt;
  read_bitstream(stream, 16, &taille_dqt, false);
  jdesc->taille_dqt = taille_dqt;
  printf("length %u bytes \n", jdesc->taille_dqt);
  /*calcul du nombre de tables de quantification*/
  int nombre_table_quantification = (jdesc->taille_dqt - 1) / (1 + 64);
  jdesc->nombre_table_quantification = nombre_table_quantification;
  printf("\t nombre de table de quantification %u \n", jdesc->nombre_table_quantification);
  /*on récupère les tables de quantification et les précisions*/
//  int table_quantification[nombre_table_quantification][64];
  // int jdesc->table_quantification_zigzag[nombre_table_quantification][8][8];
  // int jdesc->precision_dqt[nombre_table_quantification];
  jdesc->table_quantification = allocation_matrice(nombre_table_quantification, 64);
  jdesc->precision_dqt = malloc(sizeof(uint32_t)*nombre_table_quantification);
  for (int i = 0; i < nombre_table_quantification; i++){
    /*on lit la précision*/
    uint32_t precision;
    read_bitstream(stream, 4, &precision, false);
    /*on lit l'indice de la table*/
    uint32_t indice;
    read_bitstream(stream, 4, &indice, false);
    if (precision == 0){
      jdesc->precision_dqt[indice] = 8;
    }
    else {
      jdesc->precision_dqt[indice] = 16;
    }
    printf("\t \t quantization table index %u \n", indice);
    printf("\t \t quantization precision %u bits \n", jdesc->precision_dqt[indice]);
    for (int j = 0; j < 64; j++){
      /*on remplit les matrices de quantification avec les 64 valeurs*/
      read_bitstream(stream, 8, &(jdesc->table_quantification[indice][j]), false);
    }
    printf("\t \t quantization table read (64 bytes)\n");
    /*on met la table sous forme de matrice en utilisant zigzag inverse*/
  //  zigzag(table_quantification[indice], jdesc->table_quantification[indice]);
  }
}

void sof0(struct bitstream * stream, struct jpeg_desc *jdesc){
  printf("[SOF0]\t ");
  /*longueur du bloc*/
  uint32_t taille_sof0;
  read_bitstream(stream, 16, &taille_sof0, false);
  jdesc->taille_sof0 = taille_sof0;
  printf("length %u bytes \n", jdesc->taille_sof0);
  uint32_t precision_sof0;
  /*précision en bits par composante*/
  read_bitstream(stream, 8, &precision_sof0, false);
  jdesc->precision_sof0 = precision_sof0;
  printf("\t sample precision %u \n", jdesc->precision_sof0);
  /*hauteur*/
  uint32_t hauteur;
  read_bitstream(stream, 16, &hauteur, false);
  jdesc->hauteur = hauteur;
  printf("\t image height %u \n", jdesc->hauteur);
  /*largeur*/
  uint32_t largeur;
  read_bitstream(stream, 16, &largeur, false);
  jdesc->largeur = largeur;
  printf("\t image weight %u \n", jdesc->largeur);
  /*nombre de composantes*/
  uint32_t nombre_composante;
  read_bitstream(stream, 8, &nombre_composante, false);
  jdesc->nombre_composante = nombre_composante;
  printf("\t nombre of component %u \n", jdesc->nombre_composante);
  // int jdesc->info_composante[nombre_composante][4];
  jdesc->info_composante = allocation_matrice(nombre_composante, 4);
  for (uint32_t i = 0; i < jdesc->nombre_composante; i++){
    if (i == 0){
      printf("\t component Y \n");
    }
    else if (i == 1){
      printf("\t component Cb \n");
    }
    else if (i == 2){
      printf("\t component Cr \n");
    }
    /*identifiant de composante*/
    read_bitstream(stream, 8, &jdesc->info_composante[i][0], false);
    printf("\t \t id %u \n", jdesc->info_composante[i][0]);
    /*facteur d'échantillonnage horizonal*/
    read_bitstream(stream, 4, &jdesc->info_composante[i][1], false);
    /*facteur d'échantillonnage vertical*/
    read_bitstream(stream, 4, &jdesc->info_composante[i][2], false);
    printf("\t \t sampling factors (hxv) %ux%u \n", jdesc->info_composante[i][1], jdesc->info_composante[i][2]);
    /*table de quantification associée*/
    read_bitstream(stream, 8, &jdesc->info_composante[i][3], false);
    printf("\t \t quantization table index %i\n", jdesc->info_composante[i][3]);
  }
}

void dht(struct bitstream * stream, struct jpeg_desc *jdesc){
  printf("[DHT]\t ");
  /*longueur de la section*/
  uint32_t taille_dht;
  read_bitstream(stream, 16, &taille_dht, false);
  jdesc->taille_dht = taille_dht;
  printf("length %u bytes \n", jdesc->taille_dht);
  /*information sur la table de huffman*/
  uint32_t zero;
  read_bitstream(stream, 3, &zero, false);
  if (zero != 0){
    printf("Erreur DHT \n");
    return;
  }
  uint32_t type;
  read_bitstream(stream, 1, &type, false);
  if (type == 0){
    printf("\t Huffman table type DC\n");
  }
  else {
    printf("\t Huffman table type AC\n");
  }
  uint32_t indice;
  read_bitstream(stream, 4, &indice, false);
  printf("\t Huffman table index %u \n", indice);
  for (int i = 0; i < 16; i++){
    uint32_t nombre_symbole;
    read_bitstream(stream, 8, &nombre_symbole, false);
    jdesc->nombre_code_huffman += nombre_symbole;
  }
  printf("\t total nb of Huffman codes %u \n", jdesc->nombre_code_huffman);
  jdesc->table_huffman = malloc(sizeof(uint32_t)*jdesc->nombre_code_huffman);
  for (uint32_t i = 0; i < jdesc->nombre_code_huffman; i++){
    read_bitstream(stream, 8, &jdesc->table_huffman[i], false);
  }
}

void sos(struct bitstream *stream, struct jpeg_desc *jdesc){
  printf("[SOS] \t ");
  uint32_t taille_sos;
  read_bitstream(stream, 16, &taille_sos, false);
  jdesc->taille_sos = taille_sos;
  printf("lenght %u bytes \n", jdesc->taille_sos);
  uint32_t nb_compo;
  read_bitstream(stream, 8, &nb_compo, false);
  jdesc->nb_compo = nb_compo;
  printf("\t nb of components in scan %u \n", nb_compo);
  jdesc->info_compo = allocation_matrice(nb_compo, 3);
  for (uint32_t i = 0; i < nb_compo; i++){
    printf("\t scan component index %u \n", i);
    read_bitstream(stream, 8, &jdesc->info_compo[i][0], false);
    printf("\t \t associated to component of id %u \n", jdesc->info_compo[i][0]);
    read_bitstream(stream, 4, &jdesc->info_compo[i][1], false);
    printf("\t \t associated to DC Huffman table of index %u \n", jdesc->info_compo[i][1]);
    read_bitstream(stream, 4, &jdesc->info_compo[i][2], false);
    printf("\t \t associated to AC Huffman table of index %u \n", jdesc->info_compo[i][2]);
  }
  jdesc->ignoreg_sos = 3;
  printf("\t other parameters ignored (%u bytes)\n", jdesc->ignoreg_sos);
  uint32_t autre;
  read_bitstream(stream, 24, &autre, false);
  printf("\t End of Scan Header (SOS)\n");
}
