#ifndef __OUTILSREADER__H__
#define __OUTILSREADER_H__

struct jpeg_desc;

extern void init(struct jpeg_desc *jdesc);
extern void SOI(struct bitstream * stream, struct jpeg_desc *jdesc);
extern void app0(struct bitstream * stream, struct jpeg_desc *jdesc);
extern void dqt(struct bitstream * stream, struct jpeg_desc *jdesc);
extern void sof0(struct bitstream * stream, struct jpeg_desc *jdesc);
extern void dht(struct bitstream * stream, struct jpeg_desc *jdesc);
extern void sos(struct bitstream * stream, struct jpeg_desc *jdesc);

#endif
