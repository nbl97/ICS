#include "common.h"
#include <stdlib.h>
#include "memory/cache.h"

int is_mmio(hwaddr_t);
uint32_t mmio_read(hwaddr_t, size_t, int);
void mmio_write(hwaddr_t, size_t, uint32_t, int);
int cache1_read(hwaddr_t);
void cache1_write(hwaddr_t, size_t, uint32_t);

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);


/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	int index = is_mmio(addr);
	if(index >= 0) {
		return mmio_read(addr, len, index);
	}
	uint32_t offset = addr & (BLOCK_SIZE - 1);
	uint32_t block = cache1_read(addr);
	uint8_t temp[4];
	memset(temp, 0, sizeof(temp));
	if(offset + len >= BLOCK_SIZE) {
		uint32_t block1 = cache1_read(addr + len);
		memcpy(temp, cache1[block].data + offset, BLOCK_SIZE - offset);
		memcpy(temp + BLOCK_SIZE - offset, cache1[block1].data, len - (BLOCK_SIZE - offset));
	}
	else {
		memcpy(temp, cache1[block].data + offset, len);
	}
	int zero = 0;
	uint32_t t = unalign_rw(temp + zero, 4) & (~0u >> ((4 - len) << 3));
	
	return t;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	int index = is_mmio(addr);
	if(index >= 0) {
		mmio_write(addr, len, data, index);
		return ;
	}
	cache1_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

