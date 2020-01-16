#include "../include/2_decodage_progressif.h"
#include "../include/3_ecriture_PPM.h"
#include "../include/outils.h"

#include <time.h>



int main(int argc, char **argv)
{
    if (argc != 2) {
	/* Si y'a pas au moins un argument en ligne de commandes, on
	 * boude. */
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

    int nb_mcu_ligne = ceil(largeur/8.0);
    int nb_mcu_colonne = ceil(hauteur/8.0);
    int nb_MCU= nb_mcu_ligne*nb_mcu_colonne;

    int nb_bloc = nb_MCU;
    int nb_bloc_ligne = ceil(largeur/8.0);
    int nb_bloc_colonne = ceil(hauteur/8.0);
    uint8_t val_echantillonnage_hori = get_frame_component_sampling_factor (jdesc, DIR_H, 0);
    uint8_t val_echantillonnage_verti = get_frame_component_sampling_factor (jdesc, DIR_V, 0);
/*-----------------------------------------------------------------------*/
    /* Ecriture de l'en-tête */
    FILE *fichier = NULL;
    fichier = fopen("images/resultat.pgm", "wb");
    if (fichier != NULL){
      entetePGM(fichier, largeur, hauteur);
    }
/*-----------------------------------------------------------------------*/
    /* déclaration de variables */
    int **mem_bloc = malloc(sizeof(int*)*nb_MCU);
    for (int i = 0; i < nb_MCU; i++){
      mem_bloc[i] = malloc(sizeof(int)*64);
    }

    int predicateur = 0;
    int id_bloc = 0;
/*-----------------------------------------------------------------------*/
    /* calcul des tronc */
    int tronc_droit = (nb_bloc_ligne*8 - largeur)%8;
    int tronc_bas = (nb_bloc_colonne*8 - hauteur)%8;

    /*-----------------------------------------------------------------------*/
    /* FAKE car pas forcement pareil si nb_bloc_ligne impair */
    uint8_t ***ligne_Y = NULL;
/*-----------------------------------------------------------------------*/
    /* allocation des lignes */
    ligne_Y = my_malloc_ligne_of_ptr(nb_bloc_ligne);
/*-----------------------------------------------------------------------*/
    /* Parcours chaque ligne de l'image
      (une ligne étant faite de MCU) */
    for (int id_ligne = 0; id_ligne < nb_mcu_colonne; id_ligne++){

      /* decodage et ecriture d'une ligne de MCU
        (ou deux si echantillonnage vertical)
        parcours chaque bloc (Y/Cb/Cr) de chaque MCU d'une ligne de l'image*/
      for (int id_colonne = 0; id_colonne < nb_mcu_ligne; id_colonne++){
        /*-----------------------------------------------------------------------*/
        /* lecture et decodage du jpeg */
        //printf("id_bloc = %i \n", id_bloc);
        /* cas normal (SANS echantillonnage)*/
        predicateur = decodage_Component_progressif_DC(jdesc, stream, mem_bloc[id_bloc],
                                                      ligne_Y[id_colonne], predicateur, 0,0, jdesc->al);

/*-----------------------------------------------------------------------*/
        id_bloc += 1;
      }
      /* fin decodage ligne */
/*-----------------------------------------------------------------------*/
      /* ecriture ligne*/
      ecrire_ligne_de_bloc(nb_bloc_ligne, id_bloc, nb_bloc, tronc_droit,
        tronc_bas, nb_component_byMCU, ligne_Y, NULL, NULL, fichier);
    }
/*-----------------------------------------------------------------------*/
    /* fin de l'écriture*/
    fclose(fichier);
/*-----------------------------------------------------------------------*/
    /* free des lignes */
    free(ligne_Y);

    /* OK que pour invader */
    /* On récupère les coefficients AC*/
    for (int i = 0; i < 8; i++){
      /* On lit les entetes*/
      // on lit les bits restants dans le buffer
      uint32_t restant;
      read_bitstream (stream, stream->nb_restant, &restant, false);
      printf("\n");

      /*on regarde si on a une autre sos ou dht*/
      uint32_t marker_sup;
      read_bitstream (stream, 8, &marker_sup, false);
      printf("marker sup %x \n", marker_sup);
      while (marker_sup != 0xff){
        read_bitstream (stream, 8, &marker_sup, false);
        printf("marker sup %x \n", marker_sup);
      }
      uint32_t marker_inf;
      read_bitstream (stream, 8, &marker_inf, false);
      printf("marker inf %x \n", marker_inf);
      if (marker_inf == 0xd9){
        /* Nettoyage de printemps : close_jpeg ferme aussi le bitstream */
        for (int i = 0; i < nb_MCU; i++){
          free(mem_bloc[i]);
        }
        free(mem_bloc);
        close_jpeg (jdesc);
        /* On se congratule. */
        return EXIT_SUCCESS;
      }
      //printf("marker inf %x \n", marker_inf);
      if (marker_inf == 0xc4){
        printf("[DHT]\t ");
        parse_dht(jdesc, stream, 1);
        read_bitstream (stream, 8, &marker_sup, false);
        read_bitstream (stream, 8, &marker_inf, false);
      }
      /*on veut un sos*/
      if (marker_inf == 0xda){
        printf("[SOS]\t ");
        parse_sos(jdesc, stream);
      }
      else {
        printf("erreur \n");
      }
      /*on met à jour le stream*/
      jdesc->stream = stream;

    /*-----------------------------------------------------------------------*/
    /* On écrit le fichier image amélioré */
    /* Ecriture de l'en-tête */
    fichier = NULL;
    fichier = fopen("images/resultat1.pgm", "wb");

    if (fichier != NULL){
      entetePGM(fichier, largeur, hauteur);
    }
    /* On récupère les valeurs */
    /*-----------------------------------------------------------------------*/
    id_bloc = 0;
    /*-----------------------------------------------------------------------*/
    /* allocation des lignes */
    ligne_Y = my_malloc_ligne_of_ptr(nb_bloc_ligne);
    /*-----------------------------------------------------------------------*/
    /* Parcours chaque ligne de l'image
      (une ligne étant faite de MCU) */
    for (int id_ligne = 0; id_ligne < nb_mcu_colonne; id_ligne++){
      /* decodage et ecriture d'une ligne de MCU
        (ou deux si echantillonnage vertical)
        parcours chaque bloc (Y/Cb/Cr) de chaque MCU d'une ligne de l'image*/
      for (int id_colonne = 0; id_colonne < nb_mcu_ligne; id_colonne++){
        /*-----------------------------------------------------------------------*/
        /* lecture et decodage du jpeg */
        decodage_Component_progressif_AC(jdesc, stream, mem_bloc[id_bloc],
                                          ligne_Y[id_colonne], 0,0, jdesc->ss, jdesc->se, jdesc->al);
    /*-----------------------------------------------------------------------*/
        id_bloc += 1;
      }
      /* fin decodage ligne */
    /*-----------------------------------------------------------------------*/
      /* ecriture ligne*/
      ecrire_ligne_de_bloc(nb_bloc_ligne, id_bloc, nb_bloc, tronc_droit,
        tronc_bas, nb_component_byMCU, ligne_Y, NULL, NULL, fichier);
    }
    /*-----------------------------------------------------------------------*/
    /* fin de l'écriture*/
    fclose(fichier);
    /*-----------------------------------------------------------------------*/
    /* free des lignes */
    free(ligne_Y);
  }
/*-----------------------------------------------------------------------*/
}
