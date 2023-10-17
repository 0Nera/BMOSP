#include <fb.h>
#include <mem.h>
#include <tool.h>
extern void *bootpng_ptr;
extern uint64_t bootpng_size;
extern "C" {

void *realloc(void *pointer, int new_size) {
	return mem::realloc(pointer, new_size);
}

void free(void *pointer) {
	free(pointer);
}
#define memcpy(x, y, z) tool::memcpy(x, y, z)
#define memset(x, y, z) tool::memset(x, y, z)
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) assert(x)
#define malloc(sz)                                                             \
	mem::alloc(sz);                                                            \
	fb::printf("malloc(%uКБ) %s, %s, %u\n", sz / 1024, __FILE__, __FUNCTION__, \
	           __LINE__);
#define STBI_MALLOC(sz) malloc(sz)
#define STBI_REALLOC(p, newsz) realloc(p, newsz)
#define STBI_FREE(p) free(p)
#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_NO_SIMD
#define STBI_ONLYPNG
#include "stb_image.h"

unsigned int *png_parse(unsigned char *ptr, int size) {
	int i, w, h, f;
	unsigned char *img, *p;
	stbi__context s;
	stbi__result_info ri;
	s.read_from_callbacks = 0;
	s.img_buffer = s.img_buffer_original = ptr;
	s.img_buffer_end = s.img_buffer_original_end = ptr + size;
	ri.bits_per_channel = 8;
	img = p = (unsigned char *)stbi__png_load(&s, (int *)&w, (int *)&h,
	                                          (int *)&f, 1, &ri);
	unsigned int *data =
	    (unsigned int *)malloc((w * h + 2) * sizeof(unsigned int));
	fb::printf("Выделено %u\n", (w * h + 2) * sizeof(unsigned int));
	pause( );
	pause( );
	pause( );
	pause( );
	if (!data) {
		// free(img);
		return NULL;
	}
	for (i = 0; i < w * h; i++, p += f) switch (f) {
			case 1:
				data[2 + i] = 0xFF000000 | (p[0] << 16) | (p[0] << 8) | p[0];
				break;
			case 2:
				data[2 + i] = (p[1] << 24) | (p[0] << 16) | (p[0] << 8) | p[0];
				break;
			case 3:
				data[2 + i] = 0xFF000000 | (p[0] << 16) | (p[1] << 8) | p[2];
				break;
			case 4:
				data[2 + i] = (p[3] << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
				break;
		}
	// free(img);
	data[0] = w;
	data[1] = h;
	return data;
}

void main( ) {
	fb::printf("Загрузка завершена! 1\n");
	unsigned int *data = png_parse((unsigned char *)bootpng_ptr, bootpng_size);
	fb::printf("Загрузка завершена! 2 %x\n", data);
}
}