#include "common.h"
#include <stdlib.h>
#include "burst.h"
#include "memory/cache.h"
#include "cpu/reg.h"

void ddr3_read(hwaddr_t, void*);
void ddr3_write(hwaddr_t, void*, uint8_t*);

void init_cache() {
	int i;
	for(i = 0; i < L1_SIZE/BLOCK_SIZE; i++) {
		cache1[i].valid = false;
	}
	for(i = 0; i < L2_SIZE/BLOCK_SIZE; i++) {
		cache2[i].valid = false;
	}
}


int cache2_read(hwaddr_t addr) {
	int i;
	bool v = false;
	uint32_t group_nr = (addr >> 6) & 0xfff;
	uint32_t b = (addr >> 6) << 6;
	for(i = 16 * group_nr; i < 16 * (group_nr + 1); i++) {
		if(((addr >> 18) == cache2[i].tag) && (cache2[i].valid == true)) {
			v = true;
			break;
		}
	}
	if(v != true) {
		int j;
		for(i = 16 * group_nr; i < 16 * (group_nr + 1); i++) {
			if(cache2[i].valid != true) {
				break;
			}
		}
		if(i == 16 * (group_nr + 1)) {
			srand(0);
			i = 16 * group_nr + rand() % 16;
			if(cache2[i].dirty) {
				uint8_t mask[BURST_LEN * 2];
				memset(mask, 1, BURST_LEN * 2);
				for(j = 0; j < BLOCK_SIZE/BURST_LEN; j++) {
					ddr3_write(b + j * BURST_LEN, cache2[i].data + j * BURST_LEN, mask);
				}
			}
		}
		cache2[i].valid = true;
		cache2[i].tag = addr >> 18;
		cache2[i].dirty = false;
		for(j = 0; j < BURST_LEN; j++) {
			ddr3_read(b + j * BURST_LEN, cache2[i].data + j * BURST_LEN);
		}
	}
	return i;
}

int cache1_read(hwaddr_t addr) {
	int i;
	bool v = false;
	uint32_t group_nr = (addr >> 6) & 0x7f;
	for(i = 8 * group_nr; i < 8 * (group_nr + 1); i++) {
		if(((addr >> 13) == cache1[i].tag) && (cache1[i].valid == true)) {
			v = true;
			break;
		}
	}
	if(v != true) {
		int m = cache2_read(addr);
		for(i = 8 * group_nr; i < 8 * (group_nr + 1); i++) {
			if(cache1[i].valid != true) {
				break;
			}
		}
		if(i == 8 * (group_nr + 1)) {
			srand(0);
			i = 8 * group_nr + rand() % 8;
		}
		cache1[i].valid = true;
		cache1[i].tag = addr >> 13;
		memcpy(cache1[i].data, cache2[m].data, BLOCK_SIZE);
	}
	return i;
}

void cache2_write(hwaddr_t addr, size_t len, uint32_t data) {
	int i;
	bool v = false;
	uint32_t group_nr = (addr >> 6) & 0xfff;
	uint32_t offset = addr & (BLOCK_SIZE - 1);
	for(i = 16 * group_nr; i < 16 * (group_nr + 1); i++) {
		if(((addr >> 18) == cache2[i].tag) && (cache2[i].valid == true)) {
			v = true;
			break;
		}
	}
	if(v != true) {
		i = cache2_read(addr);
	}
	cache2[i].dirty = true;
	memcpy(cache2[i].data + offset, &data, len);
}

void cache1_write(hwaddr_t addr, size_t len, uint32_t data) {
	int i;
	bool v = false;
	uint32_t group_nr = (addr >> 6) & 0x7f;
	uint32_t offset = addr & (BLOCK_SIZE - 1);
	for(i = 8 * group_nr; i < 8 * (group_nr + 1); i++) {
		if(((addr >> 13) == cache1[i].tag) && (cache1[i].valid == true)) {
			v = true;
			break;
		}
	}
	if(v == true) {
		memcpy(cache1[i].data + offset, &data, len);
	}
	cache2_write(addr, len, data);
}
