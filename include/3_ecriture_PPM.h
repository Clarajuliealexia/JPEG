#include "2c_conversion_RGB.h"
#include <stdint.h>

/*Écriture du résultat dans le fichier PPM*/

/* Cette fonction permet d'écrire l'entete du fichier en fonction du nombre
de composantes*/
FILE * ecrire_entete(int nb_component_byMCU, uint16_t largeur, uint16_t hauteur);

/* Entete fichier pgm*/
void entetePGM(FILE *fichier, uint16_t largeur, uint16_t hauteur);

/* Entete PPM*/
void entetePPM(FILE *fichier, uint16_t largeur, uint16_t hauteur);

/* Ecriture en RGB*/
void ecrire_RGB(uint8_t *RGB ,FILE *fichier);

/* Ecriture d'un pixel en RGB si on est en couleur ou la composante sinon*/
void ecrire_pixel(uint8_t nb_component_byMCU,
                  uint8_t ***ligne_Y,
                  uint8_t ***ligne_Cb,
                  uint8_t ***ligne_Cr,
                  int id_colonne_bloc, uint8_t id_colonne, uint8_t id_ligne,
                  FILE *fichier);

/* Ecriture d'une ligne de bloc */
void ecrire_ligne_de_bloc( int nb_bloc_ligne, int id_bloc, int nb_bloc,
  			   int tronc_droit, int tronc_bas, int nb_component_byMCU,
  			   uint8_t ***ligne_Y, uint8_t ***ligne_Cb, uint8_t ***ligne_Cr,
  			   FILE *fichier);
