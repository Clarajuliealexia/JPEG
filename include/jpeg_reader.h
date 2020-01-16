#include "outils.h"
#include "huffman.h"
#include "bitstream.h"

/* Ce module permet de lire l'entete et de récupérer les valeurs utiles au décodage*/


struct jpeg_desc{
  const char* nom_fichier;
  struct bitstream * stream;

  /*DQT*/
  uint8_t nombre_table_quantification;
  uint32_t **table_quantification;

  /*SOF0*/
  uint16_t hauteur;
  uint16_t largeur;
  uint32_t nombre_composante;
  uint32_t **info_composante;

  /*DHT*/
  struct huff_table **table_huff;

  /*SOS*/
  uint32_t nb_compo;
  uint32_t **info_compo;
  uint32_t ignoreg_sos;
  /*utile pour le progressif*/
  uint8_t ss;
  uint8_t se;
  uint8_t ah;
  uint8_t al;
};

enum direction{
  DIR_H,
  DIR_V,
  DIR_NB
};

enum acdc {
    DC = 0,
    AC = 1,
    /* sentinelle */
    ACDC_NB
};

/* Lecture de l'entete APP0*/
void parse_appo(struct bitstream *stream);

/* Lecture de l'entete DQT*/
void parse_dqt(struct jpeg_desc *jdesc, struct bitstream *stream, int compteur);

/* Lecture de l'entete SOF0 et SOF2 (pour le progressif)*/
void parse_sof0(struct jpeg_desc *jdesc, struct bitstream *stream);

/* Lecture de l'entete DHT*/
void parse_dht(struct jpeg_desc *jdesc, struct bitstream *stream, int compteur);

/* Lecture de l'entete SOS*/
void parse_sos(struct jpeg_desc *jdesc, struct bitstream *stream);



/*read_jpeg ouvre le fichier JPEG, lit tous les entêtes de sections rencontrés et stocke les in-
formations requises dans le descripteur retourné-> La lecture est stoppée au début des données
brutes codant l’image (après l’entête de la section SOS) ;*/
struct jpeg_desc * read_jpeg  (const char * filename);

/* accès au stream, juste au début des données d'image */
struct bitstream * get_bitstream (const struct jpeg_desc *jpeg);

/* ferme la structure jpeg */
void close_jpeg (struct jpeg_desc *jpeg);

/* Donne le nom du fichier */
const char * get_filename (const struct jpeg_desc *jpeg);


// from DQT

/* Donne le nombre de table de quantification */
uint8_t get_nb_quantization_tables (const struct jpeg_desc *jpeg);

/* Donne la table de quantification d'indice index*/
uint32_t *get_quantization_table (const struct jpeg_desc *jpeg, uint8_t index);


// from DHT

/* Donne le nombre de tables de Huffman*/
uint8_t get_nb_huffman_tables (const struct jpeg_desc *jpeg, enum acdc type);

/* Donne la table de Huffman d'indice index*/
struct huff_table *get_huffman_table (const struct jpeg_desc *jpeg,
                                          enum acdc type, uint8_t index);

// from frame header SOF0

/* Donne la taille de l'image selon la direction horizontale ou verticale*/
uint16_t get_image_size (struct jpeg_desc *jpeg, enum direction dir);

/* Donne le nombre de composante de l'image */
uint8_t get_nb_components (const struct jpeg_desc *jpeg);

/* Donne l'indice de la composante défini dans le frame*/
uint8_t get_frame_component_id (const struct jpeg_desc *jpeg,
                                uint8_t frame_comp_index);

/* Donne le coefficient d'échantillonnage suivant la direction horizontale ou
verticale */
uint8_t get_frame_component_sampling_factor (const struct jpeg_desc *jpeg,
                                              enum direction dir,
                                              uint8_t frame_comp_index);

/* Donne l'indice de quantification pour la composante d'indice frame_comp_index*/
uint8_t get_frame_component_quant_index (const struct jpeg_desc *jpeg,
                                        uint8_t frame_comp_index);

// from scan header SOS

/* Donne l'indice de la composante d'indice scan_comp_index*/
uint8_t get_scan_component_id (const struct jpeg_desc *jpeg,
                                     uint8_t scan_comp_index);

/* Donne l'indice de la table de Huffman pour la composante d'indice scan_comp_index*/
uint8_t get_scan_component_huffman_index (const struct jpeg_desc *jpeg,
                                                enum acdc acdc,
                                                uint8_t scan_comp_index);
