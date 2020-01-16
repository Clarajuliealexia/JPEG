#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../include/outils.h"

/*(c) Reconstruction des pixels : conversion YCbCr vers RGB */

/* Conversion simple : Conversion de YCbCr en RGB */
float *conversion_simple(uint8_t y, uint8_t cb, uint8_t cr);

/*ConversionRGB : Saturation du vecteur obtenue */
uint8_t *conversionRGB(uint8_t y, uint8_t cb, uint8_t cr);
