namespace mem {
void init( );
void *alloc(size_t size);
void free(void *addr);
void *realloc(void *addr, size_t size);
void *frame_alloc(uint64_t wanted_frames);
void frame_free(void *ptr, uint64_t frames);
void *frame_calloc(uint64_t frames);
} // namespace mem