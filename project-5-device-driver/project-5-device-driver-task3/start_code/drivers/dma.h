#include "mac.h"

extern struct desc rd_desc[64];
extern struct desc sd_desc[64];

void rd_desc_init(void);
void sd_desc_init(void);
void dma_buffer_clear(void);

extern int dma_buffer_start_rd;
extern int dma_buffer_start_sd;

extern queue_t mac_wait;

void check_mac(void);