#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

struct bitstream{
  FILE *fichier;
  /* le buffer contient l'octet qu'on est entrain de lire*/
  uint8_t buffer;
  uint8_t nb_restant;
  /*permet de savoir si le buffer est ff*/
  bool est_ff;
};

/* Remplit le buffer
si le byte_stuffing est true et que stream->est_ff est true on saute un octet
qui correspond au 00 et on remet à jour le buffer*/
void maj_buffer(struct bitstream *stream, bool discard_byte_stuffing);

/* Création et initialisation de la structure bistream*/
struct bitstream *create_bitstream (const char *filename);

/* On libère la structure */
void close_bitstream (struct bitstream *stream);

/* Cette fonction permet de lire un bit dans le fichier */
uint8_t lit_1_bit(struct bitstream *stream);

/* Cette fonction permet de lire nb_bits dans le fichier
on met à jour la valeur de dest avec la valeur lu et on retourne le nombre
de bits lu*/
uint8_t read_bitstream (struct bitstream *stream,
                              uint8_t nb_bits,
                              uint32_t *dest,
                              bool discard_byte_stuffing);

/* Répond à la question : est-t-on à la fin du fichier ?*/
bool end_of_bitstream (struct bitstream *stream);

#endif
