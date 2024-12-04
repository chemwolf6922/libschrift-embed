#ifndef _UTIL_H
#define _UTIL_H

#include <stddef.h>

/**
 * @brief Map file to memory
 * 
 * @param filename 
 * @return void*
 */
void* map_file(const char* filename, size_t *size);

void unmap_file(void* ptr, size_t size);

#endif
