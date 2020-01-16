#include "../include/2_decodage_MCU.h"
#include "../include/3_ecriture_PPM.h"
#include "../include/outils.h"

#include <time.h>

int main(int argc, char **argv){

  float temps;
  clock_t t2, debut;
  debut = clock();

  if (argc != 2) {
  	/* Si y'a pas au moins un argument en ligne de commandes, on* boude. */
  	fprintf(stderr, "Usage: %s fichier.jpeg\n", argv[0]);
  	return EXIT_FAILURE;
  }

  /* On recupere le nom du fichier JPEG sur la ligne de commande. */
  const char *filename = argv[1];

  /* On cree un jpeg_desc qui permettra de lire ce fichier. */
  struct jpeg_desc *jdesc = read_jpeg (filename);

  /* On recupere le flux des donnees brutes a partir du descripteur. */
  struct bitstream *stream = get_bitstream (jdesc);

  /* Nombre de MCU  */
  uint16_t hauteur = get_image_size (jdesc, DIR_V);
  uint16_t largeur = get_image_size (jdesc, DIR_H);

  int nb_component_byMCU = get_nb_components (jdesc);

  uint8_t val_echantillonnage_horiY = get_frame_component_sampling_factor (jdesc, DIR_H, 0);
  uint8_t val_echantillonnage_vertiY = get_frame_component_sampling_factor (jdesc, DIR_V, 0);
  uint8_t val_echantillonnage_horiCb = 0;
  uint8_t val_echantillonnage_vertiCb = 0;
  uint8_t val_echantillonnage_horiCr = 0;
  uint8_t val_echantillonnage_vertiCr = 0;
  if(nb_component_byMCU > 1){
    val_echantillonnage_horiCb = get_frame_component_sampling_factor (jdesc, DIR_H, 1);
    val_echantillonnage_vertiCb = get_frame_component_sampling_factor (jdesc, DIR_V, 1);
    val_echantillonnage_horiCr = get_frame_component_sampling_factor (jdesc, DIR_H, 2);
    val_echantillonnage_vertiCr = get_frame_component_sampling_factor (jdesc, DIR_V, 2);
  }

  int nb_mcu_ligne = ceil(largeur/(8.0*val_echantillonnage_horiY));
  int nb_mcu_colonne = ceil(hauteur/(8.0*val_echantillonnage_vertiY));
  int nb_MCU= nb_mcu_ligne*nb_mcu_colonne;

  int nb_bloc = nb_MCU*val_echantillonnage_horiY*val_echantillonnage_vertiY;
  int nb_bloc_ligne = ceil(largeur/8.0);
  int nb_bloc_colonne = ceil(hauteur/8.0);

/*-----------------------------------------------------------------------*/
  /* Ecriture de l'en-tête */
  FILE *fichier = ecrire_entete(nb_component_byMCU, largeur, hauteur);

/*-----------------------------------------------------------------------*/
  /* déclaration de variables */
  int predicateur = 0;
  int id_bloc = 0;

  /* cas couleur */
  int predicateur_Cb = 0;
  int predicateur_Cr = 0;

/*-----------------------------------------------------------------------*/
  /* calcul des tronc */
  int tronc_droit = (nb_bloc_ligne*8 - largeur)%8;
  int tronc_bas = (nb_bloc_colonne*8 - hauteur)%8;

/*-----------------------------------------------------------------------*/
  /* déclarations de variables */

  /* FAKE car pas forcement pareil si nb_bloc_ligne impair */
  int nb_bloc_ligne_FAKE = nb_mcu_ligne*val_echantillonnage_horiY;

  uint8_t ***ligne_Y = NULL;

  /* cas couleur */
  uint8_t ***ligne_Cb = NULL;
  uint8_t ***ligne_Cr = NULL;

  /* cas echantillonnage vertical */
  uint8_t ***ligne_Y2 = NULL;
  uint8_t ***ligne_Cb2 = NULL;
  uint8_t ***ligne_Cr2 = NULL;

  uint8_t ***ligne_Y3 = NULL;
  uint8_t ***ligne_Cb3 = NULL;
  uint8_t ***ligne_Cr3 = NULL;
  uint8_t ***ligne_Y4 = NULL;
  uint8_t ***ligne_Cb4 = NULL;
  uint8_t ***ligne_Cr4 = NULL;

/*-----------------------------------------------------------------------*/
  /* allocation des lignes */
  ligne_Y = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);

  /* cas couleur */
  uint8_t **idc_Cb = NULL;
  uint8_t **idc_Cr = NULL;
  if(nb_component_byMCU > 1){
    idc_Cb = allocation_matrice_64();
    idc_Cr = allocation_matrice_64();
    ligne_Cb = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
    ligne_Cr = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
  }

  /* cas echantillonnage vertical */
  if(val_echantillonnage_vertiY > 1){
    ligne_Y2 = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
    ligne_Cb2 = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
    ligne_Cr2 = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
  }

  if(val_echantillonnage_vertiY == 4){
    ligne_Y3 = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
    ligne_Cb3 = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
    ligne_Cr3 = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
    ligne_Y4 = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
    ligne_Cb4 = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
    ligne_Cr4 = my_malloc_ligne_of_ptr(nb_bloc_ligne_FAKE);
  }

  uint8_t **idc_Cb2 = NULL;
  if(val_echantillonnage_horiCb > 1 || val_echantillonnage_vertiCb > 1){
    idc_Cb2 = allocation_matrice_64();
  }
  uint8_t **idc_Cr2 = NULL;
  if(val_echantillonnage_horiCr > 1 || val_echantillonnage_vertiCr > 1){
    idc_Cr2 = allocation_matrice_64();
  }

  uint8_t **idc_Cb3 = NULL;
  uint8_t **idc_Cb4 = NULL;
  if((val_echantillonnage_horiCb == 2 && val_echantillonnage_vertiCb == 2)
    || val_echantillonnage_horiCb == 4 || val_echantillonnage_vertiCb == 4){
    idc_Cb3 = allocation_matrice_64();
    idc_Cb4 = allocation_matrice_64();
  }
  uint8_t **idc_Cr3 = NULL;
  uint8_t **idc_Cr4 = NULL;
  if((val_echantillonnage_horiCr == 2 && val_echantillonnage_vertiCr == 2)
    || val_echantillonnage_horiCr == 4 || val_echantillonnage_vertiCr == 4){
    idc_Cr3 = allocation_matrice_64();
    idc_Cr4 = allocation_matrice_64();
  }
/*-----------------------------------------------------------------------*/

  /* Parcours chaque ligne de l'image
    (une ligne étant faite de MCU) */
  for (int id_ligne = 0; id_ligne < nb_mcu_colonne; id_ligne++){

    /* decodage et ecriture d'une ligne de MCU
      (ou deux si echantillonnage vertical == 2)
      parcours chaque bloc (Y/Cb/Cr) de chaque MCU d'une ligne de l'image*/
    for (int id_colonne = 0; id_colonne < nb_mcu_ligne; id_colonne++){
      int index = id_colonne*val_echantillonnage_horiY;

  /*-----------------------------------------------------------------------*/
      /* lecture et decodage du jpeg */

      /* cas normal (SANS echantillonnage)*/
      predicateur = decodage_Component(jdesc, stream, ligne_Y[index],
                        predicateur, 0,0);

      /* cas echantillonnage horizontal*/
      if(val_echantillonnage_horiY > 1){
        predicateur = decodage_Component(jdesc, stream, ligne_Y[index+1],
                            predicateur, 0,0);
      }
      if(val_echantillonnage_horiY == 4){
        predicateur = decodage_Component(jdesc, stream, ligne_Y[index+2],
                            predicateur, 0,0);
        predicateur = decodage_Component(jdesc, stream, ligne_Y[index+3],
                            predicateur, 0,0);
      }

      /* cas echantillonnage vertical */
      if(val_echantillonnage_vertiY > 1){
        predicateur = decodage_Component(jdesc, stream, ligne_Y2[index],
                            predicateur, 0,0);
      }
      if(val_echantillonnage_vertiY == 4){
        predicateur = decodage_Component(jdesc, stream, ligne_Y3[index],
                            predicateur, 0,0);
        predicateur = decodage_Component(jdesc, stream, ligne_Y4[index],
                            predicateur, 0,0);
      }

      /* cas echantillonnage horizontal ET vertical */
      if(val_echantillonnage_horiY > 1 && val_echantillonnage_vertiY > 1){
        predicateur = decodage_Component(jdesc, stream, ligne_Y2[index+1],
                            predicateur, 0,0);
      }

      /* cas couleur */
      if(nb_component_byMCU > 1){
        /* récuperation des données  Cb et Cr*/
        /* Cb */
        predicateur_Cb = decodage_Component(jdesc, stream, idc_Cb,
                                            predicateur_Cb,1,1);
        if(val_echantillonnage_horiCb > 1 || val_echantillonnage_vertiCb > 1){
          predicateur_Cb = decodage_Component(jdesc, stream, idc_Cb2,
                                              predicateur_Cb,1,1);
        }
        if((val_echantillonnage_horiCb == 2 && val_echantillonnage_vertiCb == 2)
          || val_echantillonnage_horiCb == 4 || val_echantillonnage_vertiCb == 4){
          predicateur_Cb = decodage_Component(jdesc, stream, idc_Cb3,
                                              predicateur_Cb,1,1);
          predicateur_Cb = decodage_Component(jdesc, stream, idc_Cb4,
                                              predicateur_Cb,1,1);
        }
        /* Cr */
        predicateur_Cr = decodage_Component(jdesc, stream, idc_Cr,
                                            predicateur_Cr,2,1);
        if(val_echantillonnage_horiCr > 1 || val_echantillonnage_vertiCr > 1){
          predicateur_Cr = decodage_Component(jdesc, stream, idc_Cr2,
                                              predicateur_Cr,2,1);
        }
        if((val_echantillonnage_horiCr == 2 && val_echantillonnage_vertiCr == 2)
          || val_echantillonnage_horiCr == 4 || val_echantillonnage_vertiCr == 4){
          predicateur_Cr = decodage_Component(jdesc, stream, idc_Cr3,
                                              predicateur_Cr,2,1);
          predicateur_Cr = decodage_Component(jdesc, stream, idc_Cr4,
                                              predicateur_Cr,2,1);
        }

        /* stockage des données  Cb et Cr dans les lignes respectives */

        /* cas echantillonnage horizontal ET vertical */
        /* val_echantillonnage_horiY et val_echantillonnage_vertiY
           ne peuvent pas être les deux égal à 4 en même temps
           de même 4*2 ou 2*4 n'est pas possible*/
        /* Y 4*1 */
        if (val_echantillonnage_horiY == 4){
          remplir_Chro_Y41(ligne_Cb, index, idc_Cb, idc_Cb2, idc_Cb3,
            idc_Cb4, val_echantillonnage_horiCb);
          remplir_Chro_Y41(ligne_Cr, index, idc_Cr, idc_Cr2, idc_Cr3,
            idc_Cr4, val_echantillonnage_horiCr);
        }
        /* Y 1*4 */
        else if (val_echantillonnage_vertiY == 4){
          remplir_Chro_Y14(ligne_Cb, ligne_Cb2, ligne_Cb3, ligne_Cb4, index,
            idc_Cb, idc_Cb2, idc_Cb3, idc_Cb4, val_echantillonnage_vertiCb);
          remplir_Chro_Y14(ligne_Cr, ligne_Cr2, ligne_Cr3, ligne_Cr4, index,
            idc_Cr, idc_Cr2, idc_Cr3, idc_Cr4, val_echantillonnage_vertiCr);
        }

        /* Y 2*2 */
        else if (val_echantillonnage_vertiY > 1 && val_echantillonnage_horiY > 1){
          remplir_Chro_Y22(ligne_Cb, ligne_Cb2, index, idc_Cb, idc_Cb2, idc_Cb3,
            idc_Cb4, val_echantillonnage_horiCb, val_echantillonnage_vertiCb);

          remplir_Chro_Y22(ligne_Cr, ligne_Cr2, index, idc_Cr, idc_Cr2, idc_Cr3,
            idc_Cr4, val_echantillonnage_horiCr, val_echantillonnage_vertiCr);
        }

        /* cas echantillonnage horizontal */
        /* Y 2*1 */
        else if (val_echantillonnage_horiY == 2){
          /* Cb */
          if(val_echantillonnage_horiCb == 2){
            for(int i = 0; i < 8; i++){
              for(int j = 0; j < 8; j++){
                ligne_Cb[index][i][j] = idc_Cb[i][j];
                ligne_Cb[index + 1][i][j] = idc_Cb2[i][j];
              }
            }
          }
          else{
            duplicate_horizontal(idc_Cb, ligne_Cb[index], ligne_Cb[index+1]);
          }
          /* Cr */
          if(val_echantillonnage_horiCr == 2){
            for(int i = 0; i < 8; i++){
              for(int j = 0; j < 8; j++){
                ligne_Cr[index][i][j] = idc_Cr[i][j];
                ligne_Cr[index + 1][i][j] = idc_Cr2[i][j];
              }
            }
          }
          else{
            duplicate_horizontal(idc_Cr, ligne_Cr[index], ligne_Cr[index+1]);
          }
        }

        /* cas echantillonnage vertical */
        /* Y 1*2 */
        else if (val_echantillonnage_vertiY == 2){
          /* Cb */
          if(val_echantillonnage_vertiCb == 2){
            for(int i = 0; i < 8; i++){
              for(int j = 0; j < 8; j++){
                ligne_Cb[index][i][j] = idc_Cb[i][j];
                ligne_Cb2[index][i][j] = idc_Cb2[i][j];
              }
            }
          }
          else{
            duplicate_vertical(idc_Cb, ligne_Cb[index], ligne_Cb2[index]);
          }
          /* Cr */
          if(val_echantillonnage_vertiCr == 2){
            for(int i = 0; i < 8; i++){
              for(int j = 0; j < 8; j++){
                ligne_Cr[index][i][j] = idc_Cr[i][j];
                ligne_Cr2[index][i][j] = idc_Cr2[i][j];
              }
            }
          }
          else{
            duplicate_vertical(idc_Cr, ligne_Cr[index], ligne_Cr2[index]);
          }
        }

        /* cas SANS echantillonnage*/
        else{
          for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
              ligne_Cb[index][i][j] = idc_Cb[i][j];
              ligne_Cr[index][i][j] = idc_Cr[i][j];
            }
          }
        }
      }
/*-----------------------------------------------------------------------*/
      id_bloc += 1;
      /* cas echantillonnage horizontal OU vertical */
      if(val_echantillonnage_vertiY > 1 || val_echantillonnage_horiY > 1){
        id_bloc += 1;
      }
      /* cas echantillonnage horizontal ET vertical */
      if(val_echantillonnage_vertiY > 1 && val_echantillonnage_horiY > 1){
        id_bloc += 2;
      }
    }
    /* fin decodage ligne */
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
    /* ecriture ligne*/
    ecrire_ligne_de_bloc(nb_bloc_ligne, id_bloc, nb_bloc, tronc_droit,
      tronc_bas, nb_component_byMCU, ligne_Y, ligne_Cb, ligne_Cr, fichier);
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
    /* ecriture ligne en plus (cas echantillonnage vertical) */
    if(val_echantillonnage_vertiY > 1){
      ecrire_ligne_de_bloc(nb_bloc_ligne, id_bloc, nb_bloc, tronc_droit,
        tronc_bas, nb_component_byMCU, ligne_Y2, ligne_Cb2, ligne_Cr2, fichier);
    }

    if(val_echantillonnage_vertiY == 4){
      ecrire_ligne_de_bloc(nb_bloc_ligne, id_bloc, nb_bloc, tronc_droit,
        tronc_bas, nb_component_byMCU, ligne_Y3, ligne_Cb3, ligne_Cr3, fichier);
      ecrire_ligne_de_bloc(nb_bloc_ligne, id_bloc, nb_bloc, tronc_droit,
        tronc_bas, nb_component_byMCU, ligne_Y4, ligne_Cb4, ligne_Cr4, fichier);
    }
    /* fin ecriture cas echantillonnage vertical */
/*-----------------------------------------------------------------------*/

  }
  fclose(fichier);

    t2 = clock();
    temps = (float)(t2-debut)/CLOCKS_PER_SEC;
    printf("temps total = %f\n", temps);
/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
  /* free des lignes */
  my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Y);

  /* cas couleur */
  if(nb_component_byMCU > 1){
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Cb);
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Cr);
    free_matrice_64(idc_Cb);
    free_matrice_64(idc_Cr);
    if(val_echantillonnage_horiCb > 1 || val_echantillonnage_vertiCb > 1){
      free_matrice_64(idc_Cb2);
    }
    if(val_echantillonnage_horiCr > 1 || val_echantillonnage_vertiCr > 1){
      free_matrice_64(idc_Cr2);
    }
    if((val_echantillonnage_horiCb == 2 && val_echantillonnage_vertiCb == 2)
      || val_echantillonnage_horiCb == 4 || val_echantillonnage_vertiCb == 4){
      free_matrice_64(idc_Cb3);
      free_matrice_64(idc_Cb4);
    }
    if((val_echantillonnage_horiCr == 2 && val_echantillonnage_vertiCr == 2)
      || val_echantillonnage_horiCr == 4 || val_echantillonnage_vertiCr == 4){
      free_matrice_64(idc_Cr3);
      free_matrice_64(idc_Cr4);
    }
  }

  /* cas echantillonnage vertical */
  if(val_echantillonnage_vertiY > 1){
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Y2);
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Cb2);
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Cr2);
  }

  if(val_echantillonnage_vertiY == 4){
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Y3);
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Cb3);
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Cr3);
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Y4);
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Cb4);
    my_free_ligne_of_ptr(nb_bloc_ligne_FAKE, ligne_Cr4);
  }
/*-----------------------------------------------------------------------*/

  /* Nettoyage de printemps : close_jpeg ferme aussi le bitstream
   * (voir Annexe C du sujet). */
  close_jpeg (jdesc);

  /* On se congratule. */
  return EXIT_SUCCESS;
}
