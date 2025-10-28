// (Advanced) Implement malloc() and free().

#include "/home/trenston/workspace/TLPI/tlpi-book/lib/tlpi_hdr.h"

struct MemoryBlock {
  void *ptr;
  int size;
};

void *_malloc(size_t size) // Returns pointer to allocated memory on success, or NULL on error 
{
  return;
}

void free(void *ptr) 
{
  return;
}

int
main()
{
  printf("Program break: %p", sbrk(0));
}