#include "../include/bitstream.h"

void maj_buffer(struct bitstream *stream, bool discard_byte_stuffing){
  if (discard_byte_stuffing == 1){
    if (stream->est_ff == 1){
      // on ne lit pas le 00
      int nb_lu = 0;
      uint8_t inutile;
      nb_lu = fread(&inutile, sizeof(uint8_t), 1, stream->fichier);
      nb_lu = fread(&stream->buffer, sizeof(uint8_t), 1, stream->fichier);
      if (stream->buffer == 0xff){
        stream->est_ff = true;
      }
      else {
        stream->est_ff = false;
      }
      stream->nb_restant = 8;

    }
    else {
      int nb_lu = 0;
      nb_lu = fread(&stream->buffer, sizeof(uint8_t), 1, stream->fichier);
      stream->nb_restant = 8;
      if (stream->buffer == 0xff){
        stream->est_ff = true;
      }
    }
  }
  else {
    int nb_lu = 0;
    nb_lu = fread(&stream->buffer, sizeof(uint8_t), 1, stream->fichier);
    stream->nb_restant = 8;
  }
}

struct bitstream *create_bitstream (const char *filename){
  // on alloue le stream
  struct bitstream *stream = malloc(sizeof(struct bitstream));
  // on ouvre le fichier en mode lecture par octet
  stream->fichier = fopen(filename, "rb");
  if (stream->fichier == NULL){
    fprintf(stderr, "Erreur d'ouverture \n");
    return NULL;
  }
  // initialisation buffer
  int nb_lu = 0;
  nb_lu = fread(&stream->buffer, sizeof(uint8_t), 1, stream->fichier);
  stream->nb_restant = 8;
  stream->est_ff = false;
  return stream;
}


void close_bitstream (struct bitstream *stream){
  // on ferme le fichier
  fclose(stream->fichier);
  // on free la structure
  free(stream);
}


uint8_t lit_1_bit(struct bitstream *stream){
  uint8_t lu = (stream->buffer >> 7);
  stream->buffer = (stream->buffer << 1);
  stream->nb_restant -= 1;
  return lu;
}


uint8_t read_bitstream (struct bitstream *stream,
                              uint8_t nb_bits,
                              uint32_t *dest,
                              bool discard_byte_stuffing){
  *dest = 0;
  uint8_t nb_lu = 0;
  uint8_t lu;
  for (uint8_t i = 0; i < nb_bits; i++){
    lu  = 0;
    if (stream->nb_restant == 0){
      // si le buffer est vide, on le re rempli
      maj_buffer(stream, discard_byte_stuffing);
    }
    // on lit un bit
    lu = lit_1_bit(stream);
    nb_lu += 1;
    *dest = (*dest << 1) + lu;
  }
  return nb_lu;
}


bool end_of_bitstream (struct bitstream *stream){
  uint32_t marker;
  read_bitstream (stream, 16, &marker, false);
  if (marker == 0xffd9){
    return true;
  }
  else {
    return false;
  }
}
