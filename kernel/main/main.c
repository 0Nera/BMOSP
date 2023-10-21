#include <fb.h>
#include <mem.h>
#include <tool.h>
extern void *bootpng_ptr;
extern uint64_t bootpng_size;

typedef struct {
	unsigned char magic1;             // must be zero
	unsigned char colormap;           // must be zero
	unsigned char encoding;           // must be 2
	unsigned short cmaporig, cmaplen; // must be zero
	unsigned char cmapent;            // must be zero
	unsigned short x;                 // must be zero
	unsigned short y;                 // image's height
	unsigned short h;                 // image's height
	unsigned short w;                 // image's width
	unsigned char bpp;                // must be 32
	unsigned char pixeltype;          // must be 40
} __attribute__((packed)) tga_header_t;

unsigned int *tga_parse(unsigned char *ptr, int size) {
	unsigned int *data;
	int i, j, k, x, y, w = (ptr[13] << 8) + ptr[12],
	                   h = (ptr[15] << 8) + ptr[14],
	                   o = (ptr[11] << 8) + ptr[10];
	int m = ((ptr[1] ? (ptr[7] >> 3) * ptr[5] : 0) + 18);

	if (w < 1 || h < 1) return NULL;

	data = (unsigned int *)mem_alloc((w * h + 2) * sizeof(unsigned int));
	if (!data) {
		fb_printf("Err %u, %x, %u kb\n", __LINE__, data,
		          ((w * h + 2) * sizeof(unsigned int)) / 1024);
		return NULL;
	}

	switch (ptr[2]) {
		case 1:
			if (ptr[6] != 0 || ptr[4] != 0 || ptr[3] != 0 ||
			    (ptr[7] != 24 && ptr[7] != 32)) {
				fb_printf("Err %u\n", __LINE__);
				mem_free(data);
				return NULL;
			}
			for (y = i = 0; y < h; y++) {
				k = ((!o ? h - y - 1 : y) * w);
				for (x = 0; x < w; x++) {
					j = ptr[m + k++] * (ptr[7] >> 3) + 18;
					data[2 + i++] = ((ptr[7] == 32 ? ptr[j + 3] : 0xFF) << 24) |
					                (ptr[j + 2] << 16) | (ptr[j + 1] << 8) |
					                ptr[j];
				}
			}
			break;
		case 2:
			if (ptr[5] != 0 || ptr[6] != 0 || ptr[1] != 0 ||
			    (ptr[16] != 24 && ptr[16] != 32)) {
				fb_printf("Err %u\n", __LINE__);
				mem_free(data);
				return NULL;
			}
			for (y = i = 0; y < h; y++) {
				j = ((!o ? h - y - 1 : y) * w * (ptr[16] >> 3));
				for (x = 0; x < w; x++) {
					data[2 + i++] =
					    ((ptr[16] == 32 ? ptr[j + 3] : 0xFF) << 24) |
					    (ptr[j + 2] << 16) | (ptr[j + 1] << 8) | ptr[j];
					j += ptr[16] >> 3;
				}
			}
			break;
		case 9:
			if (ptr[6] != 0 || ptr[4] != 0 || ptr[3] != 0 ||
			    (ptr[7] != 24 && ptr[7] != 32)) {
				fb_printf("Err %u\n", __LINE__);
				mem_free(data);
				return NULL;
			}
			y = i = 0;
			for (x = 0; x < w * h && m < size;) {
				k = ptr[m++];
				if (k > 127) {
					k -= 127;
					x += k;
					j = ptr[m++] * (ptr[7] >> 3) + 18;
					while (k--) {
						if (!(i % w)) {
							i = ((!o ? h - y - 1 : y) * w);
							y++;
						}
						data[2 + i++] =
						    ((ptr[7] == 32 ? ptr[j + 3] : 0xFF) << 24) |
						    (ptr[j + 2] << 16) | (ptr[j + 1] << 8) | ptr[j];
					}
				} else {
					k++;
					x += k;
					while (k--) {
						j = ptr[m++] * (ptr[7] >> 3) + 18;
						if (!(i % w)) {
							i = ((!o ? h - y - 1 : y) * w);
							y++;
						}
						data[2 + i++] =
						    ((ptr[7] == 32 ? ptr[j + 3] : 0xFF) << 24) |
						    (ptr[j + 2] << 16) | (ptr[j + 1] << 8) | ptr[j];
					}
				}
			}
			break;
		case 10:
			if (ptr[5] != 0 || ptr[6] != 0 || ptr[1] != 0 ||
			    (ptr[16] != 24 && ptr[16] != 32)) {
				fb_printf("Err %u\n", __LINE__);
				mem_free(data);
				return NULL;
			}
			y = i = 0;
			for (x = 0; x < w * h && m < size;) {
				k = ptr[m++];
				if (k > 127) {
					k -= 127;
					x += k;
					while (k--) {
						if (!(i % w)) {
							i = ((!o ? h - y - 1 : y) * w);
							y++;
						}
						data[2 + i++] =
						    ((ptr[16] == 32 ? ptr[m + 3] : 0xFF) << 24) |
						    (ptr[m + 2] << 16) | (ptr[m + 1] << 8) | ptr[m];
					}
					m += ptr[16] >> 3;
				} else {
					k++;
					x += k;
					while (k--) {
						if (!(i % w)) {
							i = ((!o ? h - y - 1 : y) * w);
							y++;
						}
						data[2 + i++] =
						    ((ptr[16] == 32 ? ptr[m + 3] : 0xFF) << 24) |
						    (ptr[m + 2] << 16) | (ptr[m + 1] << 8) | ptr[m];
						m += ptr[16] >> 3;
					}
				}
			}
			break;
		default: {
			fb_printf("Err %u\n", __LINE__);
			mem_free(data);
			return NULL;
		}
	}
	data[0] = w;
	data[1] = h;
	return data;
}
void main( ) {
	for (uint64_t i = 512; i > 1; i--) { pause( ); }
	fb_printf("Загрузка завершена! 1\n");
	unsigned int *res = tga_parse((uint8_t *)bootpng_ptr, bootpng_size);
	fb_printf("Загрузка завершена! 2 %x\n", res);

	tga_header_t *head = (tga_header_t *)bootpng_ptr;

	if (res != NULL) {
		fb_printf("Размер экрана загрузки: %ux%u \n", res[0], res[1]);
	}
	fb_printf("Размер экрана загрузки: %ux%u \n", head->h, head->w);
	mem_dump_memory( );

	fb_print_buf(0, 0, head->w, head->h, (uint32_t *)(res + 2));
}