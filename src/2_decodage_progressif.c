#include "../include/2_decodage_progressif.h"

int decodage_Component_progressif_DC(struct jpeg_desc *jdesc, struct bitstream *stream,
                                    int *bloc ,uint8_t **idc, int predicateur,
                                    uint8_t comp_index, int index_quantif, uint8_t al){

      /*Tables de Huffman*/
      /*DC*/
      uint8_t compo_huffman_index = get_scan_component_huffman_index (jdesc,0,comp_index);
      struct huff_table *table_DC = get_huffman_table (jdesc, 0, compo_huffman_index);
      //print_table(table_DC);
      int coef_magnitudeDC = next_huffman_value (table_DC, stream);
      /*on lit coef_magnitude bits*/
      uint32_t byteDC = 0;
      uint8_t nb_lu = 0;
      nb_lu = read_bitstream (stream, coef_magnitudeDC, &byteDC, true);
      /*les bits lu sont la position dans la table de magnitude*/
      int magn = magnitude(coef_magnitudeDC, byteDC);
      /* mettre magn dans bloc */

      bloc[0] = (magn + predicateur) << al;
      predicateur = magn + predicateur;
      for (int i = 1; i < 63; i++){
        bloc[i] = 0;
      }

      /*Quantification inverse*/
      int *iquant = malloc(sizeof(int)*64);
      quantification_inverse(jdesc, bloc, iquant, index_quantif);

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

      return predicateur;
}


void decodage_Component_progressif_AC(struct jpeg_desc *jdesc, struct bitstream *stream,
                                    int *bloc ,uint8_t **idc, uint8_t comp_index,
                                    int index_quantif, uint8_t ss, uint8_t se, uint8_t al){
      int compteur = ss;
      /*Tables de Huffman*/
      /*On récupère la table*/

      uint8_t compo_huffman_index_AC = get_scan_component_huffman_index (jdesc,1,comp_index);
      struct huff_table *table_AC = get_huffman_table (jdesc, 1, compo_huffman_index_AC);
      /*on lit la prochaine valeur de Huffman tant que la composante n'est pas
      de taille 64*/
      while (compteur < se){
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
          bloc[compteur] += 0;
          compteur += 1;
        }
        /*on lit le coefficient de magnitude*/
        int classe_magnitude_AC = char_to_int(&hex[0]);
        if (classe_magnitude_AC != 0 ){
          /*on lit classe_magnitude bits*/
          int nb_lu = 0;
          nb_lu = read_bitstream (stream, classe_magnitude_AC, &byteAC, true);
          /*les bits lu sont la position dans la table de magnitude*/
          int valeur = magnitude(classe_magnitude_AC, byteAC);
          /* mettre magn dans bloc */
          bloc[compteur] += valeur << al;
          compteur += 1;
        }
        //Traitement du cas 0xF0
        else{
          if(char_to_int(&hex[1]) == 15){
            bloc[compteur] += 0;
            compteur += 1;
          }
        }
        /*On libère hex et test*/
        free(hex);
      }

      /*Quantification inverse*/
      int *iquant = malloc(sizeof(int)*64);
      quantification_inverse(jdesc, bloc, iquant, index_quantif);

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
}
