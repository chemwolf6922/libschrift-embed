#ifndef _UTIL_H
#define _UTIL_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Map file to memory
 * 
 * @param filename 
 * @return void*
 */
void* map_file(const char* filename, size_t *size);

void unmap_file(void* ptr, size_t size);

int utf8_to_utf32(const uint8_t *utf8, uint32_t *utf32, int max);

#endif
