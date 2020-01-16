#include "2a_reconstruction_bloc.h"
#include "2c_conversion_RGB.h"

int decodage_Component_progressif_DC(struct jpeg_desc *jdesc, struct bitstream *stream, int *mem_bloc,
                        uint8_t **idc, int predicateur,  uint8_t comp_index,
                        int index_quantif, uint8_t al);

void decodage_Component_progressif_AC(struct jpeg_desc *jdesc, struct bitstream *stream,
                                    int *bloc ,uint8_t **idc, uint8_t comp_index,
                                    int index_quantif, uint8_t ss, uint8_t se, uint8_t al);
