/*
 * eve_ram_g.h
 *
 *  Created on: 20 Nov 2017
 */

#ifndef SOURCES_EVE_RAM_G_H_
#define SOURCES_EVE_RAM_G_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void init_ram_g(uint32_t addr);
uint32_t malloc_ram_g(size_t size);
void free_ram_g(uint32_t addr);

#define DEBUG_RAMG_MALLOC(...) //printf(__VA_ARGS__)

#endif /* SOURCES_EVE_RAM_G_H_ */
