struct jpeg_desc{
  /*SOI*/
  bool marker_JPEG;

  /*APP0*/
  int taille_app0;
  int ignored_app0;

  /*DQT*/
  int nombre_table_quantification;
  int taille_dqt;
  int ***table_quantification;
  int *precision_dqt;

  /*SOF0*/
  int taille_sof0;
  int precision_sof0;
  uint16_t hauteur;
  uint16_t largeur;
  int nombre_composante;
  int **info_composante;

  /*DHT*/
  /*DC*/
  int taille_dht_dc;
  int index_table_dc;
  int nombre_code_huffman_dc;
  /*AC*/
  int taille_dht_ac;
  int index_table_ac;
  int nombre_code_huffman_ac;


  char* nom_fichier;
}



/*read_jpeg ouvre le fichier JPEG, lit tous les entêtes de sections rencontrés et stocke les in-
formations requises dans le descripteur retourné. La lecture est stoppée au début des données
brutes codant l’image (après l’entête de la section SOS) ;*/
struct jpeg_desc * read_jpeg (const char * filename){
  struct bitstream * stream;
  stream = create_bitstream (filename);
  /*allocation dynamique de jdesc*/
  struct jpeg_desc *jdesc = malloc(sizeof(struct jpeg_desc));
  jdesc->nom_fichier = filename;

  /*On vérifie que le fichier est bien un fichier JPEG*/
  int marker;
  read_bitstream(stream, 16, &marker, false);
  if (marker != 0xffd8){
    printf("Erreur, le fichier n'est pas un fichier JPEG \n");
    jdesc->marker_JPEG = false;
    return NULL;
  }
  jpeg_desc->marker_JPEG = true;

  /*Bloc APP0*/
  read_bitstream(stream, 16, &marker, false);
  if (marker != 0xffe0){
    printf("Erreur APP0 \n");
    return;
  }
  int jdesc.taille_app0;
  read_bitstream(stream, 16, &jdesc.taille_app0, false);
  // nombre d'octets ignorés
  int jdesc.ignored_app0 = jdesc.taille - 7;
  int inutile;
  /*on passe le reste du bloc*/
  read_bitstream(stream, jdesc.ignored_app0 - 7, &inutile, false);


  /*Bloc DQT*/
  read_bitstream(stream, 16, &marker, false);
  if (marker != 0xffdb){
    printf("Erreur DQT \n");
    return;
  }
  /*longueur du bloc*/
  int jdesc.taille_dqt;
  read_bitstream(stream, 16, &jdesc.taille_dqt, false);
  /*calcul du nombre de tables de quantification*/
  int nombre_table_quantification = (jdesc.taille_dqt - 2) / (8 + 64);
  int jdesc->nombre_table_quantification = nombre_table_quantification;
  /*on récupère les tables de quantification et les précisions*/
  int **table_quantification = allocation_matrice(nombre_table_quantification, 64);
  int ***jdesc->table_quantification_zigzag = allocation_matrice3d(nombre_table_quantification, 8, 8);
  int *jdesc->precision_dqt = malloc(sizeof(int)*nombre_table_quantification);
  for (int i = 0; i < nombre_table_quantification; i++){
    /*on lit la précision*/
    int precison;
    read_bitstream(stream, 4, &precision, false);
    /*on lit l'indice de la table*/
    int indice;
    read_bitstream(stream, 4, &indice, false);
    jdesc->precision_dqt[indice] =  precision;
    for (int j = 0; j < 64; j++){
      /*on remplit les matrices de quantification avec les 64 valeurs*/
      read_bitstream(stream, 8, &table_quantification[indice][j], false);
    }
    /*on met la table sous forme de matrice en utilisant zigzag inverse*/
    zigzag(table_quantification[indice], jdesc->table_quantification[indice]);
  }

  /*Bloc SOF0*/
  read_bitstream(stream, 16, &marker, false);
  if (marker != 0xffc0){
    printf("Erreur SOF0 \n");
    return;
  }
  /*longueur du bloc*/
  int jdesc.taille_sof0;
  read_bitstream(stream, 16, &jdesc.taille_sof0, false);
  int jdesc.precision_sof0;
  /*précision en bits par composante*/
  read_bitstream(stream, 8, &jdesc.precision_sof0, false);
  /*hauteur*/
  int jdesc.hauteur;
  read_bitstream(stream, 16, &jdesc.hauteur, false);
  /*largeur*/
  int jdesc.largeur;
  read_bitstream(stream, 16, &jdesc.largeur, false);
  /*nombre de composantes*/
  int jdesc.nombre_composante;
  read_bitstream(stream, 8, &jdesc.nombre_composante, false);
  int **jdesc->info_composante = allocation_matrice(jdesc.nombre_composante, 4);
  for (int i = 0; i < jdesc.nombre_composante; i++){
    /*identifiant de composante*/
    read_bitstream(stream, 8, &jdesc->info_composante[i][1], false);
    /*facteur d'échantillonnage horizonal*/
    read_bitstream(stream, 4, &jdesc->info_composante[i][2], false);
    /*facteur d'échantillonnage vertical*/
    read_bitstream(stream, 4, &jdesc->info_composante[i][3], false);
    /*table de quantification associée*/
    read_bitstream(stream, 8, &jdesc->info_composante[i][4], false);
  }

  /*Bloc DHT*/
  read_bitstream(stream, 16, &marker, false);
  if (marker != 0xffc4){
    printf("Erreur DHT \n");
    return;
  }
  /*longueur de la section*/
  int jdesc.taille_dht;
  read_bitstream(stream, 18, &jdesc.taille_dht, false);
  /*DC*/
  /*information sur la table de huffman*/



  /*AC*/





}
void close_jpeg ( struct jpeg_desc *jpeg);
struct bitstream * get_bitstream ( const struct jpeg_desc *jpeg);
char * get_filename ( const struct jpeg_desc *jpeg);




uint16_t get_image_size (struct jpeg_desc *jpeg, enum direction dir){
  if (dir == DIR_H){
    return jpeg.hauteur;
  }
  else {
    return jpeg.largeur;
  }
}


uint8_t get_nb_components (const struct jpeg_desc *jpeg){
  return jpeg.nombre_composante;
}


uint8_t get_frame_component_id (const struct jpeg_desc *jpeg,
                                uint8_t frame_comp_index );


uint8_t get_frame_component_sampling_factor (const struct jpeg_desc *jpeg,
                                              enum direction dir,
                                              uint8_t frame_comp_index) {
  if (dir == DIR_H){
    return jpeg->info_composante[frame_comp_index][3];
  }
  else {
    return jpeg->info_composante[frame_comp_index][2];
  }
}


uint8_t get_frame_component_quant_index(const struct jpeg_desc *jpeg,
                                        uint8_t frame_comp_index) {
}
