#include "mac.h"

extern struct desc rd_desc[64];
extern struct desc sd_desc[64];

extern void rd_desc_init(void);
extern void sd_desc_init(void);
extern void dma_buffer_clear(void);

extern int dma_buffer_start_rd;
extern int dma_buffer_start_sd;