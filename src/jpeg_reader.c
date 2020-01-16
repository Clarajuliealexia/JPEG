#include "../include/jpeg_reader.h"

/*read_jpeg ouvre le fichier JPEG, lit toutes les entêtes de sections rencontrées et stocke les in-
formations requises dans le descripteur retourné
La lecture est stoppée au début des données
brutes codant l’image (après l’entête de la section SOS)*/

void parse_appo(struct bitstream *stream){
  uint32_t taille_app0;
  read_bitstream (stream, 16, &taille_app0, false);
  printf("length %u bytes \n", taille_app0);
  /*On lit JFIF*/
  uint32_t jfif;
  read_bitstream (stream, 32, &jfif, false);
  read_bitstream (stream, 8, &jfif, false);
  printf("\t JFIF application \n");
  // nombre d'octets ignorés
  uint32_t ignored = taille_app0 - 7;
  printf("\t other parameters ignoreg (%u bytes)\n", ignored);
  uint32_t inutile;
  /*on passe le reste du bloc*/
  for (int i = 0; i < 8; i++){
    read_bitstream (stream, ignored, &inutile, false);
  }
}

void parse_dqt(struct jpeg_desc *jdesc, struct bitstream *stream, int compteur){
    jdesc->nombre_table_quantification += 1;
    /*longueur du bloc*/
    uint32_t taille_dqt;
    read_bitstream (stream, 16, &taille_dqt, false);
    printf("length %u bytes \n", taille_dqt);
    /*on lit la précision*/
    uint32_t precision;
    read_bitstream (stream, 4, &precision, false);
    /*on lit l'indice de la table*/
    uint32_t index;
    read_bitstream (stream, 4, &index, false);
    printf("\t \t quantization table index %u \n", index);
    if (precision == 0){
      precision = 8;
    }
    else {
      precision = 16;
    }
    printf("\t \t quantization precision %u bits \n", precision);
    /*on remplit les matrices de quantification avec les 64 valeurs*/
    for (int j = 0; j < 64; j++){
      read_bitstream (stream, 8, &(jdesc->table_quantification[compteur][j]), false);
    }
    printf("\t \t quantization table read (64 bytes)\n");
}


void parse_sof0(struct jpeg_desc *jdesc, struct bitstream *stream){
  /*longueur du bloc*/
  uint32_t taille_sof0;
  read_bitstream (stream, 16, &taille_sof0, false);
  printf("length %u bytes \n", taille_sof0);
  uint32_t precision_sof0;
  /*précision en bits par composante*/
  read_bitstream (stream, 8, &precision_sof0, false);
  printf("\t sample precision %u \n", precision_sof0);
  /*hauteur*/
  uint32_t hauteur;
  read_bitstream (stream, 16, &hauteur, false);
  jdesc->hauteur = hauteur;
  printf("\t image height %u \n", jdesc->hauteur);
  /*largeur*/
  uint32_t largeur;
  read_bitstream (stream, 16, &largeur, false);
  jdesc->largeur = largeur;
  printf("\t image weight %u \n", jdesc->largeur);
  /*nombre de composantes*/
  uint32_t nombre_composante;
  read_bitstream (stream, 8, &nombre_composante, false);
  jdesc->nombre_composante = nombre_composante;
  printf("\t nombre of component %u \n", jdesc->nombre_composante);
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
    read_bitstream (stream, 8, &jdesc->info_composante[i][0], false);
    printf("\t \t id %u \n", jdesc->info_composante[i][0]);
    /*facteur d'échantillonnage horizonal*/
    read_bitstream (stream, 4, &jdesc->info_composante[i][1], false);
    /*facteur d'échantillonnage vertical*/
    read_bitstream (stream, 4, &jdesc->info_composante[i][2], false);
    printf("\t \t sampling factors (hxv) %ux%u \n", jdesc->info_composante[i][1], jdesc->info_composante[i][2]);
    /*table de quantification associée*/
    read_bitstream (stream, 8, &jdesc->info_composante[i][3], false);
    printf("\t \t quantization table index %i\n", jdesc->info_composante[i][3]);
  }
}

void parse_dht(struct jpeg_desc *jdesc, struct bitstream *stream, int compteur){
  /*longueur de la section*/
  uint32_t taille_dht;
  read_bitstream (stream, 16, &taille_dht , false);
  printf("length %u bytes \n", taille_dht);
  uint32_t zero = 0;
  read_bitstream (stream, 3, &zero, false);
  if (zero != 0){
    printf("Erreur DHT \n");
    return;
  }
  uint32_t type = 0;
  read_bitstream (stream, 1, &type, false);
  if (type == 0){
    printf("\t \t Huffman table type DC\n");
  }
  else {
    printf("\t \t Huffman table type AC\n");
  }
  uint32_t index;
  read_bitstream (stream, 4, &index, false);
  printf("\t \t Huffman table index %u \n", index);
  uint16_t lu = 0;
  (jdesc->table_huff)[compteur] = load_huffman_table (stream, &lu);
  printf("\t \t total nb of Huffman codes %u \n", jdesc->table_huff[compteur]->nb_code);
}

void parse_sos(struct jpeg_desc *jdesc, struct bitstream *stream){
  uint32_t taille_sos;
  read_bitstream (stream, 16, &taille_sos, false);
  printf("lenght %u bytes \n", taille_sos);
  uint32_t nb_compo;
  read_bitstream (stream, 8, &nb_compo, false);
  jdesc->nb_compo = nb_compo;
  printf("\t nb of components in scan %u \n", nb_compo);
  jdesc->info_compo = allocation_matrice(nb_compo, 3);
  for (uint32_t i = 0; i < nb_compo; i++){
    printf("\t scan component index %u \n", i);
    read_bitstream (stream, 8, &jdesc->info_compo[i][0], false);
    printf("\t \t associated to component of id %u \n", jdesc->info_compo[i][0]);
    read_bitstream (stream, 4, &jdesc->info_compo[i][1], false);
    printf("\t \t associated to DC Huffman table of index %u \n", jdesc->info_compo[i][1]);
    read_bitstream (stream, 4, &jdesc->info_compo[i][2], false);
    printf("\t \t associated to AC Huffman table of index %u \n", jdesc->info_compo[i][2]);
  }
  uint32_t ss;
  read_bitstream (stream, 8, &ss, false);
  jdesc->ss = (uint8_t)ss;
  printf("\t ss = %x \n", ss);
  uint32_t se;
  read_bitstream (stream, 8, &se, false);
  jdesc->se = (uint8_t)se;
  printf("\t se = %x \n", se);
  uint32_t ah;
  read_bitstream (stream, 4, &ah, false);
  jdesc->ah = (uint8_t)ah;
  printf("\t ah = %x \n", ah);
  uint32_t al;
  read_bitstream (stream, 4, &al, false);
  jdesc->al = (uint8_t)al;
  printf("\t al = %x \n", al);
  jdesc->ignoreg_sos = 3;
  printf("\t End of Scan Header (SOS)\n");
}


struct jpeg_desc * read_jpeg (const char * filename){
  struct bitstream * stream;
  stream = create_bitstream (filename);
  /*allocation dynamique de jdesc*/
  struct jpeg_desc *jdesc;
  jdesc = malloc(sizeof(struct jpeg_desc));
  jdesc->nom_fichier = filename;

  /*initialisation*/
  jdesc->table_quantification = allocation_matrice(2, 64);
  jdesc->table_huff = malloc(sizeof(struct huff_table)*4);
  /* On initialise les tables de Huffman pour les couleurs*/
  jdesc->table_huff[2] = NULL;
  jdesc->table_huff[3] = NULL;

  /*SOI*/
  printf("[SOI]\t ");
  /*On vérifie que le fichier est bien un fichier JPEG*/
  uint32_t marker;
  read_bitstream (stream, 16, &marker, false);
  if (marker != 0xffd8){
    printf("Erreur, le fichier n'est pas un fichier JPEG \n");
    return NULL;
  }
  printf("marker found \n");

  /*Bloc APP0*/
  printf("[APP0]\t ");
  read_bitstream (stream, 16, &marker, false);
  if (marker != 0xffe0){
    printf("Erreur APP0 \n");
    return NULL;
  }
  parse_appo(stream); 

  /*Bloc DQT*/
  read_bitstream (stream, 16, &marker, false);
  int compteur = 0;
  while (marker == 0xffdb){
    printf("[DQT]\t ");
    parse_dqt(jdesc, stream, compteur);
    compteur += 1;
    /*on lit le marqueur suivant*/
    read_bitstream (stream, 16, &marker, false);
  }
  /* On regarde si on est progressif ou pas*/
  if (marker == 0xffc2){
    printf("[SOF2]\t ");
  }
  else if (marker == 0xffc0){
    printf("[SOF0]\t ");
  }
  parse_sof0(jdesc, stream);

  /*Bloc DHT*/
  read_bitstream (stream, 16, &marker, false);
  compteur = 0;
  while (marker == 0xffc4){
    printf("[DHT]\t ");
    parse_dht(jdesc, stream, compteur);
    /*on incrémente le compteur*/
    compteur += 1;
    /*on lit le marqueur suivant*/
    read_bitstream (stream, 16, &marker, false);
  }

  /*Bloc SOS*/
  /*Une seule fois*/
  printf("[SOS] \t ");
  if (marker != 0xffda){
    printf("Erreur SOS \n");
    return NULL;
  }
  parse_sos(jdesc, stream);

  jdesc->stream = stream;
  return jdesc;
}

// accès au stream, juste au début des données d'image
struct bitstream * get_bitstream (const struct jpeg_desc *jpeg){
  return jpeg->stream;
}

void close_jpeg (struct jpeg_desc *jpeg){
  /*on vérifie qu'on est à la fin de l'image*/
  /*on récupère le stream*/
  struct bitstream *stream;
  stream = get_bitstream (jpeg);
  uint32_t restant;
  read_bitstream (stream, stream->nb_restant, &restant, false);
  printf("[EOI] \t");
  printf("marker found \n");
  printf("bitstream empty \n");
  /*on free tout ce qu'on a alloué dynamiquement*/
  free_matrice(jpeg->table_quantification, 2);
  free_matrice(jpeg->info_composante, jpeg->nombre_composante);
  if (jpeg->nb_compo == 1){
    for (int i = 0; i < 2; i++){
      free_huffman_table (jpeg->table_huff[i]);
    }
  }
  else {
    for (int i = 0; i < 4; i++){
      free_huffman_table (jpeg->table_huff[i]);
    }
  }
  free(jpeg->table_huff);
  /*on ferme le stream*/
  close_bitstream (stream);
  /*on free jpeg*/
  free(jpeg);
}

const char * get_filename (const struct jpeg_desc *jpeg){
  return jpeg->nom_fichier;
}


// from DQT
uint8_t get_nb_quantization_tables (const struct jpeg_desc *jpeg){
  return jpeg->nombre_table_quantification;
}

uint32_t *get_quantization_table (const struct jpeg_desc *jpeg, uint8_t index){
  return jpeg->table_quantification[index];
}


// from DHT
uint8_t get_nb_huffman_tables (const struct jpeg_desc *jpeg, enum acdc type){
  if (type == 0){
    return jpeg->table_huff[0]->nb_code + jpeg->table_huff[2]->nb_code;
  }
  else {
    return jpeg->table_huff[1]->nb_code + jpeg->table_huff[3]->nb_code;
  }
}

struct huff_table *get_huffman_table (const struct jpeg_desc *jpeg,
                                          enum acdc type, uint8_t index){
  if (type == 0){
    if (index == 0){
      return (jpeg->table_huff)[0];
    }
    else {
      return (jpeg->table_huff)[2];
    }
  }
  else {
    if (index == 0){
      return (jpeg->table_huff)[1];
    }
    else {
      return (jpeg->table_huff)[3];
    }
  }
}

// from frame header SOF0
uint16_t get_image_size (struct jpeg_desc *jpeg, enum direction dir){
  if (dir == DIR_V){
    return jpeg->hauteur;
  }
  else {
    return jpeg->largeur;
  }
}

uint8_t get_nb_components (const struct jpeg_desc *jpeg){
  return jpeg->nombre_composante;
}

uint8_t get_frame_component_id (const struct jpeg_desc *jpeg,
                                uint8_t frame_comp_index) {
  return jpeg->info_composante[frame_comp_index][0];
}

uint8_t get_frame_component_sampling_factor (const struct jpeg_desc *jpeg,
                                              enum direction dir,
                                              uint8_t frame_comp_index) {
  if (dir == DIR_V){
    return jpeg->info_composante[frame_comp_index][2];
  }
  else {
    return jpeg->info_composante[frame_comp_index][1];
  }
}

uint8_t get_frame_component_quant_index (const struct jpeg_desc *jpeg,
                                        uint8_t frame_comp_index){
  return jpeg->info_composante[frame_comp_index][3];
}

// from scan header SOS
uint8_t get_scan_component_id (const struct jpeg_desc *jpeg,
                                     uint8_t scan_comp_index){
  return jpeg->info_compo[scan_comp_index][0];
}

uint8_t get_scan_component_huffman_index (const struct jpeg_desc *jpeg,
                                                enum acdc acdc,
                                                uint8_t scan_comp_index){
  return jpeg->info_compo[scan_comp_index][1+acdc];
}
