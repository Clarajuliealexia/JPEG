#include "2a_reconstruction_bloc.h"
#include "2c_conversion_RGB.h"

/* Cette fonction permet de décoder les MCU
On récupère en premier les valeur de huffman qu'on met dans un vecteur bloc
On effectue ensuite une quantification inverse
On fait une réorganisation zig-zag et on effectue la transformation en cosinus
discrète inverse
On retourne la valeur du prédicateur qui nous servira pour le prochain bloc*/
int decodage_Component(struct jpeg_desc *jdesc, struct bitstream *stream,
                        uint8_t **idc, int predicateur,  uint8_t comp_index,
                        int index_quantif);

/* duplicate_vertical : prend en entrée trois matrices, la première est la
   matrice idc initiale qui sera ensuite dupliquée en deux sous matrices
   verticalement */
void duplicate_vertical(uint8_t **idc, uint8_t **idc_1, uint8_t **idc_2);

/* duplicate_horizontal : prend en entrée trois matrices, la première est la
   matrice idc initiale qui sera ensuite dupliquée en deux sous matrices
   horizontalement */
void duplicate_horizontal(uint8_t **idc, uint8_t **idc_1, uint8_t **idc_2);

/* duplicate_vertical_ET_vertical : prend en entrée cinq matrices, la première
   est la matrice idc initiale qui sera ensuite dupliquée en quatres sous
   matrices verticalement et horizontalement */
void duplicate_horizontal_ET_vertical(uint8_t **idc, uint8_t **idc_1,
                                      uint8_t **idc_2, uint8_t **idc_3,
                                      uint8_t **idc_4);

/* Appelé dans le cas ou Y a comme valeur d'échantillonnage 2*2
   rempli les lignes Cb ou Cr en fonction de leur valeur d'échantillonnage
   horizontale et verticale, soit utilise duplicate_horizontal_ET_vertical cas
   1*1, soit utilise duplicate_horizontal cas 1*2, soit utilise
   duplicate_vertical dans le cas 2*1 ou copie les 4 idc_Cbi directement */
void remplir_Chro_Y22(uint8_t *** ligne_Cb, uint8_t *** ligne_Cb2, int index,
                      uint8_t **idc_Cb, uint8_t **idc_Cb2, uint8_t **idc_Cb3,
                      uint8_t **idc_Cb4, uint8_t val_echantillonnage_horiCb,
                      uint8_t val_echantillonnage_vertiCb);

/* Appelé dans le cas ou Y a comme valeur d'échantillonnage 4*1
   rempli les lignes Cb ou Cr en fonction de leur valeur d'échantillonnage
   horizontale et verticale, soit utilise duplicate_horizontal cas 2*1, soit
   copie les 2 idc_Cbi directement */
void remplir_Chro_Y41(uint8_t *** ligne_Cb, int index, uint8_t **idc_Cb,
                      uint8_t **idc_Cb2, uint8_t **idc_Cb3, uint8_t **idc_Cb4,
                      uint8_t val_echantillonnage_horiCb);

/* Appelé dans le cas ou Y a comme valeur d'échantillonnage 1*4
   rempli les lignes Cb ou Cr en fonction de leur valeur d'échantillonnage
   horizontale et verticale, soit utilise duplicate_vertical cas 1*2, soit
   copie les 2 idc_Cbi directement */
void remplir_Chro_Y14(uint8_t *** ligne_Cb, uint8_t *** ligne_Cb2,
                      uint8_t *** ligne_Cb3, uint8_t *** ligne_Cb4, int index,
                      uint8_t **idc_Cb, uint8_t **idc_Cb2, uint8_t **idc_Cb3,
                      uint8_t **idc_Cb4, uint8_t val_echantillonnage_vertiCb);
