#include "../include/3_ecriture_PPM.h"
/*Écriture du résultat dans le fichier PPM*/

FILE * ecrire_entete(int nb_component_byMCU, uint16_t largeur, uint16_t hauteur){
  FILE *fichier = NULL;
  /* cas couleur */
  if(nb_component_byMCU > 1){
    fichier = fopen("images/resultat1.ppm", "wb");
  }
  /* cas noir et blanc */
  else{
    fichier = fopen("images/resultat1.pgm", "wb");
  }

  if (fichier != NULL){
    /* cas couleur */
    if(nb_component_byMCU > 1){
      entetePPM(fichier, largeur, hauteur);
    }
    /* cas noir et blanc */
    else{
      entetePGM(fichier, largeur, hauteur);
    }
  }
  return fichier;
}

void entetePGM(FILE *fichier, uint16_t largeur, uint16_t hauteur){
  // printf("hauteur : %u \n", hauteur);
  // printf("largeur : %u \n", largeur);
  char var[3] = "P5\n";
  fwrite(&var,sizeof(var), 1, fichier);
  char largeurc[5];
  sprintf(largeurc, "%u", largeur);
  // printf("largeurc : %s \n", largeurc);
  fwrite(&largeurc,sizeof(largeurc), 1, fichier);
  char espace[1] = " ";
  fwrite(&espace,sizeof(espace), 1, fichier);
  char hauteurc[5];
  sprintf(hauteurc, "%u", hauteur);
  fwrite(&hauteurc,sizeof(hauteurc), 1, fichier);
  // printf("largeurc : %s \n", largeurc);
  char retourligne[1] = "\n";
  fwrite(&retourligne,sizeof(retourligne), 1, fichier);
  char var2[4] = "255\n";
  fwrite(&var2,sizeof(var2), 1, fichier);
}

void entetePPM(FILE *fichier, uint16_t largeur, uint16_t hauteur){
  // printf("hauteur : %u \n", hauteur);
  // printf("largeur : %u \n", largeur);
  char var[3] = "P6\n";
  fwrite(&var,sizeof(var), 1, fichier);
  char largeurc[5];
  sprintf(largeurc, "%u", largeur);
  // printf("largeurc : %s \n", largeurc);
  fwrite(&largeurc,sizeof(largeurc), 1, fichier);
  char espace[1] = " ";
  fwrite(&espace,sizeof(espace), 1, fichier);
  char hauteurc[5];
  sprintf(hauteurc, "%u", hauteur);
  fwrite(&hauteurc,sizeof(hauteurc), 1, fichier);
  // printf("largeurc : %s \n", largeurc);
  char retourligne[1] = "\n";
  fwrite(&retourligne,sizeof(retourligne), 1, fichier);
  char var2[4] = "255\n";
  fwrite(&var2,sizeof(var2), 1, fichier);
}

void ecrire_RGB(uint8_t *RGB ,FILE *fichier){
  fwrite(&RGB[0], sizeof(uint8_t), 1, fichier);
  fwrite(&RGB[1], sizeof(uint8_t), 1, fichier);
  fwrite(&RGB[2], sizeof(uint8_t), 1, fichier);
}

void ecrire_pixel(uint8_t nb_component_byMCU,
                  uint8_t ***ligne_Y,
                  uint8_t ***ligne_Cb,
                  uint8_t ***ligne_Cr,
                  int id_colonne_bloc, uint8_t id_colonne, uint8_t id_ligne,
                  FILE *fichier){
  /* cas couleur */
  if(nb_component_byMCU > 1){

      uint8_t *RGB = conversionRGB(ligne_Y[id_colonne_bloc][id_colonne][id_ligne],
                                  ligne_Cb[id_colonne_bloc][id_colonne][id_ligne],
                                  ligne_Cr[id_colonne_bloc][id_colonne][id_ligne]);
      ecrire_RGB(RGB, fichier);
      free(RGB);
  }
  /* cas noir et blanc */
  else{
    uint8_t val = ligne_Y[id_colonne_bloc][id_colonne][id_ligne];
    fwrite(&val, sizeof(uint8_t), 1, fichier);
  }
}


void ecrire_ligne_de_bloc( int nb_bloc_ligne, int id_bloc, int nb_bloc,
  int tronc_droit, int tronc_bas, int nb_component_byMCU,
  uint8_t ***ligne_Y, uint8_t ***ligne_Cb, uint8_t ***ligne_Cr,
  FILE *fichier){

  /* id_ligne et id_colonne servent pour parcourir les donnees des blocs
     d'une composante */
  for (int id_ligne = 0; id_ligne < 8; id_ligne++){
    for (int id_colonne_bloc = 0; id_colonne_bloc < nb_bloc_ligne; id_colonne_bloc++){
      for (int id_colonne = 0; id_colonne < 8; id_colonne++){

        /* cas du dernier bloc */
        if(id_colonne_bloc == nb_bloc_ligne-1 && id_bloc == nb_bloc){
          if(id_colonne < 8-tronc_droit && id_ligne < 8-tronc_bas){
            ecrire_pixel(nb_component_byMCU, ligne_Y, ligne_Cb, ligne_Cr,
              id_colonne_bloc, id_colonne, id_ligne, fichier);
          }
        }

        /* cas tronc a droite */
        //dernier bloc de chaque ligne
        else if (id_colonne_bloc == nb_bloc_ligne - 1){
          if(id_colonne < 8-tronc_droit){
            ecrire_pixel(nb_component_byMCU, ligne_Y, ligne_Cb, ligne_Cr,
              id_colonne_bloc, id_colonne, id_ligne, fichier);
          }
        }

        /* tronc bas */
        // derniere ligne de bloc
        else if (id_bloc == nb_bloc){
          if(id_ligne < 8-tronc_bas){
            ecrire_pixel(nb_component_byMCU, ligne_Y, ligne_Cb, ligne_Cr,
              id_colonne_bloc, id_colonne, id_ligne, fichier);
          }
        }

        /* cas normal */
        else{
          ecrire_pixel(nb_component_byMCU, ligne_Y, ligne_Cb, ligne_Cr,
            id_colonne_bloc, id_colonne, id_ligne, fichier);
        }
      }
    }
  }
}
