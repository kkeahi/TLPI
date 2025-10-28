// (Advanced) Implement malloc() and free().

#include "/home/trenston/workspace/TLPI/tlpi-book/lib/tlpi_hdr.h"

struct block_header {
  size_t size;
  int free;
  struct block_header *next;
};

static struct block_header *heap_start = NULL;

static struct block_header *make_space(size_t size) {
  void *p = sbrk(sizeof(struct block_header) + size);
  if (p == (void *)-1) return NULL;

  struct block_header *block = (struct block_header *)p;
  block->size = size;
  block->free = 0;
  block->next = NULL;

  if (heap_start == NULL) heap_start = block;
  return block;
}

static struct block_header *find_space(size_t size)
{
  struct block_header *ptr = heap_start;
  while (ptr) {
    if (ptr->size >= size && ptr->free == 1) {
      return ptr;
    }

    ptr = ptr->next;
  }

  return NULL;
}

void *_malloc(size_t size) // Returns pointer to allocated memory on success, or NULL on error 
{
  if (size == 0) return NULL;

  size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

  struct block_header *block = find_space(size);
  if (!block) {
    block = make_space(size);
    if (!block) return NULL;

    if (block != heap_start) {
      struct block_header *tail = heap_start;
      while (tail->next) {
        tail = tail->next;
      }
      tail->next = block;
    }
  } else {
    block->free = 0;
  }

  return (void *)(block + 1);
}

void _free(void *ptr) {
  if (!ptr) return;

  struct block_header *block = ((struct block_header *)ptr) - 1;
  block->free = 1;
}

int main(void) {
  printf("Initial program break:        %p\n", sbrk(0));

  void *p = _malloc(10 * 1024);   // 10 KB
  printf("User pointer returned:        %p\n", p);
  printf("Program break after malloc:   %p\n", sbrk(0));

  _free(p);
  printf("Program break after free:     %p\n", sbrk(0));

  return 0;
}
