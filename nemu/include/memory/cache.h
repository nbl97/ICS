#ifndef __CACHE_H__
#define __CACHE_H__

#define BLOCK_SIZE 64
#define L1_SIZE 64*1024
#define L2_SIZE 4*1024*1024

typedef struct L1Cache{
	bool valid;
	uint32_t tag;
	uint8_t data[BLOCK_SIZE];
}L1Cache;

typedef struct L2Cache{
	bool valid, dirty;
	uint32_t tag;
	uint8_t data[BLOCK_SIZE];
}L2Cache;

L1Cache cache1[L1_SIZE/BLOCK_SIZE];

L2Cache cache2[L2_SIZE/BLOCK_SIZE];

#endif
