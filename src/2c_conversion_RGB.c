#include "../include/2c_conversion_RGB.h"
/*(c) Reconstruction des pixels : conversion YCbCr vers RGB */

  /* Conversion simple : Conversion de YCbCr en RGB */
float *conversion_simple(uint8_t y, uint8_t cb, uint8_t cr){
  float *pixel = calloc(3,sizeof(float));
  pixel[0] = y + 1.402*(cr - 128);
  pixel[1] = y - 0.34414*(cb - 128) - 0.71414*(cr - 128);
  pixel[2] = y + 1.772*(cb - 128);
  return pixel;
}
  /*ConversionRGB : Saturation du vecteur obtenue */
uint8_t *conversionRGB(uint8_t y, uint8_t cb, uint8_t cr){
  float *pixel = conversion_simple(y, cb, cr);
  uint8_t *pix = calloc(3,sizeof(uint8_t));
  for (int i = 0; i < 3; i++){
    pix[i] = saturation(pixel[i]);
  }
  free(pixel);
  return pix;
}
