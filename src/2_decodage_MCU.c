#include "../include/2_decodage_MCU.h"

int decodage_Component(struct jpeg_desc *jdesc, struct bitstream *stream, uint8_t **idc, int predicateur,  uint8_t comp_index, int index_quantif){
      /*Initialisation de bloc*/
      int *bloc = malloc(sizeof(int)*64);
      init_vecteur64(bloc);
      int compteur = 0;

      /*Tables de Huffman*/

      /*DC*/
      uint8_t compo_huffman_index = get_scan_component_huffman_index (jdesc,0,comp_index);
      struct huff_table *table_DC = get_huffman_table (jdesc, 0, compo_huffman_index);
      int coef_magnitudeDC = next_huffman_value (table_DC, stream);
      /*on lit coef_magnitude bits*/
      uint32_t byteDC = 0;
      uint8_t nb_lu = read_bitstream (stream, coef_magnitudeDC, &byteDC, true);
      /*les bits lu sont la position dans la table de magnitude*/
      int magn = magnitude(coef_magnitudeDC, byteDC);
      /* mettre magn dans bloc */
      bloc[compteur] = magn + predicateur;
      predicateur = magn + predicateur;
      compteur += 1; /*permet de savoir à quel indice de bloc on est*/

      /* AC */
      /*On récupère la table*/

      uint8_t compo_huffman_index_AC = get_scan_component_huffman_index (jdesc,1,comp_index);
      struct huff_table *table_AC = get_huffman_table (jdesc, 1, compo_huffman_index_AC);
      /*on lit la prochaine valeur de Huffman tant que la composante n'est pas
      de taille 64*/
      while (compteur < 64){
        char *hex = calloc(4, sizeof(char));
        uint8_t val_AC = next_huffman_value (table_AC, stream);
        uint32_t byteAC = 0;
        dec_to_hex(val_AC,hex);
        //Traitement du cas EndOfBlock
        if (val_AC == 0){
          break;
        }
        int coef_nul = 0;
        /* mettre coef_nul * 0 dans bloc */
        coef_nul = char_to_int(&hex[1]);
        for (int i = 0; i < coef_nul; i++){
          bloc[compteur] = 0;
          compteur += 1;
        }
        /*on lit le coefficient de magnitude*/
        int classe_magnitude_AC = char_to_int(&hex[0]);
        if (classe_magnitude_AC != 0 ){
          /*on lit classe_magnitude bits*/
          nb_lu = read_bitstream (stream, classe_magnitude_AC, &byteAC, true);
          /*les bits lu sont la position dans la table de magnitude*/
          int valeur = magnitude(classe_magnitude_AC, byteAC);
          // printf("Valeur = %i \n", valeur);
          /* mettre magn dans bloc */
          bloc[compteur] = valeur;
          compteur += 1;

        }
        //Traitement du cas 0xF0
        else{
          if(char_to_int(&hex[1]) == 15){
            bloc[compteur] = 0;
            compteur += 1;
          }
        }
        free(hex);
      }

      /*Quantification inverse*/
      int *iquant = malloc(sizeof(int)*64);
      quantification_inverse(jdesc, bloc, iquant, index_quantif);
      // on n'a plus besoin de bloc
      free(bloc);

      /*Réorganisation zigzag*/
      int ** izz = malloc(sizeof(int*)*8);
      for (int i = 0; i < 8; i++){
        izz[i] = malloc(sizeof(int)*8);
      }
      zigzag(izz, iquant);
      // on n'a plus besoin de iquant
      free(iquant);

      /*Calcul de la transformée en cosinus discrète inverse (iDCT)*/
      idct_rapide(izz, idc);
      // on n'a plus besoin de izz
      for (int i = 0; i < 8; i++){
        free(izz[i]);
      }
      free(izz);

      /*on a besoin de la valeur du prédicateur pour le prochain DC*/
      return predicateur;
}

void duplicate_horizontal(uint8_t **idc, uint8_t **idc_1, uint8_t **idc_2){
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 8; j++){
      idc_1[i*2][j] = idc[i][j];
      idc_1[(i*2)+1][j] = idc[i][j];

      idc_2[i*2][j] = idc[i+4][j];
      idc_2[(i*2)+1][j] = idc[i+4][j];
    }
  }
}

void duplicate_vertical(uint8_t **idc, uint8_t **idc_1, uint8_t **idc_2){
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 4; j++){
      idc_1[i][j*2] = idc[i][j];
      idc_1[i][(j*2)+1] = idc[i][j];

      idc_2[i][j*2] = idc[i][j+4];
      idc_2[i][(j*2)+1] = idc[i][j+4];
    }
  }
}

void duplicate_horizontal_ET_vertical(uint8_t **idc, uint8_t **idc_1,
                                      uint8_t **idc_2, uint8_t **idc_3,
                                      uint8_t **idc_4){

  uint8_t **idc_temp_0 = allocation_matrice_64();
  uint8_t **idc_temp_1 = allocation_matrice_64();
  duplicate_horizontal(idc, idc_temp_0, idc_temp_1);
  duplicate_vertical(idc_temp_0, idc_1, idc_3);
  duplicate_vertical(idc_temp_1, idc_2, idc_4);
  free_matrice_64(idc_temp_0);
  free_matrice_64(idc_temp_1);
}

void remplir_Chro_Y22(uint8_t *** ligne_Cb, uint8_t *** ligne_Cb2, int index,
uint8_t **idc_Cb, uint8_t **idc_Cb2, uint8_t **idc_Cb3, uint8_t **idc_Cb4,
uint8_t val_echantillonnage_horiCb, uint8_t val_echantillonnage_vertiCb){
  /* Cb 2*2 */
  if (val_echantillonnage_horiCb == 2 && val_echantillonnage_vertiCb == 2){
    for(int i = 0; i < 8; i++){
      for(int j = 0; j < 8; j++){
        ligne_Cb[index][i][j] = idc_Cb[i][j];
        ligne_Cb[index + 1][i][j] = idc_Cb2[i][j];
        ligne_Cb2[index][i][j] = idc_Cb3[i][j];
        ligne_Cb2[index + 1][i][j] = idc_Cb4[i][j];
      }
    }
  }
  /* Cb 2*1 */
  else if (val_echantillonnage_horiCb == 2){
    //duplique que verti
      duplicate_vertical(idc_Cb, ligne_Cb[index], ligne_Cb2[index]);
      duplicate_vertical(idc_Cb2, ligne_Cb[index + 1], ligne_Cb2[index + 1]);
  }
  /* Cb 1*2 */
  else if (val_echantillonnage_vertiCb == 2){
    //duplique que hori
      duplicate_horizontal(idc_Cb, ligne_Cb[index], ligne_Cb[index + 1]);
      duplicate_horizontal(idc_Cb2, ligne_Cb2[index], ligne_Cb2[index + 1]);
  }
  /* Cb 1*1 */
  else{
    duplicate_horizontal_ET_vertical(idc_Cb, ligne_Cb[index],
       ligne_Cb[index+1], ligne_Cb2[index], ligne_Cb2[index+1]);
  }
}

void remplir_Chro_Y41(uint8_t *** ligne_Cb, int index,
  uint8_t **idc_Cb, uint8_t **idc_Cb2, uint8_t **idc_Cb3, uint8_t **idc_Cb4,
  uint8_t val_echantillonnage_horiCb){
  /* cas Cb 2*1 */
  if(val_echantillonnage_horiCb == 2){
    duplicate_horizontal(idc_Cb, ligne_Cb[index], ligne_Cb[index+1]);
    duplicate_horizontal(idc_Cb2, ligne_Cb[index+2], ligne_Cb[index+3]);
  }
  /* cas Cb 4*1 */
  else{
    for(int i = 0; i < 8; i++){
      for(int j = 0; j < 8; j++){
        ligne_Cb[index][i][j] = idc_Cb[i][j];
        ligne_Cb[index + 1][i][j] = idc_Cb2[i][j];
        ligne_Cb[index + 2][i][j] = idc_Cb3[i][j];
        ligne_Cb[index + 3][i][j] = idc_Cb4[i][j];
      }
    }
  }
}

void remplir_Chro_Y14(uint8_t *** ligne_Cb, uint8_t *** ligne_Cb2,
  uint8_t *** ligne_Cb3, uint8_t *** ligne_Cb4, int index,
  uint8_t **idc_Cb, uint8_t **idc_Cb2, uint8_t **idc_Cb3, uint8_t **idc_Cb4,
  uint8_t val_echantillonnage_vertiCb){
  /* cas Cb 2*1 */
  if(val_echantillonnage_vertiCb == 2){
    duplicate_vertical(idc_Cb, ligne_Cb[index], ligne_Cb2[index]);
    duplicate_vertical(idc_Cb2, ligne_Cb3[index], ligne_Cb4[index]);
  }
  /* cas Cb 4*1 */
  else{
    for(int i = 0; i < 8; i++){
      for(int j = 0; j < 8; j++){
        ligne_Cb[index][i][j] = idc_Cb[i][j];
        ligne_Cb2[index][i][j] = idc_Cb2[i][j];
        ligne_Cb3[index][i][j] = idc_Cb3[i][j];
        ligne_Cb4[index][i][j] = idc_Cb4[i][j];
      }
    }
  }
}
