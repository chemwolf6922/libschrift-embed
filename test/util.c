#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void* map_file(const char* filename, size_t* size)
{
    if(!filename)
        return NULL;
    int fd = open(filename, O_RDONLY);
    if(fd < 0)
        return NULL;
    struct stat st;
    if(fstat(fd, &st) < 0)
    {
        close(fd);
        return NULL;
    }
    void* ptr = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if(ptr == MAP_FAILED)
        return NULL;
    *size = (size_t)st.st_size;
    return ptr;
}

void unmap_file(void* ptr, size_t size)
{
    if(ptr)
        munmap(ptr, size);
}